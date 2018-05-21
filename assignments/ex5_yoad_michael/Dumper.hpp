#ifndef Dumper_hpp
#define Dumper_hpp

#include <iostream>
#include <pthread.h>
#include <fstream>
#include <string>
#include <stdlib.h>

using namespace std;

class Dumper {
  
private:
    int resultArraySize;
    pthread_t* thread;
    string* resultArray;
    string outputFile; 


public:
    Dumper(string* resultArray, int resultArraySize, string outputFile);
    ~Dumper();
    int run();
    void* writeToFile();
    static void* wrapperWriteToFile(void* obj);
      
};

#endif /* Dumper_hpp */
