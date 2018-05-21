

#include "ThreadPool.hpp"
#include "SharedMemory.hpp"
#include "Dumper.hpp"
#include "FileValidator.hpp"

int main(int argc, const char * argv[])
{    
    cout << "BEGIN TASK 5: DNS REQUESTER-RESOLVER\n";
    cout << "====================================\n";
    // check there are enough arguments
    if(argc < 3)
    {
        cerr << "Invalid number of files" << endl;
        exit(1);
    }

    // output file must exist for program to run
    if(!ifstream(argv[argc - 1]))
    {
        cout << "named output file <" <<argv[argc-1] <<"> does not exist, program will create default file \"result.txt\"\n";
        cout << "====================================\n";
        ofstream result ("result.txt");
        if (result.is_open())
        {
            argv[argc-1] = "result.txt";
            result.close();
        }
        else {
            cerr << "Unable to open file";
            return 1;
        }
        
    }
    
    // File validator and task counter
    FileValidator* fileValidator = new FileValidator(argv , argc);
    
    // Request Queue initialized at shared memory
    SharedMemory* sharedMemory = new SharedMemory(argc-2, fileValidator->DomainCounter());
    
    // Request thread pool
    ThreadPool* requestPool = new ThreadPool(argc-2, argc-1, fileValidator->toStringArray(), sharedMemory, "REQUESTER", fileValidator->DomainCounter());
    requestPool->run();
    
    // Resolver thread pool
    int nResolvers = 0;
    if(argc == 3)
        nResolvers = 2;
    else
        nResolvers = argc - 2;

    ThreadPool* resolverPool = new ThreadPool(nResolvers, argc-1, fileValidator->toStringArray(), sharedMemory, "RESOLVER", fileValidator->DomainCounter());
    resolverPool->run();
    
    // Wait for all threads complete all tasks
    requestPool->join();
    resolverPool->join();
    
    // Dumper thread
    Dumper* dumper = new Dumper(sharedMemory->getArray(), sharedMemory->getResultsArraySize(), fileValidator->toStringArray()[argc-2]);
    dumper->run();
    
    // call the destructor for the placed object
    sharedMemory->~SharedMemory();

    return 0;
}







































