#ifndef FileTask_hpp
#define FileTask_hpp
#include <ifstream>
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