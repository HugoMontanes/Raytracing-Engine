
#pragma once

#include <unordered_map>
#include <string>
#include <memory>
#include <mutex>
#include "engine/Thread_Pool.hpp"

namespace udit::engine
{
	//Thread pool type for different purposes
	enum class Thread_Pool_Type
	{
		GENERAL,
		RENDERING,
		LOADING,
		INPUT
	};

	class Thread_Pool_Manager
	{
	private:

		//Singleton instance
		static Thread_Pool_Manager* instance;
		static std::mutex instance_mutex;

		//Thread pools by type
		std::unordered_map<Thread_Pool_Type, std::unique_ptr<Thread_Pool>> pools;

		//Private constructor for singleton
		Thread_Pool_Manager();

	public:

		//Get the singleton instance
		static Thread_Pool_Manager& get_instance();

		//Destructor
		~Thread_Pool_Manager();

		//Disable copy and move
		Thread_Pool_Manager(const Thread_Pool_Manager&) = delete;
		Thread_Pool_Manager& operator = (const Thread_Pool_Manager&) = delete;
		Thread_Pool_Manager(Thread_Pool_Manager&&) = delete;
		Thread_Pool_Manager& operator = (Thread_Pool_Manager&&) = delete;

		//Get a thread pool by type
		Thread_Pool& get_pool(Thread_Pool_Type type);

		//Initialize the thread pools with custom thread counts
		void initialize(size_t general_threads = 0,
						size_t rendering_threads = 0,
						size_t loading_threads = 0,
						size_t input_threads = 1);

		//Shutdown all thread pools
		void shutdown();
	};
}