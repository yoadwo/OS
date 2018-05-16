#include "SafeQueue.hpp"


SafeQueue::SafeQueue() {}

SafeQueue::SafeQueue(int maxSize) : queueMaxSize(maxSize)
{
    if (pthread_mutex_init(&mutex, NULL) != 0)
    {
        cerr << "Mutex init fail" << endl;
        exit(1);
    }
    queueActualSize = 0;
    queue = NULL;
}

SafeQueue::~SafeQueue()
{
    pthread_mutex_destroy(&mutex);
}

void SafeQueue::pushTask(Task *task)
{
    pthread_mutex_lock(&mutex);
    while (queueMaxSize == queueActualSize)
    {
        pthread_mutex_unlock(&mutex);
        usleep(rand() % 100 + 5);
        pthread_mutex_lock(&mutex);
    }

    if (queueActualSize == 0)
    {
        queue = new Task *[1];
        queue[0] = task;
        queueActualSize++;
    }
    else
    {
        
        Task **temp = new Task *[queueActualSize];
        for (int i = 0; i < queueActualSize; i++)
        {
            temp[i] = queue[i];
        }
        delete[] queue;
        queue = new Task *[queueActualSize + 1];
        for (int i = 0; i < queueActualSize; i++)
        {
            queue[i] = temp[i];
        }
        queue[queueActualSize] = task;
        queueActualSize++;
        delete[] temp;
    }
    pthread_mutex_unlock(&mutex);
}

Task *SafeQueue::popTask()
{
    pthread_mutex_lock(&mutex);
    if (queueActualSize > 0)
    {
        Task *task(queue[0]);
        Task **temp = new Task *[queueActualSize - 1];
        for (int i = 1; i < queueActualSize; i++)
        {
            temp[i - 1] = queue[i];
        }
        delete[] queue;
        queue = new Task *[queueActualSize - 1];
        for (int i = 0; i < queueActualSize - 1; i++)
        {
            queue[i] = temp[i];
        }
        queueActualSize--;
        pthread_mutex_unlock(&mutex);
        return task;
    }
    else
    {
        pthread_mutex_unlock(&mutex);
        return NULL;
    }
}

bool SafeQueue::isEmpty()
{
    pthread_mutex_lock(&mutex);
    bool result = (queueActualSize == 0) ? true : false;
    pthread_mutex_unlock(&mutex);
    return result;
}

int SafeQueue::size()
{
    pthread_mutex_lock(&mutex);
    int result = queueActualSize;
    pthread_mutex_unlock(&mutex);
    return result;
}
