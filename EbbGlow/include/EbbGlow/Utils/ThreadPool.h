#pragma once

#include <thread>
#include <vector>
#include <queue>
#include <functional>
#include <mutex>
#include <future>

namespace ebbglow::utils
{
	class ThreadPool
	{
	private:
		size_t size;
		bool stop;

		std::vector<std::thread> workers;
		std::queue<std::function<void()>> tasks;

		std::condition_variable condition;
		std::condition_variable waitCondition;
		std::mutex threadMutex;

		std::atomic<size_t> activeCount;

	public:

		ThreadPool(size_t size = std::max(std::thread::hardware_concurrency(), unsigned int(1)));
		~ThreadPool();

		template<typename Func, typename ...Args>
		auto enqueue(Func&& func, Args&& ...args) -> std::future<decltype(func(args...))>
		{
			using return_type = decltype(func(args...));
			auto argsTuple = std::make_tuple(std::forward<Args>(args)...);

			auto task = std::make_shared<std::packaged_task<return_type()>>(
				[func = std::forward<Func>(func),
				args_tuple = std::move(argsTuple)]() mutable
				{
					return std::apply(func, args_tuple);
				}
			);

			std::future<return_type> result = task->get_future();

			{
				std::lock_guard lock(threadMutex);
				tasks.emplace([task]() { (*task)(); });
			}
			condition.notify_one();
			return result;
		}

		void wait();
	};
}