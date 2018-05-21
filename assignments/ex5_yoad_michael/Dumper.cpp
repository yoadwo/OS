#include "Dumper.hpp"

Dumper::Dumper(string* resultArray, int resultArraySize, string outputFile): resultArraySize(resultArraySize)
{
    this->resultArray = resultArray;
    this->outputFile = outputFile;
}

Dumper::~Dumper() {}

int Dumper::run()
{
    thread = new pthread_t[1];
    int result = pthread_create(thread, NULL, &Dumper::wrapperWriteToFile, this);
    if(result)
    {
        cerr << "Error : return code from pthread_create() is : " << result << endl;
        exit(1);
    }
    pthread_join(*thread, NULL);
    return 0;
}

void* Dumper::writeToFile()
{
    string resultFileName(outputFile);
    ofstream fs;
    fs.open(resultFileName.c_str());
    if(fs.is_open())
    {
        for (int i=0; i<resultArraySize; i++)
        {
            fs << resultArray[i] << endl;
        }
        fs.close();
    }
    else
    {
        cerr << "Cannot open/create result file : " << resultFileName << endl;
    }
    pthread_exit(NULL);
}


void* Dumper::wrapperWriteToFile(void* obj)
{
    reinterpret_cast<Dumper*>(obj)->writeToFile();
    pthread_exit(NULL);
}

