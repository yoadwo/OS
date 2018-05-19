#ifndef ResolverPool_hpp
#define ResolverPool_hpp

#include "ThreadPool.hpp"

class ResolverPool: public ThreadPool
{
    private:
        SafeQueue   *dns_task_queue;
        unordered_map<string,char* ip> dns_ip_array;
    public :
        ResolverPool(SafeQueue *dns_requester_queue);

        void startPool();

};

#endif