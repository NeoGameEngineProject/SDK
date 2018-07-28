#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <type_traits>
#include <future>
#include <functional>

#include <queue/concurrentqueue.h>

namespace Neo
{
class ThreadPool
{
	static void pushJob(const std::function<void()>& job);

public:
	static void work();
	static void start(unsigned int numThreads = 4);
	static void stop();
	static void synchronize();
	
#define FunctionResult std::result_of_t<std::decay_t<Function>(std::decay_t<Args>...)>
	
	template< class Function, class... Args>
	static std::future<FunctionResult>
	schedule(Function&& f, Args&&... args)
	{
		auto promise = std::make_shared<std::promise<FunctionResult>>();
		auto result = promise->get_future();
		
		auto job = std::bind([f, promise, args...] () {
			promise->set_value(f(args...));
		});
		
		ThreadPool::pushJob(job);
		return result;
	}
	
#undef FunctionResult

};
}

#endif
