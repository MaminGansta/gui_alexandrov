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


	struct thread_pool
	{
		std::vector<std::thread> pool;
		std::queue<std::function<void()>> tasks;
		std::condition_variable event;
		std::mutex event_mutex;
		bool stopping;


		thread_pool(size_t threads = MAX_THREADS);

		~thread_pool();

		// add task

		// task have return value
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

		// task return void
		std::future<void> add_task_void(std::function<void()> task);

		void resize(int size);

		int size();

	private:

		void start(int size);

		void stop() noexcept;
	};


	// declaration global thread pool
	extern thread_pool workers;


	// lamda with necessary params [from, to](){ for (int i = from; i < to; i++}
#define ASYNC_FOR(from_param, to_param)											 \
			{																	 \
				std::future<void> res[MAX_THREADS];								 \
				int af_width = to_param - from_param;							 \
				for (int i = 0; i < gui::workers.size(); i++)						 \
				{																 \
					int from = i * af_width / gui::workers.size() + from_param;	 \
					int to = (i + 1) * af_width / gui::workers.size() + from_param;\
					res[i] = gui::workers.add_task_void(



#define END_FOR																	\
					);															\
				}																\
																				\
				for (int i = 0; i < gui::workers.size(); i++)						\
					res[i].get();												\
			}

}