#pragma once

#include <string>
inline uint64_t strHash(const std::string& str)
{
	constexpr uint64_t offset_basis = 0xCBF29CE484222325ui64;
	constexpr uint64_t prime = 0x100000001B3ui64;

	uint64_t hash = offset_basis;
	for (auto it = str.begin(); it != str.end(); it++)
	{
		hash ^= static_cast<uint8_t>(*it);
		hash *= prime;
	}
	return hash;
}

#include <thread>
#include <vector>
#include <queue>
#include <functional>
#include <mutex>
#include <future>

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

	ThreadPool(size_t size = std::max(std::thread::hardware_concurrency(), unsigned int(1))) : size(size), stop(false), activeCount(0)
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
	~ThreadPool()
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
	void wait()
	{
		std::unique_lock lock(threadMutex);

		waitCondition.wait(lock, [this]() {return activeCount == 0 && tasks.empty(); });
	}
};


inline int ExecuteBF(const char* cmds, size_t size, std::function<int(void)> input = getchar, std::function<void(int)> output = putchar)
{
	std::vector<char> memory;
	memory.resize(size, 0);
	const char* cmd = cmds;
	char* pointer = memory.data();
	int loopCount = 0;
	while (*cmd != '\0')
	{
		if (cmd < cmds || pointer < memory.data() || pointer >= memory.data() + size)
			return 0;
		if (loopCount == 0)
		{
			switch (*cmd)
			{
			case '>':
				++pointer;
				++cmd;
				break;
			case '<':
				--pointer;
				++cmd;
				break;
			case '+':
				++(*pointer);
				++cmd;
				break;
			case '-':
				--(*pointer);
				++cmd;
				break;
			case '.':
				output(*pointer);
				++cmd;
				break;
			case ',':
				*pointer = input();
				++cmd;
				break;
			case '[':
				if (!*pointer)
					loopCount = -1;
				else
					++cmd;
				break;
			case ']':
				if (*pointer)
					loopCount = 1;
				else
					++cmd;
				break;
			default:
				++cmd;
			}
		}
		else
		{
			if (loopCount < 0)
			{
				++cmd;
			}
			else
			{
				--cmd;
			}
			if (*cmd == ']')
				++loopCount;
			if (*cmd == '[')
				--loopCount;
		}
	}
	return (loopCount == 0 ? 1 : 0);
}