#include "../../include/memory/memory_pool.h"

#include <stdexcept>

void MemoryPool::deallocate(void* ptr) {
    std::lock_guard<std::mutex> lock(_mutex);
    for (auto& block : _blocks) {
        if (block.memory == ptr) {
            block.is_free = true;
            return;
        }
    }
    throw std::runtime_error("Invalid pointer");
}

void* MemoryPool::allocate(size_t size) {
    std::lock_guard<std::mutex> lock(_mutex);
    // 优先复用空闲块
    for (auto& block : _blocks) {
        if (block.is_free) {
            block.is_free = false;
            return block.memory;
        }
    }
    // 无合适块则新建
    void* mem = ::operator new(size); // 访问全局命名空间
    _blocks.push_back({mem, size, false});
    return mem;
}

MemoryPool::~MemoryPool() {
    for (auto block : _blocks) {
        ::operator delete(block.memory);
    }
}