#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <ctime>

using namespace std;

void outputNiceTimespan(int timespan)
{
    int d, h, m, s;
    s = timespan % 60;
    timespan /= 60;
    m = timespan % 60;
    timespan /= 60;
    h = timespan % 24;
    timespan /= 24;
    d = timespan;

    if(d > 0)
        cout << d << "d " << h << "h " << m << "min " << s << "s";
    else if(h > 0)
        cout << h << "h " << m << "min " << s << "s";
    else if(m > 0)
        cout << m << "min " << s << "s";
    else
        cout << s << "s";
}

void toLower(string& s)
{
    for(int i = 0; i < s.size(); i++)
    {
        if(s[i] >= 'A' && s[i] <= 'Z')
        {
            s[i] -= 'A' - 'a';
        }
    }
}

string replaceAlias(string cmd)
{
	string ncmd = cmd;
	
    map<string, string> alias;
    alias["l"] = "list";
    alias["a"] = "add";
    alias["+"] = "add";
    alias["r"] = "remove";
    alias["rm"] = "remove";
    alias["-"] = "remove";
    alias["s"] = "start";
    alias["e"] = "end";
	alias["stop"] = "stop";
    alias["st"] = "status";
    alias["."] = "status";
    alias["h"] = "help";
    alias["?"] = "help";
    alias["/?"] = "help";
	
    if(alias.find(ncmd) != alias.end())
        ncmd = alias[ncmd];
	
	return ncmd;
}

int findTaskNum(vector<pair<string, int> >& tasks, string name)
{
	for(int i = 0; i < tasks.size(); i++)
	{
		if(tasks[i].first == name)
		{
			return i;
		}
	}
	return -1;
}

bool fileExists()
{
	ifstream f("timemgr.txt");
    if (f.good()) {
        f.close();
        return true;
    } else {
        f.close();
        return false;
    }
}

void readFile(vector<pair<string, int> >& tasks, pair<string, int>& activeTask)
{
        // Read file
        ifstream fin("timemgr.txt");
        int n;
        fin >> n;
        tasks.resize(n);
        for(int i = 0; i < n; i++)
        {
            fin >> tasks[i].first >> tasks[i].second;
        }

        fin >> activeTask.first >> activeTask.second;

        fin.close();
}

void writeFile(vector<pair<string, int> >& tasks, pair<string, int>& activeTask)
{
        ofstream fout("timemgr.txt");
        fout << tasks.size() << endl;
        for(int i = 0; i < tasks.size(); i++)
        {
            fout << tasks[i].first << " " << tasks[i].second << "\n";
        }

        fout << activeTask.first << " " << activeTask.second << endl;
        fout.close();
}

void init()
{
	if(fileExists())
{
		cout << "this will erase all your data. Continue? (y/n)" << endl;
		string response;
		do
		{
			cin >> response;
		} while(response != "y" && response != "n");
		if(response == "n")
		{
			return;
		}
	}
	ofstream fout ("timemgr.txt");
	fout << "0" << endl;
	fout << "null 0" << endl;
	fout.close();
	cout << "Done" << endl;

	ofstream log("timemgrlog.txt");
	log << "TimeMgr log" << endl;
	log.close();
}

void help()
{
        cout << "TimeMgr" << endl;
        cout << "(c) 2014 Benjamin Schmid" << endl;
        cout << endl;
        cout << "Available commands:" << endl;
        cout << "list, add, remove, start, end, status, help" << endl;
}

void list(vector<pair<string, int> >& tasks, pair<string, int>& activeTask)
{
	if(tasks.size() > 0)
	{
		cout << "tasks:" << endl;
		for(int i = 0; i < tasks.size(); i++)
		{
			cout << i << ":\t" << tasks[i].first << "\t";
			outputNiceTimespan(tasks[i].second);
			cout << "\n";
		}

		cout << endl;
	}
	else
	{
		cout << "no tasks" << endl;
	}

	if(activeTask.first != "null")
	{
		cout << "Active task:" << endl;
		cout << activeTask.first << "\t";
		outputNiceTimespan(time(0) - activeTask.second);
		cout << endl;
	}
}

void add(vector<pair<string, int> >& tasks, string name)
{
	int tnum = findTaskNum(tasks, name);
	if(tnum < 0)
	{
		tasks.push_back(make_pair(name, 0));
		cout << name << " added" << endl;
	}
	else
	{
		cout << "task already exists" << endl;
	}
}

void remove(vector<pair<string, int> >& tasks, pair<string, int>& activeTask, string name)
{
	int tnum = findTaskNum(tasks, name);
	if(tnum >= 0)
	{
		cout << "this will remove the task '" << name << "' . Continue? (y/n)" << endl;
		string response;
		do
		{
			cin >> response;
		} while(response != "y" && response != "n");
		if(response == "y")
		{
			tasks.erase(tasks.begin() + tnum);
			if(activeTask.first == name)
			{
				activeTask.first = "null";
			}
			cout << name << " removed" << endl;
		}
	}
	else
	{
		cout << "task not found" << endl;
	}
}
	
void end(vector<pair<string, int> >& tasks, pair<string, int>& activeTask)
{
	int tnum = findTaskNum(tasks, activeTask.first);
	if(tnum >= 0)
	{
		tasks[tnum].second += time(0) - activeTask.second;
		cout << "'" << activeTask.first << "' ran for ";
		outputNiceTimespan(time(0) - activeTask.second);
		cout << endl;
		ofstream log("timemgrlog.txt", ios_base::app);
		log << time(0) << "\t" << activeTask.first << "\t" << time(0) - activeTask.second << "s" << endl;
		log.close();
		activeTask.first = "null";
	}
	else
	{
		cout << "no task running" << endl;
		activeTask.first = "null";
	}
}
	
void start(vector<pair<string, int> >& tasks, pair<string, int>& activeTask, string name)
{
	if(activeTask.first != "null")
	{
		cout << "the task '" << activeTask.first << "' is still running. Stop? (y/n)" << endl;
		string response;
		do
		{
			cin >> response;
		} while(response != "y" && response != "n");
		if(response == "y")
		{
			end(tasks, activeTask);
		}
	}
	if(activeTask.first == "null")
	{
		int tnum = findTaskNum(tasks, name);
		if(tnum >= 0)
		{
			activeTask.first = name;
			activeTask.second = time(0);
			cout << name << " started" << endl;
		}
		else
		{
			cout << name << " not found" << endl;
		}
	}
}
	
void status(vector<pair<string, int> >& tasks, pair<string, int>& activeTask)
{
	if(activeTask.first != "null")
	{
		cout << "Active task:" << endl;
		cout << activeTask.first << "\t";
		outputNiceTimespan(time(0) - activeTask.second);
		cout << endl;
	}
	else
	{
		cout << "no task running" << endl;
	}
}

int main(int argv, char** args)
{
    for(int i = 0; i < argv; i++)
    {
        // flags
        if(string(args[i]) == "-x")
        {
            return 0;
        }
    }

    string cmd;
    if(argv == 1)
    {
        cmd = "help";
    }
    else
    {
        cmd = string(args[1]);
    }

	toLower(cmd);

    cmd = replaceAlias(cmd);

    if(cmd == "init")
    {
		init();
    }
    else if (cmd == "list" || cmd == "add" || cmd == "remove" || cmd == "start" || cmd == "end" || cmd == "status")
    {
		if(!fileExists()){
			init();
		}
		
        vector<pair<string, int> > tasks;
        pair<string, int> activeTask;
		readFile(tasks, activeTask);

        // Do Stuff
        if(cmd == "list")
        {
			list(tasks, activeTask);
        }
        else if(cmd == "add")
        {
            if(argv > 2)
            {
				string name = args[2];
				toLower(name);
				add(tasks, name);
            }
            else
            {
                cout << "missing task name. Usage: tm add <taskname>" << endl;
            }
        }
        else if(cmd == "remove")
        {
            if(argv > 2)
            {
				string name = args[2];
				toLower(name);
				remove(tasks, activeTask, name);
            }
            else
            {
                cout << "missing task name. Usage: tm remove <taskname>" << endl;
            }
        }
        else if(cmd == "start")
        {
            if(argv > 2)
            {
				string name = args[2];
				toLower(name);
				start(tasks, activeTask, name);
            }
            else
            {
                cout << "missing task name. Usage: tm start <taskname>" << endl;
            }
        }
        else if(cmd == "end")
        {
			end(tasks, activeTask);
        }
        else if(cmd == "status")
        {
			status(tasks, activeTask);
        }

		writeFile(tasks, activeTask);
    }
    else if(cmd == "help")
    {
		help();
    }
    else
    {
        cout << "unknown command. type help to see the available commands" << endl;
    }

    return 0;
}
