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

    map<string, string> alias;
    alias["l"] = "list";
    alias["a"] = "add";
    alias["+"] = "add";
    alias["r"] = "remove";
    alias["rm"] = "remove";
    alias["-"] = "remove";
    alias["s"] = "start";
    alias["e"] = "end";
    alias["st"] = "status";
    alias["."] = "status";
    alias["h"] = "help";
    alias["?"] = "help";
    alias["/?"] = "help";

    for(int i = 0; i < cmd.size(); i++)
    {
        if(cmd[i] >= 'A' && cmd[i] <= 'Z')
        {
            cmd[i] -= 'A' - 'a';
        }
    }

    if(alias.find(cmd) != alias.end())
        cmd = alias[cmd];

    if(cmd == "init")
    {
        cout << "this will erase all your data. Continue? (y/n)" << endl;
        string response;
        do
        {
            cin >> response;
        } while(response != "y" && response != "n");
        if(response == "y")
        {
            ofstream fout ("timemgr.txt");
            fout << "0" << endl;
            fout << "null 0" << endl;
            fout.close();
            cout << "Done" << endl;

            ofstream log("timemgrlog.txt");
            log << "TimeMgr log" << endl;
            log.close();
        }
    }
    else if (cmd == "list" || cmd == "add" || cmd == "remove" || cmd == "start" || cmd == "end" || cmd == "status")
    {
        // Read file
        ifstream fin("timemgr.txt");
        int n;
        fin >> n;
        vector<pair<string, int> > tasks (n);
        for(int i = 0; i < n; i++)
        {
            fin >> tasks[i].first >> tasks[i].second;
        }

        pair<string, int> activeTask;
        fin >> activeTask.first >> activeTask.second;

        fin.close();

        // Do Stuff
        if(cmd == "list")
        {
            if(n > 0)
            {
                cout << "tasks:" << endl;
                for(int i = 0; i < n; i++)
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
        else if(cmd == "add")
        {
            if(argv > 2)
            {
                string name (args[2]);
                bool pos = true;
                for(int i = 0; i < tasks.size(); i++)
                {
                    if(tasks[i].first == name)
                    {
                        pos = false;
                        break;
                    }
                }
                if(pos)
                {
                    tasks.push_back(make_pair(name, 0));
                    cout << name << " added" << endl;
                }
                else
                {
                    cout << "task already exists" << endl;
                }
            }
            else
            {
                cout << "missing task name" << endl;
            }
        }
        else if(cmd == "remove")
        {
            if(argv > 2)
            {
                string name (args[2]);
                bool found = false;
                for(int i = 0; i < tasks.size(); i++)
                {
                    if(tasks[i].first == name)
                    {
                        found = true;
                        cout << "this will remove the task '" << name << "' . Continue? (y/n)" << endl;
                        string response;
                        do
                        {
                            cin >> response;
                        } while(response != "y" && response != "n");
                        if(response == "y")
                        {
                            tasks.erase(tasks.begin() + i);
                            if(activeTask.first == name)
                            {
                                activeTask.first = "null";
                            }
                            cout << name << " removed" << endl;
                        }
                        break;
                    }
                }
                if(!found)
                {
                    cout << "task not found" << endl;
                }
            }
            else
            {
                cout << "missing task name" << endl;
            }
        }
        else if(cmd == "start")
        {
            if(argv > 2)
            {
                string name(args[2]);
                if(activeTask.first != "null")
                {
                    int tnum = -1;
                    for(int i = 0; i < tasks.size(); i++)
                    {
                        if(tasks[i].first == activeTask.first)
                        {
                            tnum = i;
                            break;
                        }
                    }
                    cout << "the task '" << activeTask.first << "' is still running. Stop? (y/n)" << endl;
                    string response;
                    do
                    {
                        cin >> response;
                    } while(response != "y" && response != "n");
                    if(response == "y")
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
                }
                if(activeTask.first == "null")
                {
                    int tnum = -1;
                    for(int i = 0; i < tasks.size(); i++)
                    {
                        if(tasks[i].first == name)
                        {
                            tnum = i;
                            break;
                        }
                    }
                    if(tnum >= 0)
                    {
                        activeTask.first = string(args[2]);
                        activeTask.second = time(0);
                        cout << name << " started" << endl;
                    }
                    else
                    {
                        cout << name << " not found" << endl;
                    }
                }
            }
            else
            {
                cout << "missing task name" << endl;
            }
        }
        else if(cmd == "end")
        {
            int tnum = -1;
            for(int i = 0; i < tasks.size(); i++)
            {
                if(tasks[i].first == activeTask.first)
                {
                    tnum = i;
                    break;
                }
            }
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
        else if(cmd == "status")
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

        // Write file
        ofstream fout("timemgr.txt");
        fout << tasks.size() << endl;
        for(int i = 0; i < tasks.size(); i++)
        {
            fout << tasks[i].first << " " << tasks[i].second << "\n";
        }

        fout << activeTask.first << " " << activeTask.second << endl;
        fout.close();
    }
    else if(cmd == "help")
    {
        cout << "TimeMgr" << endl;
        cout << "(c) 2014 Benjamin Schmid" << endl;
        cout << endl;
        cout << "Available commands:" << endl;
        cout << "list, add, remove, start, end, status, help" << endl;
    }
    else
    {
        cout << "unknown command. type help to see the available commands" << endl;
    }

    return 0;
}
