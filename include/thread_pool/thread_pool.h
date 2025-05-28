#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <atomic>
#include <thread>
#include <type_traits>
#include <future>

#include "./function_warpper.h"
#include "./thread_safe_queue.h"
#include "./join_threads.h"
#include "./interruptible_thread.h"


/**
 * @class ThreadPool
 * @brief 线程池接口
 * @details 支持lambda/函数指针/成员函数的任务，重载函数调用前需要声明类型
 */
class ThreadPool {
    std::atomic_bool _done;
    ThreadSafeQueue<FunctionWarpper> _work_queue;
    std::vector<std::thread> _threads;
    JoinThreads _joiner;

    void WorkerThread() {
        while (!_done) {
            FunctionWarpper task;
            // 尝试从队列中取任务
            if (_work_queue.TryPop(task)) {
                task(); // 执行任务
            } else {
                std::this_thread::yield(); // 队列空时让出cpu
            }
        }
    }

  public:
    ThreadPool() : _done(false), _joiner(_threads) {
        unsigned const thread_count = std::thread::hardware_concurrency();
        std::cout << "max hardware threads number: " << thread_count << std::endl;
        try {
            for (unsigned i = 0; i < thread_count; ++i) {
                _threads.push_back(std::thread(&ThreadPool::WorkerThread, this));
            }
        } catch (...) {
            _done = true;
            throw;
        }
    }
    
    ~ThreadPool() {
        _done = true;
    }

    template<typename FunctionType>
    std::future<typename std::result_of<FunctionType()>::type> submit(FunctionType f) {
        typedef typename std::result_of<FunctionType()>::type result_type;
        std::packaged_task<result_type()> task(std::move(f));
        std::future<result_type> res(task.get_future());
        _work_queue.push(std::move(task));
        return res;
    }

};


#endif