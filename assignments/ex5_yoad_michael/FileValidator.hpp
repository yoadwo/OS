#ifndef FileValidator_hpp
#define FileValidator_hpp

#include <string>
#include <fstream>
#include <iostream>

using namespace std;

class FileValidator {

private:
    const char** arrFiles;
    int arrFilesSize;
    int domainCounter;
public:
    FileValidator(const char** filesArray, int filesArraySize);
    ~FileValidator();
    int DomainCounter();
    string* toStringArray();
    
};

#endif /* FileValidator_hpp */
