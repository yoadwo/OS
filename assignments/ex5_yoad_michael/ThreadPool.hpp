#ifndef ThreadPool_hpp
#define ThreadPool_hpp

#include "SafeQueue.hpp"

extern "C" {
#include "util.h"
}
#include "SharedMemory.hpp"
#include <fstream>

#define MAX_ADDRESSES 10

enum ThreadPoolFunction{REQUESTER, RESOLVER};

class ThreadPool {
    
private:
    int nThreads;
    int nFiles;
    int nDomains;
    pthread_t* arrThreads;
    SafeQueue safeQueue;
    SharedMemory* sharedMemory;
    string* arrFiles;
    string function;
    pthread_mutex_t file_mutex;
    pthread_mutex_t flag_mutex;
    static pthread_mutex_t cond_mutex;
    static bool flag;
public:
    ThreadPool(int amountOfThreads, int amountOfFiles, string* filesArray, SharedMemory* sharedMemory, string sentFunction, int amountOfDomains);
    ~ThreadPool();
    void run();
    void* Request();
    void* Resolve();
    static void* wrapperRequest(void* obj);
    static void* wrapperResolve(void* obj);
    void writeToArray();
    void join();
    bool haveRequest();
    

};

#endif /* ThreadPool_hpp */
