#include "../include/memory/memory_pool.h"
#include <iostream>
#include <thread>
#include <vector>
#include <cstring>

// 测试1: 基础分配与释放
void test_basic_allocation() {
    std::cout << "=== Running basic allocation test ===\n";
    MemoryPool pool;

    // 分配内存并写入数据
    int* num = static_cast<int*>(pool.allocate(sizeof(int)));
    *num = 42;
    std::cout << "Allocated int: " << *num << "\n";

    // 释放内存
    pool.deallocate(num);
    std::cout << "Deallocated int\n";

    // 测试字符串
    const char* str = "Hello, MemoryPool!";
    char* buf = static_cast<char*>(pool.allocate(strlen(str) + 1));
    strcpy(buf, str);
    std::cout << "Allocated string: " << buf << "\n";
    pool.deallocate(buf);

    std::cout << "Basic test passed!\n\n";
}

// 测试2: 内存复用验证
void test_memory_reuse() {
    std::cout << "=== Running memory reuse test ===\n";
    MemoryPool pool;

    // 第一次分配
    void* ptr1 = pool.allocate(100);
    std::cout << "First allocation: " << ptr1 << "\n";
    pool.deallocate(ptr1);

    // 第二次分配（应复用同一块内存）
    void* ptr2 = pool.allocate(100);
    std::cout << "Second allocation: " << ptr2 << "\n";
    pool.deallocate(ptr2);

    assert(ptr1 == ptr2 && "Memory not reused!");
    std::cout << "Memory reuse test passed!\n\n";
}

// 测试3: 多线程安全性
void test_thread_safety() {
    std::cout << "=== Running thread safety test ===\n";
    MemoryPool pool;
    constexpr int kThreads = 4;
    constexpr int kAllocsPerThread = 1000;
    std::vector<std::thread> threads;

    // 工作函数：频繁分配/释放内存
    auto worker = [&pool]() {
        for (int i = 0; i < kAllocsPerThread; ++i) {
            int* ptr = static_cast<int*>(pool.allocate(sizeof(int)));
            *ptr = i;
            pool.deallocate(ptr);
        }
    };

    // 启动多线程
    for (int i = 0; i < kThreads; ++i) {
        threads.emplace_back(worker);
    }

    // 等待所有线程完成
    for (auto& t : threads) {
        t.join();
    }

    std::cout << "Thread safety test passed!\n\n";
}

// 测试4: 异常处理（无效指针释放）
void test_exception_handling() {
    std::cout << "=== Running exception handling test ===\n";
    MemoryPool pool;
    int dummy = 42;

    try {
        pool.deallocate(&dummy);  // 释放非池内指针
        assert(false && "Exception not thrown!");
    } catch (const std::runtime_error& e) {
        std::cout << "Caught expected exception: " << e.what() << "\n";
    }

    std::cout << "Exception test passed!\n\n";
}

int main() {
    test_basic_allocation();
    test_memory_reuse();
    test_thread_safety();
    test_exception_handling();

    std::cout << "All MemoryPool tests passed successfully!\n";
    return 0;
}