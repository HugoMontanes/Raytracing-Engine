
#pragma once

#include <functional>
#include <memory>
#include <future>
#include <type_traits>
#include "Task_Queue.hpp"

namespace udit::engine
{

    //Base task interface
    class ITask
    {
    public:

        virtual ~ITask() = default;

        //Execute the task
        virtual void execute() = 0;

        //Get the priority of the task
        virtual Task_Priority get_priority() const = 0;
    };

    //Template task implementation that can return values via std::future
    template<typename Result>
    class Task : public ITask
    {
    private:

        std::function<Result()> func;
        std::promise<Result> promise;
        Task_Priority priority;

    public:

        Task(std::function<Result()> func, Task_Priority priority = Task_Priority::NORMAL)
            : func(std::move(func)), priority(priority){}
        
        void execute() override
        {
            try
            {
                if constexpr (std::is_void_v<Result>)
                {
                    func();
                    promise.set_value();
                }
                else
                {
                    promise.set_value(func());
                }
            }
            catch (...)
            {
                //Caputre any exceptions and forward them to the future
                promise.set_exception(std::current_exception());
            }
        }

        Task_Priority get_priority() const override
        {
            return priority;
        }

        //Get the future associated with this task
        std::future<Result> get_future()
        {
            return promise.get_future();
        }
    };
}