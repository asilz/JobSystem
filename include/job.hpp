#pragma once

#include <functional>
#include <thread>
#include <queue.hpp>
#include <atomic>

class Job
{
public:
    std::function<void(void *)> work;
    void *arg;
};

template <size_t capacity, size_t threadCount>
class JobQueue
{
private:
    ThreadSafeQueue<Job, capacity> queue;
    std::thread threads[threadCount];
    std::atomic_bool terminate;

    void runThread();

public:
    JobQueue();
    ~JobQueue();
    void push(const Job &job);
};

template <size_t capacity, size_t threadCount>
void JobQueue<capacity, threadCount>::push(const Job &job)
{
    queue.push(job);
}

template <size_t capacity, size_t threadCount>
JobQueue<capacity, threadCount>::JobQueue() : queue(ThreadSafeQueue<Job, capacity>()), terminate(false)
{
    for (size_t i = 0; i < threadCount; ++i)
    {
        threads[i] = std::thread(&JobQueue<capacity, threadCount>::runThread, this);
    }
}

template <size_t capacity, size_t threadCount>
void JobQueue<capacity, threadCount>::runThread()
{
    while (!terminate)
    {
        Job job = queue.pop();
        job.work(job.arg);
    }
}

template <size_t capacity, size_t threadCount>
JobQueue<capacity, threadCount>::~JobQueue()
{
    terminate = true;
    for (size_t i = 0; i < threadCount; ++i)
    {
        if (threads[i].joinable())
        {
            threads[i].join();
        }
    }
}