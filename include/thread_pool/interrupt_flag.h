#ifndef INTERRUPT_FLAG_H
#define INTERRUPT_FLAG_H

#include <atomic>
#include <condition_variable>


class InterruptFlag;
extern thread_local InterruptFlag this_thread_interrupt_flag;
/**
 * @class InterruptFlag
 * @brief 可中断线程池的标志类
 */
class InterruptFlag {
    std::atomic<bool> _flag;
    std::condition_variable* _thread_cond;
    std::mutex _set_clear_mutex;

  public:
    InterruptFlag() : _thread_cond(0) {}

    void Set() {
        _flag.store(true, std::memory_order_relaxed);
        std::lock_guard<std::mutex> lk(_set_clear_mutex);
        if (_thread_cond) {
            _thread_cond->notify_all();
        }
    }

    bool IsSet() const {
        return _flag.load(std::memory_order_relaxed);
    }

    void SetConditionVariable(std::condition_variable& cv) {
        std::lock_guard<std::mutex> lk(_set_clear_mutex);
        _thread_cond = &cv;
    }

    void ClearConditionVariable() {
        std::lock_guard<std::mutex> lk(_set_clear_mutex);
        _thread_cond = 0;
    }

    struct ClearCvOnDestruct {
        ~ClearCvOnDestruct() {
            this_thread_interrupt_flag.ClearConditionVariable();
        }
    };
};


#endif