#ifndef SafeQueue_hpp
#define SafeQueue_hpp

//#include "FileTask.hpp"
//#include "DomainTask.hpp"
#include <iostream>

#include "Task.hpp"
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

using namespace std;
class SafeQueue {
    
public:
    SafeQueue();
    SafeQueue(int maxSize);
    ~SafeQueue();
    void pushTask(Task* task);
    Task* popTask();
    bool isEmpty();
    int size();

private:
    pthread_mutex_t mutex;
    Task** queue;
    int queueActualSize;
    int queueMaxSize;
};

#endif /* SafeQueue_hpp */
