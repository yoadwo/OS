#include "RequesterPool.hpp"

RequesterPool::RequesterPool(int poolsize, int argc, char *argv[], bool bLinger):
ThreadPool(poolsize, bLinger),
num_of_files(poolsize){

    for (int i = 1; i < argc - 1; i++){
        m_inputFiles.push_back(argv[i]);
    }

    /* file_task_queue=new SafeQueue();
    dns_task_queue=new SafeQueue();

    for(int i=0;i<num_of_files;i++){
        file_task_queue->pushTask(new FileTask(i,inputFiles[i]));
    } */
}

void RequesterPool:: PushTasks(SafeQueue *resolverQueue){
    for(int i=0;i<num_of_files;i++){
        m_task_queue->pushTask(new FileTask(i,m_inputFiles[i],resolverQueue, m_dns_ip_array));
    }
}

RequesterPool::~RequesterPool(){}

  