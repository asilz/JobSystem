#pragma once

#include <functional>
#include <thread>
#include <queue.hpp>
#include <atomic>
#include <condition_variable>

class Job
{
public:
    std::function<void(void *)> work;
    void *arg;
    char *test;
    Job(std::function<void(void *)> func_, void *arg_) : work(func_), arg(arg_) { test = new char; };
    Job(const Job &job)
    {
        this->arg = job.arg;
        this->test = new char;
        this->work = job.work;
    }
    ~Job() { delete test; };
};

template <size_t capacity, size_t threadCount>
class JobQueue
{
private:
    ThreadSafeQueue<Job, capacity> queue;
    std::thread threads[threadCount];
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
JobQueue<capacity, threadCount>::JobQueue() : queue(ThreadSafeQueue<Job, capacity>())
{
    for (size_t i = 0; i < threadCount; ++i)
    {
        threads[i] = std::thread(&JobQueue<capacity, threadCount>::runThread, this);
    }
}

template <size_t capacity, size_t threadCount>
void JobQueue<capacity, threadCount>::runThread()
{
    while (1)
    {
        Job job = queue.pop();
        if (job.work)
        {
            job.work(job.arg);
        }
        else
        {
            return;
        }
    }
}

template <size_t capacity, size_t threadCount>
JobQueue<capacity, threadCount>::~JobQueue()
{
    for (size_t i = 0; i < threadCount; ++i)
    {
        push({std::function<void(void *)>(), nullptr});
    }
    for (size_t i = 0; i < threadCount; ++i)
    {
        if (threads[i].joinable())
        {
            threads[i].join();
        }
    }
}