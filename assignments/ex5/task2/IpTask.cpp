#include "IpTask.hpp"


IpTask::IpTask(int id,char *name,unordered_map<string,vector <char*>>  array):Task(id),m_Name(name),dns_ip_array(array){

}  

IpTask::~IpTask(){}

bool IpTask::run(){
    char ipstr[1024];

	/* if(dnslookup(m_Name, ipstr, sizeof(ipstr)) == UTIL_FAILURE)
	{
		cout<<"Error"<<endl;
		return false;
	} *//* 
	else
	{
        
		int length = strlen(ipstr);
        //dns_ip_array.insert(std::make_pair(m_Name, std::vector<char>()));
		dns_ip_array[m_Name]=vector<char>();
        for(int i=0;i<length;i++)
            dns_ip_array[m_Name].push_back(ipstr[i]);

		return true;
        
	} */
	return true;
}















