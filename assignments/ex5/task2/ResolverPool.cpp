#include "ResolverPool.hpp"

ResolverPool::ResolverPool(int poolsize, char *output, bool bLinger):
    ThreadPool(poolsize, bLinger){
        m_OutputFile.open(output);
    }

bool ResolverPool::isOutputOpen(){
    return m_OutputFile.is_open();
}

SafeQueue* ResolverPool::getTaskQueue(){
    return m_task_queue;
}

ResolverPool::~ResolverPool(){}