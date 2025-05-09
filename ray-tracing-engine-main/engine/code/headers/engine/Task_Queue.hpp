#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <functional>

namespace udit::engine
{

    class ITask;

    enum class Task_Priority
    {
        HIGH,
        NORMAL,
        LOW
    };

    //Comparison function for priority queue
    struct Task_Priority_Compare
    {
        bool operator()(const std::shared_ptr<ITask>& a, const std::shared_ptr<ITask>& b) const;
    };

    class Task_Queue
    {
    private:

        //Priority queue of tasks
        std::priority_queue<std::shared_ptr<ITask>, std::vector<std::shared_ptr<ITask>>, Task_Priority_Compare> tasks;

        mutable std::mutex queue_mutex;
        std::condition_variable condition;
        bool stop_flag;

    public:

        Task_Queue() : stop_flag(false){}
        ~Task_Queue() = default;

        //Push a task into the queue
        void push(std::shared_ptr<ITask>task);

        //Pop a task from the queue (blocks if empty until a task is available)
        std::shared_ptr<ITask> pop();

        //Try to pop a task without blocking (returns nullptr if empty)
        std::shared_ptr<ITask> try_pop();

        //Check if the queue is empty
        bool empty() const;

        //Get the current size of the queue
        size_t size() const;

        //Signal all waiting threads to stop
        void stop();
    };
}