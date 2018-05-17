#include "RequesterPool.hpp"

RequesterPool:: RequesterPool(int poolsize, fstream *inputFiles[], bool bLinger):
    ThreadPool(poolsize, bLinger),
    m_inputFiles(inputFiles){}
//RequesterPool::~RequesterPool(){}

/* 
void RequesterPool::PoolStart(){    
    //ThreadPool::PoolStart();
    for (int i=0; i< poolsize; i++){
        DemoTask *demo = new DemoTask(new int(i+1));     
    }
}   */  