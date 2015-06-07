#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <ctime>

using namespace std;

typedef pair<string, int> task;
typedef vector<task > tasklist;

const string timemgrFile = "timemgr.txt";
const string timemgrlogFile = "timemgrlog.txt";
map<string, string> alias;

// helper methods

void outputNiceTimespan(int timespan){
	int d, h, m, s;
	s = timespan % 60;
	timespan /= 60;
	m = timespan % 60;
	timespan /= 60;
	h = timespan % 24;
	timespan /= 24;
	d = timespan;

	if(d > 0){
		cout << d << "d " << h << "h " << m << "min " << s << "s";
	}else if(h > 0){
		cout << h << "h " << m << "min " << s << "s";
	}else if(m > 0){
		cout << m << "min " << s << "s";
	}else{
		cout << s << "s";
	}
}

void toLower(string& s){
	for(int i = 0; i < s.size(); i++){
		if(s[i] >= 'A' && s[i] <= 'Z'){
			s[i] -= 'A' - 'a';
		}
	}
}

void initAlias(){
	alias["l"] = "list";
	alias["."] = "list";
	alias["a"] = "add";
	alias["+"] = "add";
	alias["r"] = "remove";
	alias["rm"] = "remove";
	alias["-"] = "remove";
	alias["s"] = "start";
	alias[":"] = "start";
	alias["e"] = "end";
	alias[";"] = "end";
	alias["stop"] = "end";
	alias["st"] = "status";
	alias[","] = "status";
	alias["res"] = "reset";
	alias["_"] = "reset";
	alias["ar"] = "archive";
	alias["%"] = "archive";
	alias["++"] = "sum";
	alias["!"] = "sum";
	alias["rn"] = "rename";
	alias["cat"] = "changeactivetask";
	alias["h"] = "help";
	alias["?"] = "help";
	alias["/?"] = "help";
	alias["pa"] = "printalias";
}

string replaceAlias(string cmd){
	string ncmd = cmd;
	
	if(alias.find(ncmd) != alias.end()){
		ncmd = alias[ncmd];
	}
	
	return ncmd;
}

bool askConfirmation(string question){
	cout << question << endl;
	string response;
	do{
		cin >> response;
	} while(response != "y" && response != "n");
	
	return response == "y";
}

int findTaskNum(tasklist& tasks, string name){
	try{
		return  stoi(name);
	} catch (invalid_argument e){}
	
	for(int i = 0; i < tasks.size(); i++){
		if(tasks[i].first == name){
			return i;
		}
	}
	return -1;
}

bool fileExists(string filename){
	ifstream f(filename);
	if (f.good()) {
		f.close();
		return true;
	} else {
		f.close();
		return false;
	}
}

void readFile(string filename, tasklist& tasks, task& activeTask){
	// Read file
	ifstream fin(filename);
	int n;
	fin >> n;
	tasks.resize(n);
	for(int i = 0; i < n; i++){
		fin >> tasks[i].first >> tasks[i].second;
	}

	fin >> activeTask.first >> activeTask.second;

	fin.close();
}

void writeFile(string filename, tasklist& tasks, task& activeTask){
	ofstream fout(filename);
	fout << tasks.size() << endl;
	for(int i = 0; i < tasks.size(); i++){
		fout << tasks[i].first << " " << tasks[i].second << "\n";
	}

	fout << activeTask.first << " " << activeTask.second << endl;
	fout.close();
}

// official methods

void init(){
	if(fileExists(timemgrFile)){
		if(!askConfirmation("this will erase all your data. Continue? (y/n)")){
			return;
		}
	}
	ofstream fout (timemgrFile);
	fout << "0" << endl;
	fout << "null 0" << endl;
	fout.close();
	cout << "Done" << endl;

	ofstream log(timemgrlogFile);
	log << "TimeMgr log" << endl;
	log.close();
}

void help(){
	cout << "TimeMgr" << endl;
	cout << "(c) 2014 - 2015 Benjamin Schmid" << endl;
	cout << endl;
	cout << "Available commands:" << endl;
	cout << "init, list, add, remove, start, end, status, reset, archive, sum, "
	     << "rename, changeactivetask, help, printalias" << endl;
}

void printAlias(){
	for(auto it = alias.begin(); it != alias.end(); it++){
		cout << it->first << "\t -> \t" << it->second << endl;
	}
}

void list(tasklist& tasks, task& activeTask){
	if(tasks.size() > 0)	{
		cout << "tasks:" << endl;
		for(int i = 0; i < tasks.size(); i++){
			cout << i << ":\t" << tasks[i].first << "\t";
			outputNiceTimespan(tasks[i].second);
			cout << "\n";
		}

		cout << endl;
	}else{
		cout << "no tasks" << endl;
	}

	if(activeTask.first != "null")	{
		cout << "Active task:" << endl;
		cout << activeTask.first << "\t";
		outputNiceTimespan(time(0) - activeTask.second);
		cout << endl;
	}
}

void add(tasklist& tasks, string name){
	int tnum = findTaskNum(tasks, name);
	if(tnum < 0)	{
		tasks.push_back(make_pair(name, 0));
		cout << name << " added" << endl;
	}else{
		cout << "task already exists" << endl;
	}
}

void remove(tasklist& tasks, task& activeTask, string name){
	int tnum = findTaskNum(tasks, name);
	if(tnum >= 0){
		if(askConfirmation("this will remove the task '" + name + "' . Continue? (y/n)")){
			tasks.erase(tasks.begin() + tnum);
			if(activeTask.first == name){
				activeTask.first = "null";
			}
			cout << name << " removed" << endl;
		}
	}else{
		cout << "task not found" << endl;
	}
}

void end(tasklist& tasks, task& activeTask){
	int tnum = findTaskNum(tasks, activeTask.first);
	if(tnum >= 0){
		tasks[tnum].second += time(0) - activeTask.second;
		cout << "'" << activeTask.first << "' ran for ";
		outputNiceTimespan(time(0) - activeTask.second);
		cout << endl;
		ofstream log(timemgrlogFile, ios_base::app);
		log << time(0) << "\t" << activeTask.first << "\t" << time(0) - activeTask.second << "s" << endl;
		log.close();
		activeTask.first = "null";
	}else{
		cout << "no task running" << endl;
		activeTask.first = "null";
	}
}

void start(tasklist& tasks, task& activeTask, string name){
	int tnum = findTaskNum(tasks, name);
	if(tnum < 0){
		add(tasks, name);
		tnum = findTaskNum(tasks, name);
	}
	if(activeTask.first != "null")	{
		if(askConfirmation("the task '" + activeTask.first + "' is still running. Stop? (y/n)" )){
			end(tasks, activeTask);
		} else {
			return;
		}
	}
	activeTask.first = tasks[tnum].first;
	activeTask.second = time(0);
	cout << name << " started" << endl;
}

void status(tasklist& tasks, task& activeTask){
	if(activeTask.first != "null"){
		cout << "Active task:" << endl;
		cout << activeTask.first << "\t";
		outputNiceTimespan(time(0) - activeTask.second);
		cout << endl;
	}else{
		cout << "no task running" << endl;
	}
}

void reset(tasklist& tasks){
	if(!askConfirmation("you will lose all your data. Continue? (y/n)")){
		return;
	}
	for(int i = 0; i < tasks.size(); i++){
		tasks[i].second = 0;
	}
	cout << "Done" << endl;
}

void archive(string filename, tasklist& tasks, task& activeTask){
	if(fileExists(filename)){
		if(!askConfirmation("file exists. Overwrite? (y/n)")){
			return;
		}
	}
	tasklist tmptasks (tasks);
	task tmpactiveTask = make_pair("null", 0);
	if(activeTask.first != "null"){
		int tnum = findTaskNum(tmptasks, activeTask.first);
		if(tnum >= 0){
			tmptasks[tnum].second += time(0) - activeTask.second;
		}
	}
	writeFile(filename, tmptasks, tmpactiveTask);
	
	cout << "Done" << endl;
}

void sum(tasklist& tasks, task& activeTask, int argv, char** args, int start){
	int res = 0;
	
	for(int i = start; i < argv; i++){
		int num = findTaskNum(tasks, string(args[i]));
		if(num >= 0){
			res += tasks[num].second;
		}
		if(num == -2){
			res += activeTask.second;
		}
	}
	
	if(start >= argv){
		for(int i = 0; i < tasks.size(); i++){
			res += tasks[i].second;
		}
	}
	
	outputNiceTimespan(res);
	cout << endl;
}

void rename(tasklist& tasks, task& activeTask, string oldName, string newName){
	int res = findTaskNum(tasks, oldName);
	int newRes = findTaskNum(tasks, newName);
	if(res >= 0){
		if(newRes < 0){
			if(activeTask.first == tasks[res].first){
				activeTask.first = newName;
			}
			tasks[res].first = newName;
			cout << "Done" << endl;
		} else {
			cout << "task already exists" << endl;
		}
	} else {
		cout << "task not found" << endl;
	}
}

void changeActiveTask(tasklist& tasks, task& activeTask, string newTask){
	int res = findTaskNum(tasks, newTask);
	if(res >= 0){
		activeTask.first = tasks[res].first;
		cout << "Done" << endl;
	} else {
		cout << "task not found" << endl;
	}
}

int main(int argv, char** args){
	for(int i = 0; i < argv; i++){
		// flags
		if(string(args[i]) == "-x"){
			return 0;
		}
	}

	string cmd;
	if(argv == 1){
		cmd = "help";
	}else{
		cmd = string(args[1]);
	}

	toLower(cmd);
	
	initAlias();

	cmd = replaceAlias(cmd);

	if(cmd == "init"){
		init();
	}else if(cmd == "help"){
		help();
	}else if(cmd == "printalias"){
		printAlias();
	}else {
		if(!fileExists(timemgrFile)){
			init();
		}
		
		tasklist tasks;
		task activeTask;
		readFile(timemgrFile, tasks, activeTask);

		// Do Stuff
		if(cmd == "list"){
			list(tasks, activeTask);
		}else if(cmd == "add"){
			if(argv > 2){
				string name = args[2];
				toLower(name);
				add(tasks, name);
			}
			else{
				cout << "missing task name. Usage: tm add <taskname>" << endl;
			}
		}else if(cmd == "remove"){
			if(argv > 2){
				string name = args[2];
				toLower(name);
				remove(tasks, activeTask, name);
			}
			else{
				cout << "missing task name. Usage: tm remove <taskname>" << endl;
			}
		}else if(cmd == "start"){
			if(argv > 2){
				string name = args[2];
				toLower(name);
				start(tasks, activeTask, name);
			}
			else{
				cout << "missing task name. Usage: tm start <taskname>" << endl;
			}
		}else if(cmd == "end"){
			end(tasks, activeTask);
		}else if(cmd == "status"){
			status(tasks, activeTask);
		} else if(cmd == "reset"){
			reset(tasks);
		}else if(cmd == "archive"){
			string filename = "timemgrarchive.txt";
			if(argv > 2){
				filename = args[2];
			}
			archive(filename, tasks, activeTask);
		} else if(cmd == "sum") {
			sum(tasks, activeTask, argv, args, 2);
		} else if(cmd == "rename"){
			if(argv > 3){
				rename(tasks, activeTask, args[2], args[3]);
			} else {
				cout << "missing task name. Usage: tm rename <oldTaskName> <newTaskName>" << endl;
			}
		} else if(cmd == "changeactivetask"){
			if(argv > 2){
				changeActiveTask(tasks, activeTask, args[2]);
			} else {
				cout << "missing task name. Usage: tm changeactivetask <newTask>" << endl;
			}
		}else{
			int num = findTaskNum(tasks, cmd);
			if(num >= 0){
				start(tasks, activeTask, cmd);
			} else {
				cout << "unknown command. type help to see the available commands" << endl;
			}
		}

		writeFile(timemgrFile, tasks, activeTask);
	}

	return 0;
}
