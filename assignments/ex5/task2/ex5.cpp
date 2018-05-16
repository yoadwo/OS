#include <stdio.h>
#include <pthread.h>
#include <time.h>    

#include <iostream>
#include <sstream>
#include <string> 
#include <thread>
#include <cstdlib>

#include "threadpool.hpp"

using namespace std;

class DemoTask : public Task
{
public:
    DemoTask(void* param):Task(param){}
    ~DemoTask(){}
    void Run(){
        usleep(rand() % 1000);
        cout<<"Thread #"<<pthread_self()<<" execute task "<<*(int*)m_param<<endl;
        
    }
};

int main(){
    cout << "hello task 5\n";
    srand (time(NULL));

    ThreadPool *pPool = new ThreadPool(3,true);
    pPool->PoolStart();
    for (int i = 0; i < 30; ++i){
        DemoTask *demo = new DemoTask(new int(i+1));    
        pPool->PushTask(demo);
    }
 
    pPool->PoolStop();

    cout << "Press Any Key To Continue...\n";
    getchar();
    return 0;
}

