
#ifndef _TASK_
#define _TASK_

class Task
{
public:
    Task(void* param):m_param(param){}
    virtual ~Task(){}
    virtual void Run() = 0;
protected:
    void* m_param;
};

#endif