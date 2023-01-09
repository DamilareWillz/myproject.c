#ifndef TASKQUEUE_H
#define TASKQUEUE_H
#include <string>

struct Task {
    std::string name;
    int arrival_time;
    int burst_time;
    int priority;
    int wait_time;

    Task(std::string name, int arr, int burst, int pr) :
        name(name), arrival_time(arr), burst_time(burst),
        priority(pr), wait_time(0) {}
    Task(const Task& o) :
        name(o.name), arrival_time(o.arrival_time), 
        burst_time(o.burst_time), priority(o.priority), 
        wait_time(o.wait_time) {}
};

typedef bool TaskCompare(Task* task1, Task* task2);

class TaskQueue {
public:
    TaskQueue() : head(nullptr), tail(nullptr), current(nullptr), size(0)
        {}
    ~TaskQueue();
    void Copy(const TaskQueue& other);
    int GetSize() const
        { return size; }
    Task* PopTask();
    void AddTask(Task* task, TaskCompare comparator);
    void AddTaskEnd(Task* task);
    Task* GetFirst();
    Task* GetNext();
    void Free();

private:
    struct Node {
        Task* task;
        Node* next;
        Node(Task* t) : task(t), next(nullptr)  {}
    };

    Node* head;         // pointer to the first node
    Node* tail;         // pointer to the last node
    Node* current;      // pointer to the current node
    int size;           // the size of queue
};

#endif