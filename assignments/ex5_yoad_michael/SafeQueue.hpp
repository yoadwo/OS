#ifndef SafeQueue_hpp
#define SafeQueue_hpp

#include "FileTask.hpp"
#include "DomainTask.hpp"
#include <pthread.h>
#include <unistd.h>

class SafeQueue {


private:
    pthread_mutex_t mutex;
    Task** queue;
    int queueMaxSize; 
    int queueActualSize;
    

public:
    SafeQueue();
    SafeQueue(int maxSize);
    ~SafeQueue();
    void pushTask(Task* task);
    Task* popTask();
    bool isEmpty();
    int size();
};

#endif /* SafeQueue_hpp */
