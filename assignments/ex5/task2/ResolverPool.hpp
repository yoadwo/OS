#ifndef ResolverPool_hpp
#define ResolverPool_hpp

#include "ThreadPool.hpp"

#include <unordered_map>
#include <fstream>
#include <vector>




class ResolverPool: public ThreadPool
{
    private:
        //SafeQueue   *dns_task_queue;
        
        ofstream m_OutputFile;

    public:
        ResolverPool(int poolsize, char *output, bool bLinger);
        ~ResolverPool();
        SafeQueue*  getTaskQueue();
        bool isOutputOpen();
        

};

#endif