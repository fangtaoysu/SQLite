#pragma once

#include <type_traits>
#include <future>

#include "./function_warpper.h"
#include "./thread_safe_queue.h"
#include "./join_threads.h"
// #include "./interruptible_thread.h"


/**
 * @class ThreadPool
 * @brief 线程池接口
 * @details 支持lambda/函数指针/成员函数的任务，重载函数调用前需要声明类型
 */
class ThreadPool {
  private:
    std::atomic_bool _done;
    ThreadSafeQueue<FunctionWarpper> _work_queue;
    std::vector<std::thread> _threads;
    JoinThreads _joiner;

    void WorkerThread();

  public:
    ThreadPool();
    
    ~ThreadPool() {
        _done = true;
    }
    
    template<typename FunctionType>
    std::future<typename std::result_of<FunctionType()>::type> submit(FunctionType f);
};

template<typename FunctionType>
std::future<typename std::result_of<FunctionType()>::type> ThreadPool::submit(FunctionType f) {
    typedef typename std::result_of<FunctionType()>::type result_type;
    std::packaged_task<result_type()> task(std::move(f));
    std::future<result_type> res(task.get_future());
    _work_queue.push(std::move(task));
    return res;
}