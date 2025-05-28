#ifndef JOIN_THREADS_H
#define JOIN_THREADS_H

#include <vector>
#include <thread>

/**
 * @class JoinThreads
 * @brief RAII包装器：确保所有工作线程在析构时可以正确回收资源
 * @details RAII: 对象生命周期和资源生命周期绑定
 */
class JoinThreads {
    std::vector<std::thread>& _threads;
  public:
    explicit JoinThreads(std::vector<std::thread>& threads) : _threads(threads) {}
    ~JoinThreads() {
        for (auto& t : _threads) {
            if (t.joinable()) {
                // 线程池一般使用join，否则当主线程退出时，整个进程被终止，所有未完成的detach线程会被操作系统强制结束
                // join资源由主线程负责释放，detach子线程则自行释放。因此使用detach时析构函数无需任何操作
                t.join(); 
            }
        }
    }
};


#endif