
#include "libs/thread_pool.h"


namespace gui
{
	// Thead pool initialization
	ThreadPool thread_pool;



	ThreadPool::ThreadPool(size_t threads)
	{
		int size = __min(std::thread::hardware_concurrency(), threads);
		stopping = false;
		start(size);
	}


	ThreadPool::~ThreadPool()
	{
		stop();
	}


	std::future<void> ThreadPool::add_task_void(std::function<void()> task)
	{
		auto wrapper = std::make_shared<std::packaged_task<void()>>(std::move(task));
		{
			std::unique_lock<std::mutex> lock(event_mutex);
			tasks.push([=]() { (*wrapper)(); });
		}
		event.notify_one();
		return wrapper->get_future();
	}


	void ThreadPool::start(int size)
	{
		for (int i = 0; i < size; i++)
		{
			pool.push_back(std::thread([&]() {
				while (true)
				{
					std::function<void()> task;
					{
						std::unique_lock<std::mutex> lock(event_mutex);

						event.wait(lock, [&]() { return stopping || !tasks.empty(); });
						if (stopping && tasks.empty()) break;

						task = std::move(tasks.front());
						tasks.pop();
					}
					task();
				}
				}));
		}
	}


	void ThreadPool::resize(int new_size)
	{
		new_size = __min(new_size, MAX_THREADS);

		if (size() == new_size)
			return;

		// free all if down size
		if (new_size < size())
		{
			{
				std::unique_lock<std::mutex> lock(event_mutex);
				stopping = true;
			}

			event.notify_all();

			for (std::thread& thread : pool)
				thread.join();

			pool.clear();
		}


		new_size = new_size - size();
		stopping = false;

		// create new threads
		for (int i = 0; i < new_size; i++)
		{
			pool.push_back(std::thread([&]()
			{
				while (true)
				{
					std::function<void()> task;
					{
						std::unique_lock<std::mutex> lock(event_mutex);

						event.wait(lock, [&]() { return stopping || !tasks.empty(); });
						if (stopping && tasks.empty()) break;

						task = std::move(tasks.front());
						tasks.pop();
					}
					task();
				}
			}));
		}
	}


	int ThreadPool::size()
	{
		return pool.size();
	}

	
	void ThreadPool::stop() noexcept
	{
		{
			std::unique_lock<std::mutex> lock(event_mutex);
			stopping = true;
		}

		event.notify_all();

		for (std::thread& thread : pool)
			thread.join();
	}

	
}