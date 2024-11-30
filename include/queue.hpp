#pragma once

#include <queue>
#include <mutex>
#include <semaphore>

template <class T, size_t capacity>
class ThreadSafeQueue
{
private:
    std::counting_semaphore<capacity> empty;
    std::counting_semaphore<capacity> full;
    T *first;
    T *last;
    std::mutex mut;
    alignas(T) std::byte data[capacity * sizeof(T)];

public:
    ThreadSafeQueue();
    ~ThreadSafeQueue();
    void push(const T &obj);
    T pop();
};

template <class T, size_t capacity>
ThreadSafeQueue<T, capacity>::ThreadSafeQueue() : empty(std::counting_semaphore<capacity>(capacity)), full(std::counting_semaphore<capacity>(0)), first(reinterpret_cast<T *>(&data)), last(reinterpret_cast<T *>(&data)), mut(std::mutex())
{
}

template <class T, size_t capacity>
void ThreadSafeQueue<T, capacity>::push(const T &obj)
{
    empty.acquire();
    mut.lock();
    if (++last == reinterpret_cast<T *>(&data[capacity * sizeof(T)]))
    {
        last = reinterpret_cast<T *>(data);
    }
    new (last) T(std::move(obj));
    mut.unlock();
    full.release();
}

template <class T, size_t capacity>
T ThreadSafeQueue<T, capacity>::pop()
{
    full.acquire();
    mut.lock();
    if (++first == reinterpret_cast<T *>(&data[capacity * sizeof(T)]))
    {
        first = reinterpret_cast<T *>(data);
    }
    T obj = std::move(*first);
    first->~T();
    mut.unlock();
    empty.release();
    return obj;
}

template <class T, size_t capacity>
ThreadSafeQueue<T, capacity>::~ThreadSafeQueue()
{
    mut.lock();
    while (first != last)
    {
        if (++first == reinterpret_cast<T *>(&data[capacity * sizeof(T)]))
        {
            first = reinterpret_cast<T *>(data);
        }
        first->~T();
    }
    mut.unlock();
}