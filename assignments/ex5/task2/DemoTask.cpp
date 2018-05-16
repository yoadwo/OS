#include "Task.hpp"
#include <iostream>

#include <time.h>
#include <unistd.h>
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