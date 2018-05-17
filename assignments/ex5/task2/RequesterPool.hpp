#ifndef RequesterPool_hpp
#define RequesterPool_hpp

#include <fstream>
#include "ThreadPool.hpp"

class RequesterPool: public ThreadPool
{
    private:
        fstream **m_inputFiles;
    public:
        RequesterPool(int poolsize, fstream *inputFiles[], bool bLinger);
};

#endif