#include "ThreadPool.hpp"
#include <pthread.h>


#define DEFAULT_QUEUE_SIZE 5

WorkerThread::WorkerThread(){
    pthread_mutex_init(&m_workerScreenMutex, NULL);
}
WorkerThread::~WorkerThread(){
    pthread_mutex_destroy(&m_workerScreenMutex);
}

void WorkerThread::StartThread(int idx)
{
    pthread_create(&m_thread_id,NULL,ThreadProc,this);
    // we do not want them detached
    // pthread_detach(m_thread_id);
    m_idx = idx;
    pthread_mutex_lock(&m_workerScreenMutex);
    cout << "thread " <<idx <<" is active\n";
    pthread_mutex_unlock(&m_workerScreenMutex);
}

void* WorkerThread::ThreadProc(void* arg)
{
    WorkerThread* pWorker = (WorkerThread*)arg;
    if (pWorker){
        pWorker->RunTask();
    }
    pthread_exit(NULL);
}

void WorkerThread::RunTask()
{
    while (true)
    {
        //TODO: google "variable lookup c++ inheritence"
        Task* pTask = m_thread_pool->PopTask();
        if (!pTask)
            break;
        pthread_mutex_lock(&m_workerScreenMutex);
        cout<<"Thread "<<m_idx<<" Running Task "<<endl;
        pthread_mutex_unlock(&m_workerScreenMutex);
        if (!pTask->Run()){
            pthread_mutex_lock(&m_workerScreenMutex);
            cout << "Task failed, returning task " << pTask->getID() <<" back to queue";
            m_thread_pool->PushTask(pTask);

        }
    }
}

/*
    ThreadPool c-tor: init pool as array of threads
        each thread then points to the pool itself
*/
ThreadPool::ThreadPool(int poolsize, bool bLinger):
        m_pool_size(poolsize),
        //m_poolType(poolType),
        m_bLinger(bLinger),
        m_bRunning(false)

{
    if (m_pool_size > 0){
        m_thread_pool = new WorkerThread[m_pool_size];
    }
    for (int i = 0; i < m_pool_size; ++i){
        m_thread_pool[i].SetThreadPool(this);
    }
    // arbitrarily init tasks queue with same size of pool
    m_task_queue = new SafeQueue(poolsize);
}

ThreadPool::~ThreadPool()
{
    delete [] m_thread_pool;
    
    while(!m_task_queue->isEmpty()){
        Task* pTask = m_task_queue->popTask();
        delete pTask;
    }
}

/* PoolStart(): call "start thread" on each thread
    poolStart -> m_pool[i] -> threadStart -> create(threadProc)
    -> runTask() -> task.run()
*/
void ThreadPool::PoolStart()
{
    if (!m_bRunning){
        m_bRunning = true;
        for (int i = 0; i < m_pool_size; ++i){
            m_thread_pool[i].StartThread(i+1);
        }
        cout<<"all threads have been started ..."<<endl;
    }else{
        cout<<"thread pool already started ..."<<endl;
    }
}

void ThreadPool::PoolStop()
{
    if (m_bRunning){
        m_bRunning = false;
        m_notifier.NotifiAll();
        Join();
        cout<<"thread pool is stopping......."<<endl;
    }else{
        cout<<"thread pool already stopped ......."<<endl;
    }
}

void ThreadPool::PushTask(Task* pTask)
{
        m_task_queue->pushTask(pTask);
/*     m_notifier.Lock();

    if (m_bRunning){
        m_task_queue.push(pTask);
        m_notifier.Notify();
    }else{
        cout<<"Pool has been stopped! Could not add new task ..."<<endl;
    }

    m_notifier.Unlock(); */
}

Task* ThreadPool::PopTask()
{
    return m_task_queue->popTask();
/*  m_notifier.Lock();

    Task* pTaskRet = NULL;

    if (m_bLinger)
    {
        while(m_bRunning && m_task_queue.empty()){
            m_notifier.Wait();
        }
        if (!m_task_queue.empty()){
            pTaskRet = m_task_queue.front();
            m_task_queue.pop();
        }
    }
    else
    {
        while (m_bRunning && m_task_queue.empty()){
            m_notifier.Wait();
        }    

        if (m_bRunning){
            pTaskRet = m_task_queue.front();
            m_task_queue.pop(); 
        }
    }
    m_notifier.Unlock();
    return pTaskRet;
    */
}

pthread_t ThreadPool::GetThreadId(int idx)
{
    assert(idx >=0 && idx < m_pool_size);
    return m_thread_pool[idx].GetThreadId();
}


void ThreadPool:: Join(){
    for (int i=0; i< m_pool_size; i++)
        pthread_join(m_thread_pool[i].GetThreadId(), NULL);
}
