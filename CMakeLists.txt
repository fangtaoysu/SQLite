cmake_minimum_required(VERSION 3.10)
project(ThreadPoolDemo)

# 启用 C++17 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 查找线程库（必需）
find_package(Threads REQUIRED)

# 添加线程池库
add_library(thread_pool
    src/thread_pool/thread_pool.cpp
    # 添加其他需要的源文件
)

# 添加内存池库（新增）
add_library(memory_pool
    src/memory/memory_pool.cpp  # 确保这个文件存在
)

# 添加orm
add_library(orm
    src/orm/database.cpp
)

# 设置线程池头文件目录
target_include_directories(thread_pool PUBLIC 
    ${CMAKE_SOURCE_DIR}/include/thread_pool/
)

# 设置内存池头文件目录（修正：必须在 add_library 之后）
target_include_directories(memory_pool PUBLIC 
    ${CMAKE_SOURCE_DIR}/include/memory/
)

# 设置orm头文件目录
target_include_directories(orm PUBLIC
    ${CMAKE_SOURCE_DIR}/include/orm
)

# 链接系统线程库
target_link_libraries(thread_pool 
    Threads::Threads
)

# 构建线程池测试
add_executable(thread_pool_test
    tests/thread_pool_test.cpp
)
target_link_libraries(thread_pool_test
    thread_pool
)

# 构建内存池测试（修正：链接 memory_pool）
add_executable(memory_pool_test
    tests/memory_pool_test.cpp
)
target_link_libraries(memory_pool_test
    memory_pool
    Threads::Threads  # 如果测试需要多线程
)

# 构建orm测试
add_executable(orm_test
tests/orm_test.cpp
)

target_link_libraries(orm_test
    orm
)