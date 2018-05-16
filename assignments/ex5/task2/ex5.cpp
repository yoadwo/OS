#include <stdio.h>
#include <pthread.h>
#include <time.h>    

#include <iostream>
#include <fstream>
#include <sstream>
#include <string> 
#include <thread>
#include <cstdlib>

#include "threadpool.hpp"
#include "DemoTask.cpp"

using namespace std;

bool openFiles(ifstream *input, ofstream *output, int argc, char* argv[]){
    int i;
    for (i=1; i < argc -1; i++){
        cout << "open file " << argv[i] << "\n";
        input[i-1].open(argv[i], ios::in);
        if (!input[i-1].is_open()){
            cerr << "File open Error: " << argv[i] << "\n";
            return false;
        }
    }
    (*output).open(argv[i]);
    if (!(*output).is_open()){
        cerr << "File open Error: " << argv[i] << "\n";
        return false;
    }
    return true;
}

void closeFiles(ifstream *input, ofstream *output, int argc ){
    int i;
    for (i=1; i< argc -1; i++){
        input[i-1].close();
    }
    (*output).close();
}

int main(int argc, char* argv[]){
    cout << "hello task ex5\n";
    srand (time(NULL));

    ifstream inputFiles[argc-2];
    ofstream output;
    string line;

    if (!openFiles(inputFiles, &output, argc, argv)){
        cerr << "File Error.. Exiting.\n";
        getchar();
        return 1;
    }
    

    pthread_mutex_t m_screenMutex;
    pthread_mutex_init(&m_screenMutex, NULL);


    ThreadPool *pPoolRequester = new ThreadPool(2, "REQUESTER", true);
    pPoolRequester->PoolStart();
    for (int i = 0; i < 30; ++i){
        DemoTask *demo = new DemoTask(new int(i+1));    
        pPoolRequester->PushTask(demo);
    }
    
    closeFiles(inputFiles, &output, argc);
    pPoolRequester->PoolStop();
    pthread_mutex_destroy(&m_screenMutex);

    cout << "Press Any Key To Continue...\n";
    getchar();
    return 0;
}

