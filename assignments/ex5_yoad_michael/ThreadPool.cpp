 #include "ThreadPool.hpp"
pthread_mutex_t ThreadPool::cond_mutex = PTHREAD_MUTEX_INITIALIZER;
bool ThreadPool::flag = true;

ThreadPool::ThreadPool(int amountOfThreads, int amountOfFiles, string* filesArray, SharedMemory* sharedMemory, string sentFunction, int amountOfDomains): nFiles(amountOfFiles), nDomains(amountOfDomains)
{
    this->nThreads = amountOfThreads;
    this->function = sentFunction;
    if(function.compare("REQUESTER") == 0)
    {
        this->arrFiles = filesArray;
        safeQueue = SafeQueue(nFiles-1);
    }
    else // resolver
    {
        arrFiles = NULL;
    }
    arrThreads = new pthread_t[nThreads];
    this->sharedMemory = sharedMemory;
    
    if(pthread_mutex_init(&file_mutex, NULL)!=0)
    {
        cerr << "Mutex init fail" << endl;
        exit(1);
    }
    if(pthread_mutex_init(&flag_mutex, NULL)!=0)
    {
        cerr << "Mutex init fail" << endl;
        exit(1);
    }
}

ThreadPool::~ThreadPool()
{
    pthread_mutex_destroy(&file_mutex);
    pthread_mutex_destroy(&flag_mutex);
    pthread_mutex_destroy(&cond_mutex);
    delete[] arrThreads;
}

void ThreadPool::run()
{
    if(function.compare("REQUESTER") == 0)
    {
        for (int i=0; i<nFiles-1; i++)
        {
            safeQueue.pushTask(new FileTask(arrFiles[i]));
        }
        int result;
        for(int i=0; i<nThreads; i++)
        {
            result = pthread_create(&arrThreads[i], NULL, &ThreadPool::wrapperRequest, this);
            if(result)
            {
                cerr << "Error : return code from pthread_create() is : " << result << endl;
                exit(1);
            }
        }
    }
    else // resolver
    {
        int result;
        for(int i=0; i<nThreads; i++)
        {
            result = pthread_create(&arrThreads[i], NULL, &ThreadPool::wrapperResolve, this);
            if(result)
            {
                cerr << "Error : return code from pthread_create() is : " << result << endl;
                exit(1);
            }
        }
    }
}
//  request action
void* ThreadPool::Request()
{
    pthread_mutex_lock(&file_mutex);
    if(!safeQueue.isEmpty())
    {
        Task* file = safeQueue.popTask();

        if(file != NULL)
        {
            string fileName(file->getName());
            pthread_mutex_unlock(&file_mutex);
            ifstream myfile(fileName.c_str());
            if(myfile.is_open())
            {
                string str;
                while(getline(myfile, str))
                {
                    pthread_mutex_lock(&cond_mutex);
                    Task* task = new DomainTask(str);
                    sharedMemory->getQueue()->pushTask(task);
                    DomainTask* domainTask = dynamic_cast<DomainTask*>(task);
                    pthread_cond_wait(&domainTask->cv, &cond_mutex);
                    cout << domainTask->getName() << endl;
                    delete task;
                    pthread_mutex_unlock(&cond_mutex);
                }
                myfile.close();
            }
            else
            {
                cerr << "Cannot open/find file : " << fileName << endl;
            }
        }
      
    }
    else
    {
        pthread_mutex_unlock(&file_mutex);
    }
    pthread_exit(NULL);
}
//resolver action
void* ThreadPool::Resolve()
{
    while(haveRequest())
    {
        pthread_mutex_lock(&cond_mutex);
        Task* task = sharedMemory->getQueue()->popTask();
        if (task)
        {
            string request(task->getName());
            char** ipArray = new char*[MAX_ADDRESSES];
            for(int i = 0; i < MAX_ADDRESSES; i++)
            {
                ipArray[i] = new char[1025];
            }
            int addressCount = 0;
            dnslookupAll(request.c_str(), ipArray, MAX_ADDRESSES, &addressCount);
            string hostAndIP = "";
            hostAndIP += request;
            for (int i=0; i<addressCount; i++)
            {
                hostAndIP += ",";
                hostAndIP += ipArray[i];
            }
            if(addressCount == 0)
            {
                cerr << "Failed to resolve domain name: " << hostAndIP << endl;
                hostAndIP += ",";
            }
            
            task->name = hostAndIP;
            sharedMemory->addStringToArray(hostAndIP);
            DomainTask* domainTask = dynamic_cast<DomainTask*>(task);
            pthread_cond_signal(&domainTask->cv);
            pthread_mutex_unlock(&cond_mutex);
        }
        else
        {
            pthread_mutex_unlock(&cond_mutex);
        }
    }
    pthread_exit(NULL);
}
//join threads
void ThreadPool::join()
{
    for(int i=0; i<nThreads; i++)
    {
        pthread_join(arrThreads[i], NULL);
    }
    if(function.compare("REQUESTER") == 0)
    {
        pthread_mutex_lock(&flag_mutex);
        flag = false;
        pthread_mutex_unlock(&flag_mutex);
    }
}
// returns true if there any request
bool ThreadPool::haveRequest()
{
    pthread_mutex_lock(&flag_mutex);
    bool result = flag;
    pthread_mutex_unlock(&flag_mutex);
    return result;
}
//casts object when calling request                                                                                                  
void* ThreadPool::wrapperRequest(void* obj)
{
    reinterpret_cast<ThreadPool*>(obj)->Request();
    pthread_exit(NULL);
}
//casts object when calling resolve  
void* ThreadPool::wrapperResolve(void* obj)
{
    reinterpret_cast<ThreadPool*>(obj)->Resolve();
    pthread_exit(NULL);
}
