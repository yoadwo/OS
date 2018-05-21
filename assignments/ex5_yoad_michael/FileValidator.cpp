#include "FileValidator.hpp"

FileValidator::FileValidator(const char** filesArray, int filesArrSize) : arrFilesSize(filesArrSize)
{
    this->arrFiles = arrFiles;
}

FileValidator::~FileValidator() {}

int FileValidator::DomainCounter()
{
    domainCounter = 0;
    for (int i=1; i<arrFilesSize-1; i++)
    {
        string fileName(arrFiles[i]);
        ifstream myfile(fileName.c_str());
        if(myfile.is_open())
        {
            string str;
            while(getline(myfile, str))
            {
                domainCounter++;
            }
            myfile.close();
        }
        else
        {
            cerr << "Cannot open/find file : " << fileName << endl;
        }
    }
    return domainCounter;
}

string* FileValidator::toStringArray()
{
    string* result = new string[arrFilesSize-1];
    for (int i=1; i<arrFilesSize; i++)
    {
        result[i - 1] = string(arrFiles[i]);
    }
    return result;
}
