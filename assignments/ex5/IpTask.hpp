#ifndef IpTask_hpp
#define IpTask_hpp

#include <fstream>
#include "Task.hpp"
#include <map>

class IpTask: public Task
{
    private:
        char*    m_Name;
       unordered_map<string,char* ip> dns_ip_array;
    public:
        IpTask(char* name,unordered_map<string,char* ip> array);
        ~IpTask();
        bool Run();
        //char* getName();

};


#endif