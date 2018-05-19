#ifndef FileTask_hpp
#define FileTask_hpp

#include <fstream>
#include "Task.hpp"
#include "SafeQueue.hpp"

class FileTask: public Task
{
    private:
        char    *m_FileName;
        SafeQueue *m_resolverQueue;
    public:
        FileTask(int id, char *file, SafeQueue *resolverQueue);
        ~FileTask();
        bool Run();
        char* getName();

};

#endif