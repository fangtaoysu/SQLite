#ifndef THREAD_SAFE_QUEUE_H
#define THREAD_SAFE_QUEUE_H


#include <iostream>
#include <mutex>
#include <queue>
#include <functional>
#include <condition_variable>


/**
 * @class ThreadSafeQueue
 * @brief 给加入线程池的任务排队
 */
template<typename T>
class ThreadSafeQueue {
  private:
    mutable std::mutex mut;
    std::queue<T> _data_queue;
    std::condition_variable _data_cond;
  public:
    ThreadSafeQueue() {}
    
    void push(T new_value) {
        std::lock_guard<std::mutex> lk(mut); // 加锁
        _data_queue.push(std::move(new_value)); // 入队
        _data_cond.notify_one(); // 唤醒一个工作线程
    }

    std::shared_ptr<T> WaitAndPop() {
        std::unique_lock<std::mutex> lk(mut);
        _data_cond.wait(lk, [this]{return !_data_queue.empty();});
        std::shared_ptr<T> res(std::make_shared<T>(std::move(_data_queue.front())));
        _data_queue.pop();
        return res;
    }

    bool TryPop(T& value) {
        std::lock_guard<std::mutex> lk(mut);
        if (_data_queue.empty()) {
            return false;
        }
        // 显示执行移动赋值运算符，避免拷贝
        value = std::move(_data_queue.front());
        _data_queue.pop();
        return true;
    }
    
    std::shared_ptr<T> TryPop() {
        std::lock_guard<std::mutex> lk(mut);
        if (_data_queue.empty()) {
            return std::shared_ptr<T>();
        }
        std::shared_ptr<T> res(std::make_shared<T>(std::move(_data_queue.front())));
        _data_queue.pop();
        return res;
    }

    bool empty() const {
        std::lock_guard<std::mutex> lk(mut);
        return _data_queue.empty();
    }
};


#endif