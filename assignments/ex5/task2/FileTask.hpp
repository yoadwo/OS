#ifndef FileTask_hpp
#define FileTask_hpp

#include <fstream>
#include "Task.hpp"

class FileTask: public Task
{
    private:
        fstream    *m_File;
    public:
        FileTask(int id, fstream *file);
        ~FileTask();
        void Run();

};

#endif