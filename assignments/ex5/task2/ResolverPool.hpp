#ifndef ResolverPool_hpp
#define ResolverPool_hpp

#include "ThreadPool.hpp"

#include <unordered_map>
#include <fstream>
#include <unordered_map>
#include <vector>




class ResolverPool: public ThreadPool
{
    private:
        SafeQueue   *dns_task_queue;
        unordered_map<string,vector<char>> dns_ip_array;
        ofstream m_OutputFile;

    public:
        ResolverPool(int poolsize, char *output, bool bLinger);
        ~ResolverPool();
        SafeQueue*  getTaskQueue();
        bool isOutputOpen();
};

#endif