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
        size_t loop = 1;

        while (loop < 32)
        {
            ++loop;
            queue.push({printer, nullptr});
        }
    }

    return 0;
}