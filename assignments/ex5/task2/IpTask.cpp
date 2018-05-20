#include "IpTask.hpp"


IpTask::IpTask(int id,string name, unordered_map<string,vector <char*>>  *array):
Task(id),m_Name(name),m_dns_ip_array(array){
}  

IpTask::~IpTask(){}

bool IpTask::Run(){
    char ipstr[1024];

	if(dnslookup(m_Name.c_str(), ipstr, sizeof(ipstr)) == UTIL_FAILURE){
		cout<<"DNS lookup error\n";
		return false;
	}

	// check if item exists
	unordered_map<string,vector <char*>>::const_iterator got = m_dns_ip_array->find (m_Name);
	// if not exists, init values container virst
	if ( got == m_dns_ip_array->end() )
		(*m_dns_ip_array)[m_Name] = vector<char*>();
	// if exists, add to value to key
	else
		(*m_dns_ip_array)[m_Name].push_back(ipstr);
	return true;
    
}















