#ifndef ResolverPool_hpp
#define ResolverPool_hpp

#include "ThreadPool.hpp"

#include <fstream>

class ResolverPool: public ThreadPool
{
    private:
        SafeQueue   *dns_task_queue;
        unordered_map<string,char* ip> dns_ip_array;
        ofstream m_OutputFile;

    public:
        ResolverPool(int poolsize, char *output, bool bLinger);
        ~ResolverPool();
        SafeQueue*  getTaskQueue();
        bool isOutputOpen();
};

#endif