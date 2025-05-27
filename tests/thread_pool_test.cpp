#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <type_traits>
#include <vector>

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

class FunctionWarpper {
    // 存储不同类型的任务（lambda/函数指针/成员函数等）
    struct _ImplementBase {
        virtual void call()=0;
        virtual ~_ImplementBase() {}
    };
    // 对象切片：多态赋值/传值时，派生对象被截断为基类对象；赋值时编译器会丢弃派生类特有的东西
    // _ImplementBase impl = _ImplementType<MyFunctor>(); 
    // 使用指针避免对象切片
    std::unique_ptr<_ImplementBase> _implement;
    template<typename F>
    struct _ImplementType : _ImplementBase {
        F f;
        // && 通用引用，可以匹配左值/右值
        _ImplementType(F&& f_) : f(std::move(f_)) {}
        void call() {
            f();
        }
    };
  public:
    template<typename F>
    FunctionWarpper(F&& f) : _implement(new _ImplementType<F>(std::move(f))) {}
    void operator() () {
        _implement->call();
    }
    FunctionWarpper() = default;
    // 移动操作允许任务对象放入容器vector
    // 移动构造函数
    FunctionWarpper(FunctionWarpper&& other) : _implement(std::move(other._implement)) {}
    // 移动操作运算符
    FunctionWarpper& operator=(FunctionWarpper&& other) {
        _implement = std::move(other._implement);
        return *this;
    }
    // 拷贝操作可能引发资源竞争
    FunctionWarpper(const FunctionWarpper&)=delete;
    FunctionWarpper(FunctionWarpper&)=delete;
    FunctionWarpper& operator=(const FunctionWarpper&)=delete;
};

class ThreadPoll {
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
    ThreadPoll() : _done(false), _joiner(_threads) {
        unsigned const thread_count = std::thread::hardware_concurrency();
        std::cout << "max hardware threads number: " << thread_count << std::endl;
        try {
            for (unsigned i = 0; i < thread_count; ++i) {
                _threads.push_back(std::thread(&ThreadPoll::WorkerThread, this));
            }
        } catch (...) {
            _done = true;
            throw;
        }
    }
    
    ~ThreadPoll() {
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

void Count() {
    for (unsigned i = 0; i < 100; ++i) {
        std::cout << i << std::endl;
    }
}

int SequeSum(int start, int end) {
    int res = 0;
    for (unsigned int i = start; i < end; ++i) {
        res += i;
    }
    return res;
}

int Sum() {
    int res = 0;
    for (unsigned int i = 1; i < 10; ++i) {
        res += i;
    }
    return res;
}

int main() {
    std::cout << "ok" << std::endl;
    ThreadPoll thread_pool;
    // 无参数无返回值函数调用
    for(unsigned i = 0; i < 3; ++i) {
        thread_pool.submit(&Count);
    }
    int total = 0;
    int res = 0;
    std::vector<std::future<int>> futures1, futures2;
    for (unsigned i = 1; i <= 100; i += 10) {
        // 有参数有返回值函数调用
        futures1.push_back(thread_pool.submit(std::bind(SequeSum, i, i+10)));
        // 无参数又返回值函数调用
        futures2.push_back(thread_pool.submit(&Sum));
        // 注意：此时无法调用重载函数，因为编译器无法直接推断函数示例的类型
    }
    // 最后一次性取出结果
    for (auto& f : futures1) {
        total += f.get();
    }
    for (auto& f : futures2) {
        res += f.get();
    }
    std::cout << "total: " + std::to_string(total) << std::endl;
    std::cout << "res: " + std::to_string(res) << std::endl;
    return 0;
}