#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <type_traits>
#include <future>
#include <functional>

#include <queue/concurrentqueue.h>

#include "NeoCore.h"

namespace Neo
{
class NEO_CORE_EXPORT ThreadPool
{
	static void pushJob(const std::function<void()>& job);

public:
	static void work();
	static void start(unsigned int numThreads = 0);
	static void stop();
	static void synchronize();
	static unsigned int threadCount();
	
#define FunctionResult std::result_of_t<std::decay_t<Function>(std::decay_t<Args>...)>
	
	template< class Function, class... Args>
	static std::future<FunctionResult>
	schedule(Function&& f, Args&&... args)
	{
#ifndef NEO_SINGLE_THREAD
		auto promise = std::make_shared<std::promise<FunctionResult>>();
		auto result = promise->get_future();
		
		auto job = std::bind([f, promise, args...] () {
			promise->set_value(f(args...));
		});
		
		ThreadPool::pushJob(job);
		return result;
#else
		std::promise<FunctionResult> promise;
		promise.set_value(f(args...));
		return promise.get_future();
#endif
	}
	
#undef FunctionResult

	template<typename Iterator, typename Functor>
	static void foreach(Iterator begin, Iterator end, Functor fn)
	{
		assert(ThreadPool::threadCount());
		size_t offset = std::distance(begin, end) / ThreadPool::threadCount();
		
		for(Iterator i = begin; std::distance(begin, i) < std::distance(begin, end); i += offset)
		{
			ThreadPool::schedule([](Iterator rangeBegin, Iterator rangeEnd, Iterator end, Functor fn) {
				for(Iterator i = rangeBegin; i != rangeEnd && i != end; i++)
					fn(*i);
				
				return true;
			}, i, i + offset, end, fn);
		}
	}
};
}

#endif
