#include "threadpool.hpp"
#include <pthread.h>

#define DEFAULT_QUEUE_SIZE 5

WorkerThread::WorkerThread(){}
WorkerThread::~WorkerThread(){}

void WorkerThread::StartThread(int idx)
{
    pthread_create(&m_thread_id,NULL,ThreadProc,this);
    // we do not want them detached
    //pthread_detach(m_thread_id);
    m_idx = idx;
    cout << "thread " <<idx <<" is active\n";
}

void* WorkerThread::ThreadProc(void* arg)
{
    WorkerThread* pWorker = (WorkerThread*)arg;
    if (pWorker){
        pWorker->RunTask();
    }
    return NULL;
}

void WorkerThread::RunTask()
{
    while (true)
    {
        Task* pTask = m_thread_pool->PopTask();
        if (!pTask)
            break;
        //cout<<"Thread "<<m_idx<<" Running Task "<<endl;
        pTask->Run();
    }
}

ThreadPool::ThreadPool(int poolsize, bool bLinger):
        m_pool_size(poolsize),
        m_bRunning(false),
        m_bLinger(bLinger)
{
    if (m_pool_size > 0){
        m_thread_pool = new WorkerThread[m_pool_size];
    }
    for (int i = 0; i < m_pool_size; ++i){
        m_thread_pool[i].SetThreadPool(this);
    }
}

ThreadPool::~ThreadPool()
{
    delete [] m_thread_pool;
    
    while(!m_task_queue->isEmpty()){
        Task* pTask = m_task_queue->popTask();
        delete pTask;
    }
}

void ThreadPool::PushTask(Task* pTask)
{
        m_task_queue->pushTask(pTask);
//     m_notifier.Lock();

//     if (m_bRunning){
//         m_task_queue.push(pTask);
//         m_notifier.Notify();
//     }else{
//         cout<<"Pool has been stopped! Could not add new task ..."<<endl;
//     }

//     m_notifier.Unlock();
}

Task* ThreadPool::PopTask()
{
    return m_task_queue->popTask();
    // m_notifier.Lock();

    // Task* pTaskRet = NULL;

    // if (m_bLinger)
    // {
    //     while(m_bRunning && m_task_queue.empty()){
    //         m_notifier.Wait();
    //     }
    //     if (!m_task_queue.empty()){
    //         pTaskRet = m_task_queue.front();
    //         m_task_queue.pop();
    //     }
    // }
    // else
    // {
    //     while (m_bRunning && m_task_queue.empty()){
    //         m_notifier.Wait();
    //     }    

    //     if (m_bRunning){
    //         pTaskRet = m_task_queue.front();
    //         m_task_queue.pop(); 
    //     }
    // }
    // m_notifier.Unlock();
    // return pTaskRet;
}

pthread_t ThreadPool::GetThreadId(int idx)
{
    assert(idx >=0 && idx < m_pool_size);
    return m_thread_pool[idx].GetThreadId();
}

void ThreadPool::PoolStart()
{
    if (!m_bRunning){
        m_bRunning = true;
        for (int i = 0; i < m_pool_size; ++i){
            m_thread_pool[i].StartThread(i+1);
        }
        m_task_queue = new SafeQueue(DEFAULT_QUEUE_SIZE);
        cout<<"thread pool started ..."<<endl;
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

void ThreadPool:: Join(){
    for (int i=0; i< m_pool_size; i++)
        pthread_join(m_thread_pool[i].GetThreadId(), NULL);
}
