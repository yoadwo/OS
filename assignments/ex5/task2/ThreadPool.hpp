
#ifndef _THREAD_POOL_
#define _THREAD_POOL_

#include <unordered_map>
#include <queue>
#include <iostream>

#include <pthread.h>
#include <assert.h>

#include "SafeQueue.hpp"

using namespace std;

class ThreadPool;

/*
** A Wrapper class of condition variable & mutex
*/
class TaskNotify
{
  public:
    TaskNotify()
    {
        pthread_mutex_init(&m_mutex, NULL);
        pthread_cond_init(&m_cond, NULL);
    }
    ~TaskNotify()
    {
        pthread_mutex_destroy(&m_mutex);
        pthread_cond_destroy(&m_cond);
    }
    void Lock() { pthread_mutex_lock(&m_mutex); }
    void Unlock() { pthread_mutex_unlock(&m_mutex); }
    void Notify() { pthread_cond_signal(&m_cond); }
    void NotifiAll() { pthread_cond_broadcast(&m_cond); }
    void Wait() { pthread_cond_wait(&m_cond, &m_mutex); }

  private:
    pthread_mutex_t m_mutex;
    pthread_cond_t m_cond;
};

/*
** WorkerThread.used to fetch task from task queue 
** and execute the task
*/

class WorkerThread
{
  public:
    WorkerThread();
    ~WorkerThread();
    void SetThreadPool(ThreadPool *pool) { m_thread_pool = pool; }
    void StartThread(int idx);
    static void *ThreadProc(void *);
    pthread_t GetThreadId() { return m_thread_id; }
    void RunTask();

  private:
    pthread_t m_thread_id;
    pthread_mutex_t m_workerScreenMutex;
    unsigned int m_idx;
    /*
    * m_thread_pool stores the pool object. we need the pool object
    * because we need to fetch task from task queue,which is a member
    * of the pool object
    */
    ThreadPool *m_thread_pool;
};

class ThreadPool
{
  public:
    ThreadPool(int poolsize, bool bLinger = false );
    ~ThreadPool();
    int GetPoolSize() { return m_pool_size; }
    void PushTask(Task *task);
    Task *PopTask();
    void PoolStart();
    void PoolStop();
    pthread_t GetThreadId(int idx);
    unordered_map   <string,vector<char*>>* getDns_ip_array();

  protected:
    SafeQueue       *m_task_queue;
    unordered_map <string,vector<char*>> *m_dns_ip_array;

  private:
    int             m_pool_size;
    //string          m_poolType;
    WorkerThread    *m_thread_pool;
    TaskNotify      m_notifier;     
    /*
    **When m_bLinger is false, A call to PoolStop will cause the 
    **thread stop fetch task from task queue and then stop.
    **When m_bLinger is true,thread will still fetch task from task 
    **queue until the task queue become empty.
    */
    bool            m_bLinger;
    bool            m_bRunning ;

    void Join();
};

#endif /*_THREAD_POOL_*/
