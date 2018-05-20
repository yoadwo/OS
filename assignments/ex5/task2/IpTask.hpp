
#ifndef IpTask_hpp
#define IpTask_hpp


#include <stdio.h>
#include <string.h>

#include <fstream>
#include "Task.hpp"
#include <unordered_map>
#include <vector>
#include <iostream>

extern "C"
{
	#include "util.h"
}

using namespace std;
class IpTask: public Task
{
    private:
        string    m_Name;
        unordered_map <string,vector<char*>> *m_dns_ip_array;
    public:
        IpTask(int id,string name,unordered_map<string,vector <char*>> *array);
        ~IpTask();
        bool Run();
        //char* getName();

};


#endif
