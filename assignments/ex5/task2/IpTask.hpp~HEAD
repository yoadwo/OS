#ifndef IpTask_hpp
#define IpTask_hpp

#include "Task.hpp"

#include <fstream>
#include <vector>
#include <unordered_map>


class IpTask: public Task
{
    private:
        char*    m_Name;
        unordered_map <string,char*> dns_ip_array;
    public:
        IpTask(char* name,unordered_map<string,vector <char*>> array);
        ~IpTask();
        bool Run();
        //char* getName();

};


#endif