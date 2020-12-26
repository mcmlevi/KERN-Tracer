
#include <pch.h>
#include "Core/JobSystem.h"
#include "Core/RingBuffer.h"

#include <algorithm>    // std::max
#include <atomic>    // to use std::atomic<uint64_t>
#include <thread>    // to use std::thread
#include <condition_variable>    // to use std::condition_variable
#include <utility>


namespace JobSystem
{
    typedef std::pair<std::function<void()>, std::atomic<int>* > JobType;
    uint32_t numThreads = 0;    // number of worker threads, it will be initialized in the Initialize() function
    ThreadSafeRingBuffer < JobType, 256 > jobPool;    // a thread safe queue to put pending jobs onto the end (with a capacity of 256 jobs). A worker thread can grab a job from the beginning
    std::condition_variable wakeCondition;    // used in conjunction with the wakeMutex below. Worker threads just sleep when there is no job, and the main thread can wake them up
    std::mutex wakeMutex;    // used in conjunction with the wakeCondition above


    struct Context
    {
        std::atomic<uint32_t> counter{ 0 };
    };

	
    Context context{};
	
    inline bool work()
    {
        JobType job;
        if (jobPool.pop_front(job))
        {
            job.first();
            job.second->fetch_sub(1);
            return true;
        }
        return false;
    }

    bool IsBusy(std::atomic<int>* counter)
    {
        return counter->load() > 0;
    }

    void Wait(std::atomic<int>* counter)
    {
        // Wake any threads that might be sleeping:
        wakeCondition.notify_all();

        // Waiting will also put the current thread to good use by working on an other job if it can:
        while (IsBusy(counter)) { work(); }
    }
	
    void Initialize()
    {
        // Initialize the worker execution state to 0:
        context.counter.store(0);

        // Retrieve the number of hardware threads in this system:
        auto numCores = std::thread::hardware_concurrency();

        // Calculate the actual number of worker threads we want:
        numThreads = std::max(1u, numCores - RESERVE_CORES);

        // Create all our worker threads while immediately starting them:
        for (uint32_t threadID = 0; threadID < numThreads; ++threadID)
        {
            std::thread worker([] {
                while (true)
                {
                    if (!work())
                    {
                        // no job, put thread to sleep
                        std::unique_lock<std::mutex> lock(wakeMutex);
                        wakeCondition.wait(lock);
                    }
                }

                });

            // *****Here we could do platform specific thread setup...

            worker.detach(); // forget about this thread, let it do it's job in the infinite loop that we created above
        }
    }
    // This little helper function will not let the system to be deadlocked while the main thread is waiting for something


    void Execute(const std::function<void()>& job, std::atomic<int>* counter)
    {
        // The main thread label state is updated:
       counter->fetch_add(1);

        // Try to push a new job until it is pushed successfully:
        while (!jobPool.push_back(JobType{ job,counter })) { wakeCondition.notify_all(); }

        wakeCondition.notify_one(); // wake one thread
    }

    //void Dispatch(uint32_t jobCount, uint32_t groupSize, const std::function<void(JobDispatchArgs)>& job)
    //{
    //    if (jobCount == 0 || groupSize == 0)
    //    {
    //        return;
    //    }

    //    // Calculate the amount of job groups to dispatch (overestimate, or "ceil"):
    //    const uint32_t groupCount = (jobCount + groupSize - 1) / groupSize;

    //    // The main thread label state is updated:
    //    context.counter.fetch_add(1);

    //    for (uint32_t groupIndex = 0; groupIndex < groupCount; ++groupIndex)
    //    {
    //        // For each group, generate one real job:
    //        auto jobGroup = [jobCount, groupSize, job, groupIndex]() {

    //            // Calculate the current group's offset into the jobs:
    //            const uint32_t groupJobOffset = groupIndex * groupSize;
    //            const uint32_t groupJobEnd = std::min(groupJobOffset + groupSize, jobCount);

    //            JobDispatchArgs args;
    //            args.groupIndex = groupIndex;

    //            // Inside the group, loop through all job indices and execute job for each index:
    //            for (uint32_t i = groupJobOffset; i < groupJobEnd; ++i)
    //            {
    //                args.jobIndex = i;
    //                job(args);
    //            }
    //        };

    //        // Try to push a new job until it is pushed successfully:
    //        while (!jobPool.push_back(jobGroup)) { wakeCondition.notify_all(); }

    //        wakeCondition.notify_one(); // wake one thread
    //    }
    //}
}
