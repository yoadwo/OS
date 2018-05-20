#ifndef FileTask_hpp
#define FileTask_hpp

#include <fstream>
#include <unordered_map>
#include <vector>
#include "Task.hpp"
#include "SafeQueue.hpp"

class FileTask: public Task
{
    private:
        char    *m_FileName;
        SafeQueue *m_resolverQueue;
        unordered_map <string,vector<char*>> *m_dns_ip_array;
        
    public:
        FileTask(int id, char *file, SafeQueue *resolverQueue, unordered_map <string,vector<char*>> *dns_ip_array);
        ~FileTask();
        bool Run();
        char* getName();

};

#endif