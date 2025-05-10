
#include"engine/Task_Queue.hpp"
#include"engine/Task.hpp"

namespace udit::engine
{
    bool Task_Priority_Compare::operator()(const std::shared_ptr<ITask>& a, const std::shared_ptr<ITask>& b) const
    {
        //Lower value means higher priority
        return a->get_priority() > b->get_priority();
    }

    void Task_Queue::push(std::shared_ptr<ITask> task)
    {
        {
            std::lock_guard<std::mutex>lock(queue_mutex);
            tasks.push(std::move(task));
        }

        condition.notify_one();
    }

    std::shared_ptr<ITask> Task_Queue::pop()
    {
        std::unique_lock<std::mutex> lock(queue_mutex);

        //Wait until the queue has tasks or stop is signaled
        condition.wait(lock, [this] 
            {
            return !tasks.empty() || stop_flag;
            });

        //Return nullptr if stop was signaled
        if (stop_flag && tasks.empty()) 
        {
            return nullptr;
        }

        auto task = tasks.top();
        tasks.pop();
        return task;
    }

    std::shared_ptr<ITask> Task_Queue::try_pop()
    {
        std::lock_guard<std::mutex> lock(queue_mutex);

        if (tasks.empty())
        {
            return nullptr;
        }

        auto task = tasks.top();
        tasks.pop();
        return task;
    }

    bool Task_Queue::empty() const
    {
        std::lock_guard<std::mutex>lock(queue_mutex);
        return tasks.empty();
    }

    size_t Task_Queue::size() const
    {
        std::lock_guard<std::mutex> lock(queue_mutex);
        return tasks.size();
    }

    void Task_Queue::stop()
    {
        {
            std::lock_guard<std::mutex> lock(queue_mutex);
            stop_flag = true;
        }

        condition.notify_all();
    }
}
