
#ifndef _TASK_
#define _TASK_

class Task
{
public:
    Task(int id):m_id(id){}
    virtual ~Task(){}
    virtual void Run() = 0;
protected:
    int m_id;
};

#endif