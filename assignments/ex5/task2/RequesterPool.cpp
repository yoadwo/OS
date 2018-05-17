#include "RequesterPool.hpp"

RequesterPool:: RequesterPool(int poolsize, ifstream *inputFiles, bool bLinger):
    ThreadPool(poolsize, bLinger),
    m_inputFiles(inputFiles){}
RequesterPool::~RequesterPool()


void RequesterPool::PoolStart(){    
    ThreadPool::PoolStart();
    
}    