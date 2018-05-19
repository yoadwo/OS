#ifndef FileTask_hpp
#define FileTask_hpp

#include <fstream>
#include "Task.hpp"

class FileTask: public Task
{
    private:
        char    *m_FileName;
    public:
        FileTask(int id, char *file);
        ~FileTask();
        bool Run();
        char* getName();

};

#endif