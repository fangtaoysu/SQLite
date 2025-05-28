cmake_minimum_required(VERSION 3.10)
project(ThreadPoolDemo)

# 启用 C++17 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 查找线程库（必需）
find_package(Threads REQUIRED)

# # 添加线程池库
add_library(thread_pool
    src/thread_pool/thread_pool.cpp
    # 添加其他需要的源文件
)

# 设置头文件目录
target_include_directories(thread_pool PUBLIC 
    ${CMAKE_SOURCE_DIR}/include/thread_pool/
)

# 链接系统线程库
target_link_libraries(thread_pool 
    Threads::Threads
)

# 执行
add_executable(thread_pool_test
    tests/thread_pool_test.cpp
)