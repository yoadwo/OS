
#ifndef _TASK_
#define _TASK_

#include <pthread.h>

using namespace std;

class Task
{
public:
    Task(int id):m_id(id){
        pthread_mutex_init(&m_TaskScreenMutex, NULL);
    }
    virtual ~Task(){
        pthread_mutex_destroy(&m_TaskScreenMutex);
    }
    virtual bool Run() = 0;
    int getID(){return m_id;}
protected:
    int m_id;
    pthread_mutex_t m_TaskScreenMutex;
};

#endif