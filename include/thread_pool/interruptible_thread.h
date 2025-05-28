#ifndef INTERRUPTIBLE_THREAD_H
#define INTERRUPTIBLE_THREAD_H


class InterruptibleThread {
  public:
    template<typename FunctionType>
    InterruptibleThread(FunctionType f);
    void join();
    void detach();
    bool joinable() const;
    void Interrupt();
};


#endif