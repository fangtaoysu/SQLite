#include <functional>
#include <future>
#include <iostream>
#include <vector>

#include "../include/thread_pool/thread_pool.h"


void Count() {
    for (unsigned i = 0; i < 10; ++i) {
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
    ThreadPool thread_pool;
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