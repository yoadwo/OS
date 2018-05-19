#include "FileTask.hpp"
#include <iostream>
#include <fstream>

#include <time.h>
#include <unistd.h>
using namespace std;


FileTask::FileTask(int id, char *file):
Task(id), m_FileName(file){}

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
    cout<<"Thread #"<<pthread_self()<<" beings read from file  "<< m_FileName <<endl;
    pthread_mutex_unlock(&m_TaskScreenMutex);

    while (getline(file, str))
    {
        pthread_mutex_lock(&m_TaskScreenMutex);
        cout<< m_FileName <<": " <<str <<"\n";
        pthread_mutex_unlock(&m_TaskScreenMutex);
    }

    return true;   
}