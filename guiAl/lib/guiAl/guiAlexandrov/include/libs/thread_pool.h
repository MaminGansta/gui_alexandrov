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
#define MAX_THREADS 16
#endif


	struct ThreadPool
	{
		std::vector<std::thread> pool;
		std::queue<std::function<void()>> tasks;
		std::condition_variable event;
		std::mutex event_mutex;
		bool stopping;


		ThreadPool(size_t threads = MAX_THREADS);

		~ThreadPool();

		
		// add task

		template <typename F, typename ...Args>
		auto add_task(F&& task, Args&& ...args) ->std::future<decltype(task(args...))>
		{
		
			auto wrapper = std::make_shared<std::packaged_task<decltype(task(args...))()>>(
				std::bind(std::forward<F>(task), std::forward<Args>(args)...)
			);

			{
				std::unique_lock<std::mutex> lock(event_mutex);
				tasks.push([wrapper]() { (*wrapper)(); });
			}
		
			event.notify_one();
			return wrapper->get_future();
		}


		// task return void (common use case)
		std::future<void> add_task_void(std::function<void()> task);


		void resize(int size);

		int size();

	private:

		void start(int size);

		void stop() noexcept;
	};


	// declaration global thread pool
	extern ThreadPool thread_pool;






	// =========== Async for function ===========


	/*
		Split range from small pices
		
		Prototype: F(int from, int to)
	*/
	template <typename F>
	void parallel_for(int from_param, int to_param, bool wait, F func)
	{
		// split whole task on pisces
		std::future<void> res[MAX_THREADS];
		int width = to_param - from_param;
		int threads = __min(width, thread_pool.size());

		// put pisces on thread pool
		for (int i = 0; i < gui::thread_pool.size(); i++)
		{
			int from = i * width / gui::thread_pool.size() + from_param;
			int to = (i + 1) * width / gui::thread_pool.size() + from_param;

			res[i] = gui::thread_pool.add_task(func, from, to);
		}

		// wait for result if neccessery
		if (wait)
		{
			for (int i = 0; i < threads; i++)
				res[i].get();
		}
	}




	//  ============ Async macro =============

		/* 
			lamda with necessary params [from, to]

			[from, to](){ for (int i = from; i < to; i++}
		*/
#define ASYNC_FOR(from_param, to_param)											     \
			{																	     \
				std::future<void> res[MAX_THREADS];								     \
				int width = to_param - from_param;							         \
				int threads = __min(width, gui::thread_pool.size());                 \
				for (int i = 0; i < threads; i++)								     \
				{																     \
					int from = i * width / gui::thread_pool.size() + from_param;	 \
					int to = (i + 1) * width / gui::thread_pool.size() + from_param; \
					res[i] = gui::thread_pool.add_task_void(



#define END_FOR																	     \
					);															     \
				}																     \
																				     \
				for (int i = 0; i < threads; i++)						             \
					res[i].get();												     \
			}

}