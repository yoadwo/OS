#ifndef RequesterPool_hpp
#define RequesterPool_hpp

#include <fstream>
#include "ThreadPool.hpp"

class RequesterPool: public ThreadPool
{
    private:
        ifstream *m_inputFiles;
        int   num_of_files;
        SafeQueue  *file_task_queue;
        SafeQueue   *dns_task_queue;
    public:
        RequesterPool(int poolsize, ifstream *inputFiles, bool bLinger);
        void RequesterPool::PoolStart();
        ~RequesterPool();
};

#endif