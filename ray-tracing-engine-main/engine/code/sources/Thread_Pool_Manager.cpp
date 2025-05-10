
#include"engine/Thread_Pool_Manager.hpp"

namespace udit::engine
{
	//Initialize static members
	Thread_Pool_Manager* Thread_Pool_Manager::instance = nullptr;
	std::mutex Thread_Pool_Manager::instance_mutex;

	Thread_Pool_Manager::Thread_Pool_Manager()
	{
		//Create default thread pools
		initialize();
	}

	Thread_Pool_Manager& Thread_Pool_Manager::get_instance()
	{
		if (instance == nullptr)
		{
			std::lock_guard<std::mutex> lock(instance_mutex);
			if (instance == nullptr)
			{
				instance = new Thread_Pool_Manager();
			}
		}

		return *instance;
	}

	Thread_Pool_Manager::~Thread_Pool_Manager()
	{
		shutdown();
	}

	Thread_Pool& Thread_Pool_Manager::get_pool(Thread_Pool_Type type)
	{
		auto it = pools.find(type);
		if (it == pools.end())
		{
			//If pool doesn't exist, create it with default settings
			pools[type] = std::make_unique<Thread_Pool>();
			return *pools[type];
		}
		return *it->second;
	}

	void Thread_Pool_Manager::initialize(size_t general_threads,
										size_t rendering_threads,
										size_t loading_threads,
										size_t input_threads)
	{
		// Create the thread pools with specified thread counts
		pools[Thread_Pool_Type::GENERAL] = std::make_unique<Thread_Pool>(general_threads);
		pools[Thread_Pool_Type::RENDERING] = std::make_unique<Thread_Pool>(rendering_threads);
		pools[Thread_Pool_Type::LOADING] = std::make_unique<Thread_Pool>(loading_threads);
		pools[Thread_Pool_Type::INPUT] = std::make_unique<Thread_Pool>(input_threads);
	}

	void Thread_Pool_Manager::shutdown()
	{
		//Clear all thread pools
		pools.clear();
	}
}