#ifndef SharedMemory_hpp
#define SharedMemory_hpp

#include <stdio.h>
#include <string>
#include <sys/types.h>
#include <sys/shm.h>
#include "SafeQueue.hpp"

class SharedMemory {

private:
    int segmentId_RequestQueue;
    int segmentId_ResultsArray;
    int resultsArraySize;
    int requestsQueueSize;
    int actualArraySize;
    SafeQueue* safeQueue;
    string* resultsArray;

public:
    SharedMemory(int _requestQueueSize, int _resultArraySize);
    ~SharedMemory();
    SafeQueue* getQueue();
    string* getArray();
    void addStringToArray(string str);
    int getResultsArraySize();
};

#endif /* SharedMemory_hpp */
