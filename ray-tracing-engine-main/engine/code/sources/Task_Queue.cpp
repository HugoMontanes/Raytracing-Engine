
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
            
        }
    }
}
