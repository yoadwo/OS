
#ifndef _TASK_
#define _TASK_

using namespace std;

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