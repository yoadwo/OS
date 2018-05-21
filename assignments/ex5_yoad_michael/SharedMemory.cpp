#include "SharedMemory.hpp"

SharedMemory::SharedMemory(int _requestQueueSize, int _resultArraySize)
{  
    string* tempRA = NULL;
    SafeQueue* tempSF = NULL;
    actualArraySize = 0;
    requestsQueueSize = _requestQueueSize;
    resultsArraySize = _resultArraySize;
    if((segmentId_RequestQueue = shmget(IPC_PRIVATE, sizeof(SafeQueue) + (requestsQueueSize * sizeof(DomainTask)), 0644 | IPC_CREAT))==-1)
    {
        cerr << "Shared memory segment exists" << endl;
        exit(1);
    }
    else
    {
        tempSF = new (shmat (segmentId_RequestQueue,0, 0)) SafeQueue;
        safeQueue = new (tempSF) SafeQueue(requestsQueueSize);
    }
    
    if((segmentId_ResultsArray = shmget(IPC_PRIVATE, sizeof(string) * resultsArraySize, 0644 | IPC_CREAT))==-1)
    {
        cerr << "Shared memory segment exists" << endl;
        exit(1);
    }
    else
    {
        tempRA = new (shmat (segmentId_ResultsArray,0, 0)) string;
        resultsArray = new (tempRA) string[resultsArraySize];
    }
    
}

SharedMemory::~SharedMemory()
{
    shmctl(segmentId_RequestQueue, IPC_RMID, NULL);
    shmctl(segmentId_ResultsArray, IPC_RMID, NULL);
}

SafeQueue* SharedMemory::getQueue()
{
    return safeQueue;
}

string* SharedMemory::getArray()
{
    return resultsArray;
}

int SharedMemory::getResultsArraySize()
{
    return resultsArraySize;
}

void SharedMemory::addStringToArray(string str)
{
    resultsArray[actualArraySize++] = str;
}




