#pragma once

#include <cmath>
#include <thread>
#include <vector>
#include <condition_variable>
#include <functional>
#include <queue>
#include <future>


namespace gui
{

#ifndef MAX_THREADS
#define MAX_THREADS 8
#endif

#ifndef MIN
#define MIN(a, b) (a < b ? a : b)
#endif

	struct thread_pool
	{
		size_t size;
		std::vector<std::thread> pool;
		std::queue<std::function<void()>> tasks;
		std::condition_variable event;
		std::mutex event_mutex;
		bool stopping;


		thread_pool(size_t threads = 8);

		~thread_pool();

		template <typename T>
		auto add_task(T task)->std::future<decltype(task())>
		{
			auto wrapper = std::make_shared<std::packaged_task<decltype(task()) ()>>(std::move(task));
			{
				std::unique_lock<std::mutex> lock(event_mutex);
				tasks.push([=]() { (*wrapper)(); });
			}
			event.notify_one();
			return wrapper->get_future();
		}


	private:

		void start();

		void stop() noexcept;
	};


	// declaration global thread pool
	extern thread_pool workers;



	// lamda with necessary params [from, to](){ for (int i = from; i < to; i++}
#define ASYNC_FOR(from_param, to_param)											\
			{																	\
				std::future<void> res[MAX_THREADS];								\
				int af_width = to_param - from_param;							\
				for (int i = 0; i < workers.size; i++)							\
				{																\
					int from = i * af_width / workers.size + from_param;		\
					int to = (i + 1) * af_width / workers.size + from_param;	\
					res[i] = workers.add_task(



#define END_FOR																	\
					);															\
				}																\
																				\
				for (int i = 0; i < workers.size; i++)							\
					res[i].get();												\
			}

}