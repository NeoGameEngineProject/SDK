#include <ThreadPool.h>

#include <thread>
#include <vector>

#include <iostream>

using namespace Neo;

#define QUEUE_SIZE 128

namespace
{
#ifndef NEO_SINGLE_THREAD
moodycamel::ConcurrentQueue<std::function<void()>> jobs(QUEUE_SIZE);
std::vector<std::thread> threads;
std::atomic<bool> running(false);
std::atomic<size_t> numJobs(0);

std::condition_variable conditional;
#endif
}

void ThreadPool::start(unsigned int numThreads)
{
#ifndef NEO_SINGLE_THREAD
	assert(threads.size() == 0 && numThreads > 0 && !running);
	
	running = true;
	for(unsigned int i = 0; i < numThreads; i++)
	{
		std::thread thread(ThreadPool::work);
		threads.push_back(std::move(thread));
	}
#endif
}

void ThreadPool::stop()
{
#ifndef NEO_SINGLE_THREAD
	running = false;
	conditional.notify_all();
	
	for(auto& t : threads)
	{
		while(!t.joinable());
		t.join();
	}
	
	threads.clear();
#endif
}

void ThreadPool::pushJob(const std::function<void()>& job)
{
#ifndef NEO_SINGLE_THREAD
	numJobs++;
	jobs.enqueue(job);
	conditional.notify_one();
#endif
}

void ThreadPool::work()
{
#ifndef NEO_SINGLE_THREAD
	while(running)
	{
		std::mutex mtx;
		do
		{
			std::unique_lock<std::mutex> lock(mtx);
			conditional.wait(lock, [] {
					// Continue running if there is work or the thread needs
					// to finish its job.
					return !running || numJobs != 0;
				});
			
			// Wait for notification
			std::function<void()> job;
			while(jobs.try_dequeue(job))
			{
				job();
				numJobs--;
			}
			
		} while(running);
	}
	
	// Finish up last parts in the work queue
	std::function<void()> job;
	while(jobs.try_dequeue(job))
	{
		job();
		numJobs--;
	}
#endif
}

void ThreadPool::synchronize()
{
#ifndef NEO_SINGLE_THREAD
	std::function<void()> job;
	while(jobs.try_dequeue(job))
	{
		job();
		numJobs--;
	}
#endif
}
