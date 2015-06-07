TimeMgr
=======

Console Time Manager

Allows you to keep track of used time for a given task.

Compile the file timemgr.cpp, e.g. as tm.exe.

Copy the file *tm.exe* in the directory you want to store the data. To use TimeMgr open a terminal and change to the directory where you copied *tm.exe*.

First use: call *tm init*

#Commands:
```
tm init		initializes the save file
tm list		list all available tasks
tm add		add a new task
tm remove	remove a task
tm start	start a task
tm end		end the active task
tm status	shows the active task
tm reset	resets the time for all tasks
tm archive	saves the save file under a different name
tm sum		sums up the times of all the given tasks (e.g. tm sum task1 task3 task4). Use -2 for the active task.
tm rename	renames the given task (e.g. tm rename oldTaskName newTaskName).
tm cat		changes the active task to the given task.
tm pa		prints all defined aliases
```