#include "Task.hpp"
#include <iostream>

#include <time.h>
#include <unistd.h>
using namespace std;

class FileTask : public Task
{
public:
    FileTask(void* param):Task(param){}
    ~FileTask(){}
    void Run(){
        usleep(rand() % 1000);
        cout<<"Thread #"<<pthread_self()<<" execute task "<<*(int*)m_param<<endl;
        // read lines from file
        
    }
};