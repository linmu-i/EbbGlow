#include <EbbGlow/Utils/ThreadPool.h>

namespace ebbglow::utils
{
	ThreadPool::ThreadPool(size_t size) : size(size), stop(false), activeCount(0)
	{
		for (size_t i = 0; i < size; i++)
		{
			workers.push_back(std::thread([this]()
				{
					while (true)
					{
						std::function<void()> task;
						{
							std::unique_lock lock(threadMutex);

							condition.wait(lock, [this]() {return stop || !tasks.empty(); });
							if (stop && tasks.empty())
							{
								return;
							}
							task = std::move(tasks.front());
							tasks.pop();
						}
						++activeCount;
						task();
						--activeCount;
						{
							std::lock_guard lock(threadMutex);
							if (activeCount == 0 && tasks.empty())
							{
								waitCondition.notify_all();
							}
						}
					}
				}));
		}
	}

	ThreadPool::~ThreadPool()
	{
		{
			std::lock_guard lock(threadMutex);
			stop = true;
		}
		condition.notify_all();
		for (auto& worker : workers) {
			if (worker.joinable()) {
				worker.join();
			}
		}
	}

	void ThreadPool::wait()
	{
		std::unique_lock lock(threadMutex);
		waitCondition.wait(lock, [this]() {return activeCount == 0 && tasks.empty(); });
	}
}