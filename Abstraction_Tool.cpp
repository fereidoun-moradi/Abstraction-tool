#include <iostream>
#include <fstream>
#include <cstring>
#include <sstream>
#include <vector>
#include <string>

using namespace std;

vector< vector< string > > deleteExtras(vector< vector< string > > transitions, int NumOfState){

	ofstream statesCombination;
	statesCombination.open("statesCombination.txt");

	for(int i = 0; i < NumOfState; i++){
		stringstream intState;
		intState << i+1;
		string state = intState.str() + "_0";

		bool minus = false;
		int coveredStates = 0;
		// cerr << "state " << i+1 << endl; 
		// cerr << transitions[i].size() << endl;

		for(int j = 0; j < transitions[i].size(); j++){

			if(minus == true){
				j--;
				minus = false;
			}

			// cerr << i+1 << " " << transitions[i][j] << endl;
			// cerr << j << endl;

			if(transitions[i][j] == state){
				transitions[i].erase(transitions[i].begin() + j);
				minus = true;
				coveredStates++;
				// cerr << "minus here: " << minus << endl;
				// cerr << transitions[i].size() << endl;
				continue;
			}

			for(int k = j+1; k < transitions[i].size(); k++){

				if(transitions[i][j] == transitions[i][k]){
					transitions[i].erase(transitions[i].begin() + j);
					minus = true;
					break;
				}

			}

			// cerr << "minus: " << minus << endl;
			// cerr << transitions[i].size() << endl;	
		}	

		statesCombination << coveredStates << endl;
	}

	statesCombination.close();

	return transitions;

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

int extractInfo(vector<string> stateVars, string fileName){

	ifstream readFile;
	readFile.open(fileName.c_str());

	ofstream writeTransitions;
	writeTransitions.open("transitions.txt");

	ofstream writeStates;
	writeStates.open("states.txt");


	int stateCounter = 1;	
	int transitionCounter = 1;

	while(!readFile.eof()){

		int foundedVars = 0;

		string line;
		getline(readFile, line);

		size_t foundID = line.find("id=");
		int firstTime = 1;

		if(	foundID != string::npos){
			
			size_t startPoint, endPoint;
			vector<string> result(stateVars.size());

			while(foundedVars != stateVars.size()){
				
				getline(readFile, line);

				vector <size_t> foundedVarAmount;



				std::size_t pos = line.find("<now>");
				if (pos != std::string::npos){
					if(firstTime == 1){
						line.replace(pos, 5, "\"<variable name=\"now\">");
						firstTime = firstTime + 1 ;
					} else {
						getline(readFile, line);
					}
				}	


				for(int i = 0; i < stateVars.size(); i++)
					foundedVarAmount.push_back(line.find(stateVars[i]));
				


				for(int i = 0; i < stateVars.size(); i++){
					if(foundedVarAmount[i] != string::npos){
						//cout<<line<<endl;
						foundedVars++;
					
						startPoint = line.find(">");
						endPoint = line.find("</");

						result[i] = line.substr(startPoint+1, (endPoint-startPoint-1));
					}					
				}
			}

			writeStates << "state: " << stateCounter;

				for(int i = 0; i < stateVars.size(); i++){
					writeStates << '\t' << stateVars[i] << ": " << result[i];
				}

				writeStates << endl;

			stateCounter++;

		}

		else{
			size_t foundTransition = line.find("<transition ");

			if(	foundTransition != string::npos){
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
	writeTransitions.close();
	writeStates.close();

	return (stateCounter-1);
}

bool compareStrings(string s1, string s2){
	for (unsigned int n=0; n<s1.length(); ++n){
		if(s1[n] != s2[n])
			return false;
	}
	return true;
}

vector< vector< string > > reduce(int numOfVars, int NumOfState){
	
	vector<string> allStates;
	vector< vector< string > > transitions (NumOfState);
	transitions = orderedStates(NumOfState);	

	ofstream logFile;
	logFile.open("log.txt");

	ifstream readFile;
	readFile.open("states.txt");

	string line;

	for(int i = 0; i < NumOfState; i++){
		for(int j = 0; j < transitions[i].size(); j++)
			logFile << i+1 << '\t' << transitions[i][j] << endl;
	}


	while(!readFile.eof()){

		getline(readFile, line);
		allStates.push_back(line);

	}

	for(int i = 0 ; i < NumOfState; i++){

		logFile << "step " << i+1 << endl;
		// int currentSize = transitions[i].size();

		bool minus = false;

		for(int j = 0; j < transitions[i].size(); j++){
			string temp;
			stringstream state(allStates[i]);
			vector<string> currentRoomTemps;

			state >> temp >> temp;

			if(minus){
				j = j-1;
				minus = false;
			}

			stringstream nState(transitions[i][j]);
			int nextState;
			nState >> temp;
			temp = temp.substr(0, temp.length()-2);
			stringstream ss(temp);
			ss >> nextState;

			stringstream nextStateLine(allStates[nextState-1]);
			nextStateLine >> temp >> temp;

			bool equal = true;
			int count = 0;
			// cerr << i+1 << endl;

			while(count < numOfVars){
				if(equal){
					for(int k = 0; k < numOfVars; k++){
						string currentValue, nextValue;
						state >> temp >> currentValue;
						nextStateLine >> temp >> nextValue;

						equal *= compareStrings(currentValue, nextValue);

						//cerr << currentValue << endl << nextValue << endl;

						count++;
					}
				}
			}

			// cerr << "equal : " << equal << endl;

			if(equal){

				stringstream SS;
				SS << i+1;
				string replace = SS.str() + "_0";

				// cerr << transitions[i][j] << " replace with " << replace << endl;				
				logFile << transitions[i][j] << " replace with " << replace << endl;

				string value = transitions[i][j];


				//replace all destinations which are transition[i][j]
				for(int l = 0; l < NumOfState; l++){
					for(int m = 0; m < transitions[l].size(); m++){
						if(transitions[l][m] == value)
							transitions[l][m] = replace;
					}
				}


				// stringstream sReplace(transitions[i][j]);
				stringstream sReplace(value);
				string stateToReplace = sReplace.str();

				stateToReplace = stateToReplace.substr(0, stateToReplace.length()-2);
				stringstream intSreplace(stateToReplace);

				// cerr << stateToReplace << endl;

				int replaceState;
				intSreplace >> replaceState;
				
				// cerr << replaceState-1 << endl;
				int nextCurrSize = transitions[replaceState-1].size();

				for(int o = 0; o < nextCurrSize; o++){
					string newState = transitions[replaceState-1].back();
					// cerr << newState << endl;
					transitions[replaceState-1].pop_back();
					if(newState != replace){
						transitions[i].push_back(newState);
					}
				}


				// transitions[i][j] = replace;
				// cerr << transitions[i].size() << endl;
				
				//transitions[i].erase(transitions[i].begin()+j);
				transitions[i][j] = replace;
				// cerr << value << endl;
				// cerr << transitions[i].size() << endl;
				// minus = true;

				//check if replace happened
				for(int h = 0; h < NumOfState; h++){
					for(int p = 0; p < transitions[h].size(); p++)
						logFile << h+1 << '\t' << transitions[h][p] << endl;
				}

			}

		}

	}	

	transitions = deleteExtras(transitions, NumOfState);
	
	logFile << "Final version" << endl;

	for(int i = 0; i < NumOfState; i++){
		for(int j = 0; j < transitions[i].size(); j++)
			logFile << i+1 << '\t' << transitions[i][j] << endl;
	}


	readFile.close();

	return transitions;
}

bool findState(vector<string> remainedStates, string stateID){

	for(int i = 0; i < remainedStates.size(); i++){
		if(remainedStates[i] == stateID)
			return true;
	}

	return false;

}

vector<string> extractRemainedStates(vector< vector< string > > remainedStateSpace, int NumOfState){

	vector<string> states;

	string firstState = "1_0";
	states.push_back(firstState);

	for(int i = 0; i < NumOfState; i++){
		for(int j = 0; j < remainedStateSpace[i].size(); j++){
			// if(!findState(states, remainedStateSpace[i][j])){ 
				states.push_back(remainedStateSpace[i][j]);
				// cerr << remainedStateSpace[i][j] << endl;
			// }
		}
	}

	return states;

}

string makeTransition(string source, string destination){

	string transition = "<transition source=\"" + source + "\" destination=\"" +   destination + "\" executionTime=\"0\" shift=\"0\"> <messageserver sender=\"\" owner=\"\" title=\"\"/></transition>";
	return transition;
}

vector<string> extractRemainedTransitions(vector< vector< string > > remainedStateSpace, vector<string> statesSuffix, int NumOfState){

	vector<string> transitions;

	for(int i = 0; i < NumOfState; i++){
		stringstream ss;
		ss << i+1;
		string source = ss.str() + "_" + statesSuffix[i];
		
		for(int j = 0; j < remainedStateSpace[i].size(); j++){
			string prefix = remainedStateSpace[i][j].substr(0, remainedStateSpace[i][j].length()-2);
			// cerr << "prefix " << prefix << endl;
			stringstream ss2(prefix);
			int destinationNumber;
			ss2 >> destinationNumber;
			string destination = prefix + "_" + statesSuffix[destinationNumber-1];
			// cerr << statesSuffix[destinationNumber-1] << endl;
			// cerr << destination << endl;
			string Transition = makeTransition(source, destination);
			// cerr << Transition << endl;
			transitions.push_back(Transition);	
		}
	}

	return transitions;

}


void reduceStateSpace(vector<string> remainedStates, vector<string> remainedTransitions, vector<string> statesSuffix, string fileName){
	
	ifstream readFile;
	readFile.open(fileName.c_str());

	ofstream redStateSpace;
	redStateSpace.open("miniModel.statespace");

	int counter = 1;

	while(!readFile.eof()){

		string line;
		getline(readFile, line);

		size_t foundID = line.find("id=");
		size_t foundTransition = line.find("<transition");

		if(	foundID != string::npos){
			
			stringstream ss(line);
			string temp, stateID;

			ss >> temp >> stateID;
			size_t startPoint, endPoint;

			endPoint = stateID.find("_0");
			stateID = stateID.substr(4, (endPoint-2) );
			
			stringstream ssInt(stateID);
			int intStateID;
			ssInt >> intStateID;

			if(findState(remainedStates, stateID)){
				if(statesSuffix[intStateID-1] != "0"){
					string endLine = line;
					// cerr << line << endl;
					// cerr << endLine << endl;
					endPoint += 2;
					string newLine = line.substr(0, (foundID + 3 + stateID.length())) + statesSuffix[intStateID-1] + endLine.substr(foundID + 4 + stateID.length());
					// cerr << endLine.substr(foundID + 4 + stateID.length()) << endl;
					line = newLine;
				}

				redStateSpace << line << endl;
				while(true){
					getline(readFile, line);
					redStateSpace << line << endl;
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
	redStateSpace.close();

}

vector<string> returnCombinations(){
	ifstream statesCombination;
	statesCombination.open("statesCombination.txt");	
	vector<string> statesSuffix;

	while(!statesCombination.eof()){
		string line, suffix;
		getline(statesCombination, line);
		stringstream ss(line);
		ss >> suffix;
		statesSuffix.push_back(suffix);
	}

	return statesSuffix;
}

int main(){

	string inputFile = "input.txt";
	ifstream readFile;
	readFile.open(inputFile.c_str());

	string line;
	getline(readFile, line);

	stringstream ss(line);

	int numOfVars;
	ss >> numOfVars;

	vector<string> stateVars;

	for(int i = 0 ; i < numOfVars; i++){
		
		getline(readFile, line);
		stringstream data(line);

		string variable;
		data >> variable;
		
		stateVars.push_back(variable);
	}

	getline(readFile, line);
	stringstream fileData(line);
	string fileName;
	fileData >> fileName;


	int NumOfState = extractInfo(stateVars, fileName);
	vector< vector< string > > remainedStateSpace = reduce(numOfVars, NumOfState);
	vector<string> statesSuffix = returnCombinations();
	vector<string> remainedStates = extractRemainedStates(remainedStateSpace, NumOfState);
	vector<string> remainedTransitions = extractRemainedTransitions(remainedStateSpace, statesSuffix, NumOfState);
	reduceStateSpace(remainedStates, remainedTransitions, statesSuffix, fileName);
}
