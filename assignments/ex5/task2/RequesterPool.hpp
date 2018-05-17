#ifndef RequesterPool_hpp
#define RequesterPool_hpp

#include <fstream>
#include "ThreadPool.hpp"

class RequesterPool: public ThreadPool
{
    private:
        ifstream *m_inputFiles;
    public:
        RequesterPool(int poolsize, ifstream *inputFiles, bool bLinger);
};

#endif