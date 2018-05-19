#include "IpTask.hpp"


IpTask::IpTask(name,array):m_Name(name),dns_ip_array(array){

}  


void IpTask:: run(){
    char ipstr[1024];

	if(dnslookup(m_Name, ipstr, sizeof(ipstr)) == UTIL_FAILURE)
	{
		cout<<"Error"<<endl;
	}
	else
	{
		cout<<ipstr<<endl;
        dns_ip_array.insert(std::make_pair(m_Name, std::vector<char*>());
        for(int i=0;i<strlen(ipstr);i++){
            dns_ip_array[m_Name].pushBack(ipstr[i]);

        }
	}






}