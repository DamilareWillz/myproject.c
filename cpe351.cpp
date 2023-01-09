#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <cstring>
#include <cctype>
#include "taskqueue.h"
using namespace std;

enum Scheduler {NONE, FCFS, SJF, PRIOR, RR};

bool ParsArgs(int argc, char* argv[], string& infile, string& outfile);
bool ReadInput(string infile, TaskQueue& inputQ);
int Menu(Scheduler sched, bool preempt);
void SelectSched(Scheduler& sched, int& quantum);
bool SelectPreempt();
void DoSimulation(Scheduler sched, bool preempt, int quantum, 
                  TaskQueue& inputQ, TaskQueue& finishQ);
void SaveResults(Scheduler sched, bool preempt, int quantum, 
                 TaskQueue& finishQ, string outfile);

int main(int argc, char* argv[]) {
    string infile, outfile;
    Scheduler sched = NONE;
    bool preempt = false;
    int quantum = 1;
    TaskQueue inputQ;
    TaskQueue finishQ;

    if (!ParsArgs(argc, argv, infile, outfile)) {
        exit(1);
    }

    if (!ReadInput(infile, inputQ)) {
        exit(1);
    }

    // Main loop
    bool done = false;
    while (!done) {
        int ans = Menu(sched, preempt);
        switch (ans) {
            case 1:
                SelectSched(sched, quantum);
                break;
            case 2:
                preempt = SelectPreempt();
                break;
            case 3:
                DoSimulation(sched, preempt, quantum, inputQ, finishQ);
                break;
            case 4:
                SaveResults(sched, preempt, quantum, finishQ, outfile);
                done = true;
                break;
            default:
                cout << "Unknown options" << endl << endl;
        }
    }

    return 0;
}

// Print instruction
void Usage(const char* name) {
    cerr << "Usage: " << name << " -f <input-file> -o <output-file>" << endl;
}

// Assign values to input and output file anmes
bool ParsArgs(int argc, char* argv[], string& infile, string& outfile) {
    if (argc != 5)  {
        Usage(argv[0]);
        return false;
    }
    if (strcmp(argv[1], "-f") == 0 && strcmp(argv[3], "-o") == 0) {
        infile = argv[2];
        outfile = argv[4];
        return true;
    }
    if (strcmp(argv[3], "-f") == 0 && strcmp(argv[1], "-o") == 0) {
        infile = argv[4];
        outfile = argv[2];
        return true;
    }
    Usage(argv[0]);
    return false;
}

// Convert constant to scheduler name
string SchedulingName(Scheduler sched) {
    if (sched == NONE) 
        return string("None");
    else if (sched == FCFS)
        return string("First Come, First Served");
    else if (sched == SJF) 
        return string("Shortest-Job-First");
    else if (sched == PRIOR)
        return string("Priority");
    else 
        return string("Round-Robin");
}

// Function to sort Task:
// On arrivale time
bool ArriveBefore(Task* task1, Task* task2) {
    return task1->arrival_time < task2->arrival_time;
}

// On the burst time
bool ShotterTask(Task* task1, Task* task2) {
    return task1->burst_time < task2->burst_time;
}

// On priority
bool MorePriorTask(Task* task1, Task* task2) {
    if (task1->priority == task2->priority) {
        return task1->arrival_time < task2->arrival_time;
    }
    return task1->priority < task2->priority;
}

// By name
bool AlphabeticTask(Task* task1, Task* task2) {
    return task1->name < task2->name;
}

// Read data about task from a file
bool ReadInput(string infile, TaskQueue& inputQ) {
    ifstream fin(infile.c_str());

    if (!fin) {
        cerr << "Cant open fiel " << infile << endl;
        return false;
    }
    int num = 1;
    int burst, arriv, prior;
    char ch;
    string name;
    Task* task;

    while (!fin.eof()) {
        fin >> burst >> ch >> arriv >> ch >> prior;
        name = "P" + to_string(num);
        task = new Task(name, arriv, burst, prior);
        inputQ.AddTask(task, ArriveBefore);
        num++;
    }
    return true;
}

// Display a main menu and return a choise
int Menu(Scheduler sched, bool preempt) {
    int res;

    cout << endl;
    cout << "                      CPU Scheduler Simulator" << endl;
    cout << "1) Scheduling Method (" << SchedulingName(sched) << ")" << endl;
    cout << "2) Preemptive Mode ( " << (preempt ? "On" : "Off") << ")" << endl;
    cout << "3) Show Result" << endl;
    cout << "4) End Program" << endl;
    cout << "Option > ";
    cin >> res;

    return res;
}

// Set the scheduling method
void SelectSched(Scheduler& sched, int& quantum) {
    int ans;

    cout << endl;
    cout << "Select Scheduling method:" << endl;
    cout << "1) First Come, First Served" << endl;
    cout << "2) Shortest-Job-First" << endl;
    cout << "3) Priority" << endl;
    cout << "4) Round-Robin" << endl;
    cout << "Method > ";
    cin >> ans;
    switch (ans) {
        case 1:
            sched = FCFS;
            break;
        case 2:
            sched = SJF;
            break;
        case 3:
            sched = PRIOR;
            break;
        case 4:
            sched = RR;
            break;
        default:
            sched = NONE;
            break;
    }
    if (sched == RR) {
        cout << "Enter time quantum size > ";
        cin >> quantum;
    }
}

// Select Preempty mode (on or off)
bool SelectPreempt() {
    char ch;
    cout << "Swytch Preempty Mode on (y/N) ";
    cin >> ch;
    return toupper(ch) == 'Y';
}

// Output the result of simulation int stream
void ShowResult(Scheduler sched, bool preempt, int quantum, 
                TaskQueue& queue, ostream& os)
{
    os << "Scheduling Method: " << SchedulingName(sched);
    if (sched == SJF || sched == PRIOR) {
        if (preempt) {
            os << " -- Preemptive";
        }
        else {
            os << " -- Non-Preeptive";
        }
    }
    else if (sched == RR) {
        os << " -- time_quantum = " << quantum;
    }
    os << endl;

    os << "Process Waiting Times:" << endl;
    int tot_wait_time = 0;

    // Look over all tasks
    Task* task = queue.GetFirst();
    while (task) {
        os << task->name << ": " << task->wait_time << " ms" << endl;
        tot_wait_time += task->wait_time;
        task = queue.GetNext();
    }

    // Calc average
    double avg = static_cast<double>(tot_wait_time) / queue.GetSize();
    os << "Average Waiting Time: "<< fixed << setprecision(2) 
       << avg << " ms" << endl;

}                 

// Simulate shcheduling
void DoSimulation(Scheduler sched, bool preempt, int quantum, 
                  TaskQueue& inputQ, TaskQueue& finishQ)
{
    TaskQueue arrivTask, waitTask;
    int time = 0;
    int quantum_left;
    Task* curTask = nullptr;
    TaskCompare compare;

    if (sched == NONE) {
        cout << "Scheduling Model not selected" << endl;
        return;
    }

    // Copy input data
    arrivTask.Copy(inputQ);
    finishQ.Free();

    // While not all tasks finished
    while ( finishQ.GetSize() != inputQ.GetSize()) {
        // First check if there are new tasks arrived
        Task* task = arrivTask.GetFirst();
        while (task && task->arrival_time == time) {
            task = arrivTask.PopTask();
            if (sched == FCFS) {
                // Add a task to the end
                waitTask.AddTaskEnd(task);
            }
            else if (sched == SJF) {
                // If preemptive move current task to wait
                if (preempt && curTask) {
                    waitTask.AddTask(curTask, ShotterTask);
                    curTask = nullptr;
                }
                // Add a task
                waitTask.AddTask(task, ShotterTask);
            }
            else if (sched == PRIOR) {
                waitTask.AddTask(task, MorePriorTask);
                if (preempt && curTask) {
                    waitTask.AddTask(curTask, MorePriorTask);
                    curTask = nullptr;
                }
            }
            else {
                waitTask.AddTaskEnd(task);
            }
            task = arrivTask.GetFirst();
        }

        // If round-Robin - check for the quantum end
        if (curTask && sched == RR && quantum_left == 0) {
            waitTask.AddTaskEnd(curTask);
            curTask = nullptr;

        }

        // If no current task - select the first from the queue
        if (!curTask) {
            curTask = waitTask.PopTask();
            quantum_left = quantum;
        }

        // one tic 
        if (curTask) {
            curTask->burst_time--;
            if (curTask->burst_time == 0) {
                finishQ.AddTask(curTask, AlphabeticTask);
                curTask = nullptr;
            }
            quantum_left--;
        }
        
        // Update waiting times
        task = waitTask.GetFirst();
        while (task) {
            task->wait_time++;
            task = waitTask.GetNext();
        }

        time++;
    }

    cout << endl;
    ShowResult(sched, preempt, quantum, finishQ, cout);
}     

// Save result to a disk
void SaveResults(Scheduler sched, bool preempt, int quantum, 
                 TaskQueue& finishQ, string outfile)
{
    ofstream fout(outfile.c_str());
    if (!fout) {
        cerr << "Error opening file " << outfile << endl;
        return;
    }
    ShowResult(sched, preempt, quantum, finishQ, fout);
}