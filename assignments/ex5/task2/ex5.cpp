#include <stdio.h>
#include <pthread.h>
#include <time.h>    

#include <iostream>
#include <fstream>
#include <sstream>
#include <string> 
#include <thread>
#include <cstdlib>
#include <vector>

#include "RequesterPool.hpp"
#include "ResolverPool.hpp"


using namespace std;

/* bool openFiles(fstream **input[], fstream **output, int argc, char* argv[]){
    int i;
    for (i=1; i < argc -1; i++){
        cout << "open file " << argv[i] << "\n";
        (*input[i-1])->open(argv[i]);
        if (!(*input[i-1])->is_open()){
            cerr << "File open Error: " << argv[i] << "\n";
            return false;
        }
    }
    (*output)->open(argv[i]);
    if (!(*output)->is_open()){
        cerr << "File open Error: " << argv[i] << "\n";
        return false;
    }
    return true;
} */


/* void closeFiles(vector <fstream*> input, fstream *output, int argc ){
    int i;
    for (i=1; i< argc -1; i++){
        input[i-1]->close();
    }
    output->close();
} */

int main(int argc, char* argv[]){
    cout << "hello task ex5\n";
    srand (time(NULL));

    int nRequesterThreads = argc -2, nResolverThreads = 2;

    pthread_mutex_t m_screenMutex;
    pthread_mutex_init(&m_screenMutex, NULL);

    // init resolver threads
    ResolverPool *pPoolResolver = new ResolverPool(nResolverThreads, argv[argc-1], true);
    if (!pPoolResolver->isOutputOpen()){
        cerr << "Failed to open Output file, Exiting.\n";
        pPoolResolver->~ResolverPool();
        return 1;
    }
    // init request threads
    RequesterPool *pPoolRequester = new RequesterPool(nRequesterThreads, argc, argv, true);
    // push file tasks to files queue
    pPoolRequester->PushTasks(pPoolResolver->getTaskQueue());
    // pop file tasks, read files and push as dns tasks to dns queue
    pPoolRequester->PoolStart();
    pPoolResolver->PoolStart();
    
    
    //closeFiles(inputFiles, output, argc);
    pPoolRequester->PoolStop();
    pthread_mutex_destroy(&m_screenMutex);

    cout << "Press Any Key To Continue...\n";
    getchar();
    return 0;
}

