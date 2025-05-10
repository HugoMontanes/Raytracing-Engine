
#pragma once

#include <vector>
#include <thread>
#include <atomic>
#include <future>
#include <functional>
#include <type_traits>
#include <string>
#include "engine/Task_Queue.hpp"
#include "engine/Task.hpp"

namespace udit::engine
{
	class Thread_Pool
	{
	private:

		//Worker threads
		std::vector<std::thread> workers;

		//Task queue
		Task_Queue task_queue;

		//Atomic variables for thread pool state
		std::atomic<bool> stop_flag;
		std::atomic<size_t> active_threads;

		//Main worker function
		void worker_function();

	public:

		//Constructor with optional thread count (default to hardware concurrency)
		explicit Thread_Pool(size_t thread_count = 0);

		//Destructor (join all thread)
		~Thread_Pool();

		//Disable copy and move
		Thread_Pool(const Thread_Pool&) = delete;
		Thread_Pool& operator = (const Thread_Pool&) = delete;
		Thread_Pool(Thread_Pool&&) = delete;
		Thread_Pool& operator = (Thread_Pool&&) = delete;

		//Submit a task to the thread pool and get a future for its resul
		template<typename Func, typename...Args>
		auto submit(Task_Priority priority, Func&& func, Args&&... args);

		//Overload for submitting with default priority
		template<typename Func, typename...Args>
		auto submit( Func&& func, Args&&... args)
		{
			return submit(Task_Priority::NORMAL, std::forward<Func>(func), std::forward<Args>(args)...);
		}

		//Wait for all tasks to complete
		void wait_all();

		//Get the number of tasks in the queue
		size_t get_queue_size() const;

		//Get the number of active woker threads
		size_t get_active_threads() const;

		//Get the total number of worker threads
		size_t get_thread_count() const;

	};

	//Template implementation for submit
	template<typename Func, typename...Args>
	auto Thread_Pool::submit(Task_Priority priority, Func&& func, Args&&...args)
	{
		//Create a packaged task with the function and arguments
		using return_type = std::invoke_result_t<Func, Args...>;

		auto bound_func = std::bind(std::forward<Func>(func), std::forward<Args>(args)...);
		auto task = std::make_shared<Task<return_type>>(std::move(bound_func), priority);

		//Get the future before pushing the task
		auto future = task->get_future();

		//Push the task to the queue
		task_queue.push(task);

		return future;
	}
}
