#include "../../include/thread_pool/thread_pool.h"
#include <iostream>


ThreadPool::ThreadPool() : _done(false), _joiner(_threads) {
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

void ThreadPool::WorkerThread() {
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