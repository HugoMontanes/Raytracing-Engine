
#include "engine/Thread_Pool.hpp"
#include <iostream>

namespace udit::engine
{
	Thread_Pool::Thread_Pool(size_t thread_count)
		: stop_flag(false), active_threads(0)
	{
		//Use hardware concurrency if thread_count is 0
		if (thread_count == 0)
		{
			thread_count = std::thread::hardware_concurrency();
			//Fallback to 2 if hardware_concurrency returns 0
			if (thread_count == 0)
			{
				thread_count = 2;
			}
		}

		//Create worker threads
		workers.reserve(thread_count);
		for (size_t i = 0; i < thread_count; ++i)
		{
			workers.emplace_back(&Thread_Pool::worker_function, this);
		}
	}

	Thread_Pool::~Thread_Pool()
	{
		//Set stop flag and stop the task queue
		stop_flag = true;
		task_queue.stop();

		//Join all worker threads
		for (auto& worker : workers)
		{
			if (worker.joinable())
			{
				worker.join();
			}
		}
	}

	void Thread_Pool::worker_function()
	{

		while (!stop_flag)
		{

			// Get a task from the queue
			auto task = task_queue.pop();

			if (task)
			{

				// Increment active thread counter
				++active_threads;

				// Execute the task
				task->execute();

				// Decrement active threads counter
				--active_threads;

			}
		}

	}

	void Thread_Pool::wait_all()
	{
		//Wait until all tasks are completed
		while (get_queue_size() > 0 || get_active_threads() > 0)
		{
			std::this_thread::yield();
		}
	}

	size_t Thread_Pool::get_queue_size() const
	{
		return task_queue.size();
	}

	size_t Thread_Pool::get_active_threads() const
	{
		return active_threads.load();
	}

	size_t Thread_Pool::get_thread_count() const
	{
		return workers.size();
	}
}