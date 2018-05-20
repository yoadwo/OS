#include "FileTask.hpp"
#include "IpTask.hpp"
#include <iostream>
#include <fstream>

#include <time.h>
#include <unistd.h>
using namespace std;


FileTask::FileTask(int id, char *file, SafeQueue *resolverQueue, unordered_map <string,vector<char*>> *dns_ip_array):
Task(id), m_FileName(file), m_resolverQueue(resolverQueue), m_dns_ip_array(dns_ip_array){}

FileTask::~FileTask(){}

bool FileTask::Run(){
    ifstream file(m_FileName);
    string str;
    if (!file.is_open()){
        pthread_mutex_lock(&m_TaskScreenMutex);
        cerr<<"Thread #"<<pthread_self()<<" file "<< m_FileName <<" open failed.\n";
        pthread_mutex_unlock(&m_TaskScreenMutex);
        return false;
    }
    usleep(rand() % 250);
    pthread_mutex_lock(&m_TaskScreenMutex);
    cout<<"Thread #"<<pthread_self()<<" reads from file  "<< m_FileName <<endl;
    pthread_mutex_unlock(&m_TaskScreenMutex);

    // IpTasks indexing: last 2 digits of calling threads + counter
    // i.e thread#  = 4445225, first IpTask is 25000, second is 25001 and so on
    int taskPrefix = (pthread_self() % 100) * 1000, taskSuffix = 0;
    // read all lines from file
    while (getline(file, str))
    {
        pthread_mutex_lock(&m_TaskScreenMutex);
        cout<< m_FileName <<": " <<str <<"\n";
        m_resolverQueue->pushTask(new IpTask(taskPrefix + taskSuffix,str, m_dns_ip_array));
        pthread_mutex_unlock(&m_TaskScreenMutex);
    }

    return true;   
} 