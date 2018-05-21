#ifndef FileTask_hpp
#define FileTask_hpp

#include "Task.hpp"

class FileTask : public Task {
    
public:
    FileTask(string str);
    virtual ~FileTask();
    string getName();
};

#endif /* FileTask_hpp */
