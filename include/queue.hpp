#pragma once

#include <mutex>
#include <semaphore>

template <class T, size_t capacity>
class ThreadSafeQueue
{
private:
    std::counting_semaphore<capacity> empty;
    std::counting_semaphore<capacity> full;
    size_t firstIndex;
    size_t lastIndex;
    std::mutex mut;
    alignas(T) std::byte data[capacity * sizeof(T)];

public:
    ThreadSafeQueue();
    ~ThreadSafeQueue();
    void push(const T &obj);
    T pop();
};

template <class T, size_t capacity>
ThreadSafeQueue<T, capacity>::ThreadSafeQueue() : empty(std::counting_semaphore<capacity>(capacity)), full(std::counting_semaphore<capacity>(0)), firstIndex(0), lastIndex(0), mut(std::mutex())
{
    static_assert((capacity & (capacity - 1)) == 0, "Capacity must be a power of 2");
}

template <class T, size_t capacity>
void ThreadSafeQueue<T, capacity>::push(const T &obj)
{
    empty.acquire();
    mut.lock();

    lastIndex = (lastIndex + 1) % capacity;
    new (&data[lastIndex * sizeof(T)]) T(std::move(obj));

    mut.unlock();
    full.release();
}

template <class T, size_t capacity>
T ThreadSafeQueue<T, capacity>::pop()
{
    full.acquire();
    mut.lock();

    firstIndex = (firstIndex + 1) % capacity;
    T obj = std::move(reinterpret_cast<T *>(data)[firstIndex]);
    reinterpret_cast<T *>(data)[firstIndex].~T();

    mut.unlock();
    empty.release();
    return obj;
}

template <class T, size_t capacity>
ThreadSafeQueue<T, capacity>::~ThreadSafeQueue()
{
    mut.lock();
    while (firstIndex != lastIndex)
    {
        firstIndex = (firstIndex + 1) % capacity;
        reinterpret_cast<T *>(data)[firstIndex].~T();
    }
    mut.unlock();
}