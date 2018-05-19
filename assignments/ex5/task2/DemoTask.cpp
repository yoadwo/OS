#include "Task.hpp"
#include <iostream>

#include <time.h>
#include <unistd.h>
using namespace std;

class DemoTask : public Task
{
public:
    DemoTask(int id):Task(id){}
    ~DemoTask(){}
    bool Run(){
        usleep(rand() % 1000);
        cout<<"Thread #"<<pthread_self()<<" execute task "<< m_id <<endl;
        return true;
    }
};