#ifndef FUNCTION_WARPPER_H
#define FUNCTION_WARPPER_H

#include <memory>


/**
 * @class FunctionWarpper
 * @brief 定义一个泛化的函数类型
 * @details 存储不同类型的任务（lambda/函数指针/成员函数等）
 */
class FunctionWarpper {
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


#endif