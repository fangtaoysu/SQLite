#pragma once

#include "interrupt_flag.h"
#include <future>
#include <thread>


// 考虑到这部分实现起来比较复杂，而且暂时还用不到，权且搁置

thread_local InterruptFlag this_thread_interrupt_flag;
/**
 * @class InterruptibleThread
 * @brief 实现可中断线程的类
 */
class InterruptibleThread {
    std::thread _internal_thread;
    InterruptFlag* _flag;
  public:
    template<typename FunctionType>
    InterruptibleThread(FunctionType f) {
        std::promise<InterruptFlag*> p;
        _internal_thread = std::thread([f, &p]{
            p.set_value(&this_thread_interrupt_flag);
            f();
        });
        _flag = p.get_future().get();
    }
    void join();
    void detach();
    bool joinable() const;
    void Interrupt() {
        if (_flag) {
          _flag->Set();
        }
    }
};
