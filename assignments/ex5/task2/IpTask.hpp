
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
        char*    m_Name;
       unordered_map<string,vector<char>> dns_ip_array;
    public:
        IpTask(int id,char* name,unordered_map<string,vector <char>> array);
        ~IpTask();
        bool run();
        //char* getName();

};


#endif
