#include <job.hpp>

void printer(void *ptr)
{
    std::thread::id threadID = std::this_thread::get_id();
    uint64_t *p = (uint64_t *)(&threadID);
    printf("thread %lx\n", *p);
}

int main(void)
{
    {
        JobQueue queue = JobQueue<32, 4>();
        Job job{printer, nullptr};
        size_t loop = 1;

        while (loop < 50)
        {
            ++loop;
            queue.push(job);
        }
    }

    return 0;
}