#include "taskqueue.h"

// Destructor
TaskQueue::~TaskQueue() {
    Free();
}

//  Copy a queue
void TaskQueue::Copy(const TaskQueue& other) {
    Free();

    // If queue is empty
    if (other.head == nullptr) {
        return;
    }

    // Copy the first node
    Task* newTask = new Task(*other.head->task);
    head = new Node(newTask);
    size++;

    // Copy the rest
    Node* src = other.head;
    Node* dst = head;
    while (src->next) {
        newTask = new Task(*src->next->task);
        dst->next = new Node(newTask);
        size++;
        dst = dst->next;
        src = src->next;
    }
    // Set a pointer to the last node
    tail = dst;
}

// Remove the first node from the queue
Task* TaskQueue::PopTask() {
    if (!head) {
        return nullptr;
    }
    Task* task = head->task;
    Node* tmp = head;
    head = head->next;
    delete tmp;
    size--;
    return task;
}

// Add a task to the queue, keeping it sorted
void TaskQueue::AddTask(Task* task, TaskCompare isBefore) {
    // If the queue is empty insert into head
    if (!head) {
        head = new Node(task);
        tail = head;
        size++;
        return;
    }

    // The inserted task should be the first
    if (isBefore(task, head->task)) {
        Node* newNode = new Node(task);
        newNode->next = head;
        head = newNode;
        size++;
        return;
    }

    Node* node = head;
    // Find place where to insert
    while (node->next) {
        if (isBefore(task, node->next->task)) {
            break;
        }
        node = node->next;
    }
    Node* newNode = new Node(task);
    newNode->next = node->next;
    node->next = newNode;
    size++;
    if (tail == node) {
        tail = newNode;
    }
}

// Add a task to the end of the queue
void TaskQueue::AddTaskEnd(Task* task) {
    // If the queue is empty
    if (!head) {
        head = new Node(task);
        tail = head;
        size++;
        return;
    }

    tail->next = new Node(task);
    tail = tail->next;
    size++;
}

// Return pointer to the first task, and reset current pointer
Task* TaskQueue::GetFirst() {
    current = head;
    if (current) {
        return current->task;
    }
    return nullptr;
}

// Return pointer to the next task
Task* TaskQueue::GetNext() {
    if (!current) {
        return nullptr;
    }
    current = current->next;
    if (current) {
        return current->task;
    }
    return nullptr;
}

// Clear the queue
void TaskQueue::Free() {
    Node* node = head;
    Node* tmp;

    while (node) {
        tmp = node;
        node = node->next;
        delete tmp->task;
        delete tmp;
    }
    size = 0;
    head = nullptr;
    tail = nullptr;
}