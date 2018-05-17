#include "FileTask.hpp"
#include <iostream>

#include <time.h>
#include <unistd.h>
using namespace std;


FileTask::FileTask(int id, fstream *file):Task(id){
        m_File = file;
}

FileTask::~FileTask(){}

void FileTask::Run(){
    usleep(rand() % 1000);
    cout<<"Thread #"<<pthread_self()<<" execute task "<< m_id <<endl;
    // read lines from file    
}
