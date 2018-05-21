
#ifndef DomainTask_hpp
#define DomainTask_hpp

#include "Task.hpp"

class DomainTask: public Task {
    
public:
    DomainTask(string str);
    ~DomainTask();
    string getName();
    void setName(string str);
    pthread_cond_t cv;
};

#endif /* DomainTask_hpp */
