#ifndef FileTask_hpp
#define FileTask_hpp
#include <ifstream>
#include <fstream>
#include "Task.hpp"

class FileTask: public Task
{
    private:
        ifstream *m_File;
    public:
        RequesterPool(int poolsize, ifstream *inputFiles, bool bLinger);
};

#endif