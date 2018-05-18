#include "RequesterPool.hpp"

RequesterPool:: RequesterPool(int poolsize, fstream *inputFiles[], bool bLinger):
    ThreadPool(poolsize, bLinger),m_inputFiles(inputFiles),num_of_files(poolsize){
        
        file_task_queue=new SafeQueue();
        dns_task_queue=new SafeQueue();

        for(int i=0;i<num_of_files;i++){
            file_task_queue->pushTask(new FileTask(i,inputFiles[i]));
        }
    }

RequesterPool::~RequesterPool(){}


void RequesterPool::PoolStart(){    
    ThreadPool::PoolStart();
    
}     