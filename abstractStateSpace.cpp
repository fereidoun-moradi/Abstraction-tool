#include <iostream>
#include <fstream>
#include <cstring>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <regex>

using namespace std;

int extractInfo(vector<string> stateVars, vector<string> stateQmsgs, string fileName, vector<string> &allTransitions, vector<string> &allQmsgs, int &allQmsgsNumbers){
	ifstream readFile;
	ifstream readFileQ;
	readFile.open(fileName.c_str());
	readFileQ.open(fileName.c_str());

	ofstream writeTransitions;
	writeTransitions.open("transitions.txt");

	ofstream writeTransitionLog;
	writeTransitionLog.open("transitionLog.txt");

	ofstream writeStates;
	writeStates.open("states.txt");

	int stateCounter = 1;
	int stateQCounter = 1;	
	int transitionCounter = 1;
	allQmsgsNumbers = 0;

	ofstream writeStatesQ;
	writeStatesQ.open("statesQmsg.txt");
	
	string str="";
	string id = "id=";
	string fileContents="";
	while (getline(readFileQ, str)) {
		fileContents.append(str+"\n");
	}
    
	int numberOfIds = 0;
	for (size_t i = 0; (i = fileContents.find(id, i)) != std::string::npos; numberOfIds++, i++);
	//cout<< numberOfIds<<endl;

	while(stateQCounter <= numberOfIds){
		size_t startDEL = fileContents.find("<state id=\""+to_string(stateQCounter));
		size_t stopDEL = fileContents.find("<state id=\""+to_string(stateQCounter+1));
		str = fileContents.substr(startDEL+1, (stopDEL-startDEL-1));

		size_t first = str.find("(");
		//size_t last = str.find(")", first);
		//str.erase(first+1,last-first-1);

		while(first != string::npos)
		{
			size_t last = str.find(")", first);
			str.erase(first+1,last-first-1);
			first = str.find("(",first+1);
		}
		
		for(int i = 0; i < stateQmsgs.size(); i++){
			size_t found = str.find(stateQmsgs[i]);
			if (found != string::npos){
				writeStatesQ << "state: " << stateQCounter << "_0" << endl;
				//cout<<stateQCounter<<endl;
				allQmsgs.push_back(to_string(stateQCounter)+"_0");
				allQmsgsNumbers ++;
			}
		}
		stateQCounter++;
	}

	while(!readFile.eof()){

		int foundedVars = 0;

		string line;
		getline(readFile, line);

		size_t foundID = line.find("id=");

		if(	foundID != string::npos){
			string stateId = "";
			//sscanf(line.c_str(), "/\d+/", &stateId);
			stateId = regex_replace(line,regex("[^0-9]*([0-9]+).*"),string("$1"));

			size_t startPoint, endPoint;
			vector<string> result(stateVars.size());

			while(foundedVars != stateVars.size()){
				
				getline(readFile, line);

				vector <size_t> foundedVarAmount;

				for(int i = 0; i < stateVars.size(); i++)
					foundedVarAmount.push_back(line.find(stateVars[i]));


				for(int i = 0; i < stateVars.size(); i++){
					if(foundedVarAmount[i] != string::npos){
					
						foundedVars++;
					
						startPoint = line.find(">");
						endPoint = line.find("</");

						result[i] = line.substr(startPoint+1, (endPoint-startPoint-1));
					}					
				}


			}

			//writeStates << "state: " << stateCounter;
			writeStates << "state: " << stateId;

				for(int i = 0; i < stateVars.size(); i++){
					writeStates << '\t' << stateVars[i] << ": " << result[i];
				}

				writeStates << endl;
	
			stateCounter++;

		}

		else{
			size_t foundTransition = line.find("<transition ");

			if(	foundTransition != string::npos){
				
				writeTransitionLog << line << endl;
				allTransitions.push_back(line);
				stringstream ss(line);
				string source, destination, temp;

				ss >> temp >> source >> destination;
				writeTransitions << "Transition: " << transitionCounter << '\t' << source << "\t" << destination << endl;
				transitionCounter++;

			}

			else
				continue;

		}

	}

	readFile.close();
	readFileQ.close();
	writeTransitions.close();
	writeStates.close();
	writeTransitionLog.close();

	return (stateCounter-1);
}


vector< vector< string > > orderedStates(int NumOfState){

	vector< vector< string > > transitions (NumOfState);

	ifstream readFile;
	readFile.open("transitions.txt");

	while(!readFile.eof()){

		string line;
		getline(readFile, line);

		if(line != ""){
			stringstream ss(line);

			string temp, source, destination;
			ss >> temp >> temp >> source >> destination;

			size_t startPoint, endPoint;

			startPoint = source.find("\"");
			endPoint = source.find("_");
			source = source.substr( (startPoint + 1) , endPoint-startPoint-1);

			startPoint = destination.find("\"");
			endPoint = destination.find("_");
			destination = destination.substr( (startPoint + 1) , endPoint-startPoint+1);

			stringstream sourceNumber;
			sourceNumber << source;

			int sourceNum;
			sourceNumber >> sourceNum;

			transitions[sourceNum-1].push_back(destination);

		}
	}

	return transitions;

}

vector< vector< string > > deleteExtras(vector< vector< string > > transitions, int NumOfState){

	ofstream statesCombination;
	statesCombination.open("statesCombination.txt");

	for(int i = 0; i < NumOfState; i++){
		stringstream intState;
		intState << i+1;
		string state = intState.str() + "_0";

		bool minus = false;
		int coveredStates = 0;

		for(int j = 0; j < transitions[i].size(); j++){

			if(minus == true){
				j--;
				minus = false;
			}


			if(transitions[i][j] == state){
				transitions[i].erase(transitions[i].begin() + j);
				minus = true;
				coveredStates++;
				continue;
			}

			for(int k = j+1; k < transitions[i].size(); k++){

				if(transitions[i][j] == transitions[i][k]){
					transitions[i].erase(transitions[i].begin() + j);
					minus = true;
					break;
				}

			}
		}	

		statesCombination << coveredStates << endl;
	}

	statesCombination.close();

	return transitions;

}

bool compareStrings(string s1, string s2){
	for (unsigned int n=0; n<s1.length(); ++n){
		if(s1[n] != s2[n])
			return false;
	}
	return true;
}

void reduce(int numOfVars, int NumOfState, map<string, string> &replaceMap, vector<string> allQmsgs, int allQmsgsNumbers, vector<string> stateQmsgs){


	vector<bool> checked(NumOfState, false);
	vector<bool> checkedQflag(NumOfState, false);
	vector<string> allStates;
	vector<string> allStatesIds;	
	//std::string allStatesQ[35] = { "1_0", "2_0", "3_0", "4_0", "5_0", "8_0", "9_0","10_0", "11_0", "12_0", "13_0", "14_0","15_0", "16_0", "17_0" , "21_0", "22_0", "23_0","24_0","25_0","26_0","27_0","28_0","29_0","32_0","33_0","34_0","35_0","38_0","39_0","41_0","42_0","43_0","44_0","45_0" };
	

	ofstream logFile;
	logFile.open("log.txt");

	ifstream readFile;
	readFile.open("states.txt");
	
	ifstream transitionLog;
	transitionLog.open("transitionLog.txt");
	string str="";
	string transitionfileContents="";
	while (getline(transitionLog, str)) {
		transitionfileContents.append(str+"\n");
		//cout<< allQmsgs[3] << endl;
		for(int i = 0; i < allQmsgsNumbers; i++){
			size_t fd = str.find("<transition source=\""+ allQmsgs[i] + "\" " + "destination=\"");
			if(fd != string::npos){
				size_t startPoint = str.find("title=\"");
				size_t endPoint = str.find("\"/></");
				string trans = str.substr(startPoint+7, (endPoint-startPoint-7));
				int ctr = 1;
				while(ctr!=stateQmsgs.size()){
					if(trans+"()" == stateQmsgs[ctr]){
						size_t startPoint = str.find("destination=\"");
						size_t endPoint = str.find("_0\" exe");
						string trans = str.substr(startPoint+13, (endPoint-startPoint-13));
						checkedQflag[stoi(trans)] = true;
						cout<< trans << endl;
					}
					ctr++;
				}			
			}
		}
	}

	string line;

	while(!readFile.eof()){

		getline(readFile, line);
		stringstream ss(line);
		string temp, stateID;

		ss >> temp >> stateID;
		allStatesIds.push_back(regex_replace(line,regex("[^0-9]*([0-9]+).*"),string("$1")));
		

		if(line.length() != 0){
			int size = temp.length() + stateID.length() + 1;
			string remain = line.substr(size);
			allStates.push_back(remain);
		}
	}
	
	for(int i = 0; i < NumOfState; i++){
		if(checked[stoi(allStatesIds[i])] == false){
			for(int j = i+1; j < NumOfState; j++){
				if(checked[stoi(allStatesIds[j])] == false){
					if(allStates[i] == allStates[j] && checkedQflag[stoi(allStatesIds[i])] != true){
						//size_t found1 = transitionfileContents.find("<transition source=\""+ allStatesIds[i] + "_0\" " + "destination=\"" + allStatesIds[j] + "_0\"");
						//size_t found2 = transitionfileContents.find("<transition source=\""+ allStatesIds[j] + "_0\" " + "destination=\"" + allStatesIds[i] + "_0\"");
						//if(found1 != string::npos || found2 != string::npos){
							stringstream sAlt;
							sAlt << i+1;
							string alternativeState = sAlt.str() + "_0";

							stringstream sReplace;
							sReplace << j+1;
							string stateToReplace = sReplace.str() + "_0";

							//logFile << stateToReplace << " replaced_with " << alternativeState << endl;
							logFile << allStatesIds[j] + "_0" << " replaced_with " << allStatesIds[i] + "_0" << endl;
							
							//replaceMap.insert(pair<string, string>(stateToReplace, alternativeState));
							replaceMap.insert(pair<string, string>( allStatesIds[j] + "_0", allStatesIds[i] + "_0"));
							checked[stoi(allStatesIds[j])] = true;
							
							for(int k = 0; k < allQmsgsNumbers; k++){
								if(stateToReplace == allQmsgs[k]){
								//if(allStatesIds[j] == allQmsgs[k]){
									checkedQflag[stoi(allStatesIds[i])] = true;
								}
							}
						//}
					} 		
				}
			}
			checked[stoi(allStatesIds[i])] = true;
		}
	}

	readFile.close();
	logFile.close();

}

void readInputFile(vector<string> &stateVars, vector<string> &stateQmsgs, string &fileName1, string inputFile1, string inputFile2){

	ifstream readFile1;
	readFile1.open(inputFile1.c_str());

	string line;
	getline(readFile1, line);

	stringstream ss(line);

	int numOfVars;
	ss >> numOfVars;



	ifstream readFile2;
	readFile2.open(inputFile2.c_str());

	string line2;
	getline(readFile2, line2);

	stringstream ssQ(line2);

	int numOfQmsgs;
	ssQ >> numOfQmsgs;

	for(int i = 0 ; i < numOfVars; i++){
		
		getline(readFile1, line);
		stringstream data(line);

		string variable;
		data >> variable;
		
		stateVars.push_back(variable);
	}

	getline(readFile1, line);
	stringstream fileData(line);
	fileData >> fileName1;

	
	
	for(int i = 0 ; i < numOfQmsgs; i++){
		
		getline(readFile2, line2);
		stringstream data(line2);

		string variable;
		data >> variable;
		
		stateQmsgs.push_back(variable);
	}

}

string makeTransition(string source, string destination, string exeTime, string title){

	string transition = "<transition " + source + " " +   destination + " " + exeTime + " shift=\"0\"> <messageserver sender=\"\" owner=\"\" " + title;
	return transition;
}

bool existTransition(vector<string> remainedTransitions, string transition){
	for(int i = 0; i < remainedTransitions.size(); i++){
		if(remainedTransitions[i] == transition)
			return true;
	}

	return false;
}

vector<string> editTransitions(map<string, string> replaceMap, vector<string> allTransitions){
	map<string,string>::iterator it = replaceMap.begin();

	for (it=replaceMap.begin(); it!=replaceMap.end(); ++it){

		for(int i = 0; i < allTransitions.size(); i++){
			string word = "\"" + it->first + "\"";
			string replaceWord = "\"" + it->second + "\"";
			size_t foundState = allTransitions[i].find(word);

			if(	foundState != string::npos){
				allTransitions[i].erase( foundState, word.length());
				allTransitions[i].insert( foundState, replaceWord);
			}
			
			foundState = allTransitions[i].find(word);
			if(	foundState != string::npos){
				allTransitions[i].erase( foundState, word.length());
				allTransitions[i].insert( foundState, replaceWord);
			}
	
		}

	}
	//for(int i = 0; i < allTransitions.size(); i++){cout<<allTransitions[i]<<endl;}
	vector<string> remainedTransitions;
//cout<<".........."<<endl;
//*
string strTemp;
for(int i = 0; i < allTransitions.size(); i++){
	size_t fd = strTemp.find(allTransitions[i]);
	if(fd == string::npos){
		remainedTransitions.push_back(allTransitions[i]);
		strTemp.append(allTransitions[i]);
	}
}

//for(int i = 0; i < remainedTransitions.size(); i++){cout<<remainedTransitions[i]<<endl;}
return remainedTransitions;
//*/
/*
	for(int i = 0; i < allTransitions.size(); i++){

		size_t foundTitleBegin = allTransitions[i].find("title=\"");

		size_t unWanted = allTransitions[i].find("tau=>");

		if(foundTitleBegin != string::npos && unWanted == string::npos){
			string title = allTransitions[i].substr(foundTitleBegin);

			stringstream ss(allTransitions[i]);
			string temp, source, destination, exeTime;
			ss >> temp >> source >> destination >> exeTime;

			string transition = makeTransition(source, destination, exeTime, title);

			if(!existTransition(remainedTransitions, transition))
				remainedTransitions.push_back(transition);
		}
	}
	for(int i = 0; i < remainedTransitions.size(); i++){cout<<remainedTransitions[i]<<endl;}
	return remainedTransitions;

*/
}

bool deleteState(string stateID, map<string, string> replaceMap){
	map<string,string>::iterator it = replaceMap.begin();

	for (it=replaceMap.begin(); it!=replaceMap.end(); ++it){
		if(it->first == stateID)
			return true;
	}

	return false;

}

string replacedWithState(string stateID, map<string, string> replaceMap){
	map<string,string>::iterator it = replaceMap.begin();
	string itSecond = "null";
	for (it=replaceMap.begin(); it!=replaceMap.end(); ++it){
		if(it->first == stateID)
			return itSecond = it->second;
	}

	return itSecond;

}

void buildStateSpace(map<string, string> replaceMap, vector<string> remainedTransitions, string fileName){

	ifstream readFile;
	readFile.open(fileName.c_str());


	ifstream readFileQ;
	readFileQ.open(fileName.c_str());

	ofstream redStateSpace;
	redStateSpace.open("miniModel.statespace");
	
	int counter = 1;
	int dcCounter = 1;
	int sPoint, ePoint;
	vector<string>  deletedStateQueue;
	vector<string>  replacewithId;
	vector<string>  stateTime;

	string str="";
	string fileContents="";
	while (getline(readFileQ, str)) {
		fileContents.append(str+"\n");
	}

	readFileQ.clear();
	readFileQ.seekg (0);



	while(!readFileQ.eof()){
		string lineQ;
		getline(readFileQ, lineQ);
		size_t foundID = lineQ.find("id=");

		
		if(	foundID != string::npos){
			stringstream ss(lineQ);
			string temp, stateID;

			ss >> temp >> stateID;
			stateID = stateID.substr(4, stateID.length()-5);
			string replacewith = replacedWithState(stateID, replaceMap);

			if(replacewith != "null"){
				size_t sPoint, ePoint;
				while(true){
					getline(readFileQ, lineQ);
					sPoint = lineQ.find("<queue>");
					
					if(sPoint != string::npos){
						getline(readFileQ, lineQ);
						ePoint = lineQ.find("</queue>");
						if(ePoint != string::npos){
							break;
						} else {
							while(true){
								size_t eePoint;
								replacewithId.push_back(replacewith);
								deletedStateQueue.push_back(lineQ);
								getline(readFileQ, lineQ);
								eePoint = lineQ.find("</queue>");
								if(eePoint != string::npos){
									break;
								}
							}
							break;
						}
						
					}
											
				}
			}
		}
	}
	
	

	while(!readFile.eof()){

		
		string line;
		getline(readFile, line);

		size_t foundID = line.find("id=");

		if(	foundID != string::npos){
			

			stringstream ss(line);
			string temp, stateID;

			ss >> temp >> stateID;
			stateID = stateID.substr(4, stateID.length()-5);
			
			if(!deleteState(stateID, replaceMap)){
				size_t startPoint, endPoint;
				size_t sqPoint, eqPoint, nowPoint;
				redStateSpace << line << endl;
				
				while(true){
					getline(readFile, line);
					redStateSpace << line << endl;
				////	
					sqPoint = line.find("<queue>");
					if(sqPoint != string::npos){
						int ctr = 0;
						while(ctr != replacewithId.size()){
							if(stateID == replacewithId[ctr]){
								redStateSpace << deletedStateQueue[ctr]<<endl;
								//cout<<replacewithId[ctr]<<endl;
							}
							ctr++;
						}
					}
				////	
					
					nowPoint = line.find("<now>");
					if(nowPoint != string::npos){
						stateTime.push_back(line);
						//cout<<line<<endl;
					}
					
				////
					endPoint = line.find("</state>");
					if(endPoint != string::npos)
						break;					
				}
				
			}
		}
		
		if(counter == 1)
			redStateSpace << line << endl;	

		counter++;
	}

	for(int i = 0; i < remainedTransitions.size(); i++){	
		redStateSpace << remainedTransitions[i] << endl;
	}

	redStateSpace << "</transitionsystem>" << endl;

	
	readFile.close();
	readFileQ.close();
	redStateSpace.close();
}

	
/*
	fstream stateSpace;
	stateSpace.open("miniModel.statespace");
	size_t cotPoint;
	int ctr = 0;
	while(!stateSpace.eof()){
		string line;
		getline(stateSpace, line);
		size_t foundID = line.find("atomicpropositions");
		if(	foundID != string::npos){
			cotPoint = line.find("\" >");
			cout<<line<<endl;
			line.insert(cotPoint-1, stateTime[ctr]); 
			cout<<stateTime[ctr]<<endl;
			//line.erase(cotPoint+1,cotPoint+3);
			//stateSpace << stateTime[ctr] << ", >" <<endl;
			ctr++;
		}
	}
	stateSpace.close();
*/


int main(){

	vector<string> stateVars;
	vector<string> stateQmsgs;
	vector<string> allTransitions;
	vector<string> allQmsgs;
	int allQmsgsNumbers;
	map<string, string> replaceMap;
	string fileName;

	readInputFile(stateVars, stateQmsgs, fileName, "inputVar.txt", "inputQmsg.txt");
	int NumOfState = extractInfo(stateVars, stateQmsgs, fileName, allTransitions, allQmsgs, allQmsgsNumbers);
	reduce(stateVars.size(), NumOfState, replaceMap, allQmsgs, allQmsgsNumbers, stateQmsgs);
	vector<string> remainedTransitions = editTransitions(replaceMap, allTransitions);
	buildStateSpace(replaceMap, remainedTransitions, fileName);
	
}
