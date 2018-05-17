#ifndef RequesterPool_hpp
#define RequesterPool_hpp

#include <fstream>
#include "ThreadPool.hpp"

class RequesterPool: public ThreadPool
{
    private:
        fstream **m_inputFiles;
        int   num_of_files;
        SafeQueue  *file_task_queue;
        SafeQueue   *dns_task_queue;
    public:
        RequesterPool(int poolsize, fstream *inputFiles[], bool bLinger);
        ~RequesterPool();
        void PoolStart();
};

#endif