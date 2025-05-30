#ifndef MEMORY_POOL_H
#define MEMORY_POOL_H


#include <vector>
#include <mutex>
#include <cstddef>
#include <cassert>


class MemoryPool {
  private:
    struct _Block {
        void* memory; // 通用指针：兼容所有指针类型
        size_t size;
        bool is_free;
    };
    std::vector<_Block> _blocks;
    std::mutex _mutex;

  public:
    // 分配任意大小内存
    void* allocate(size_t size);
    void deallocate(void* ptr);

    ~MemoryPool();
};


#endif