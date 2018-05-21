#include "DomainTask.hpp"

DomainTask::DomainTask(string str) : Task(str)
{
    if(pthread_cond_init(&cv, NULL)!=0)
    {
        cerr << "Cond init fail" << endl;
        exit(1);
    }
}

DomainTask::~DomainTask()
{
    pthread_cond_destroy(&cv);
}

string DomainTask::getName()
{
    return name;
}

void DomainTask::setName(string str)
{
    name = str;
}
