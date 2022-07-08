//
// Created by lmy on 2022/7/7.
//

#ifndef MYWS_FIBER_H
#define MYWS_FIBER_H

#include <memory>
#include <functional>
#include <ucontext.h>
#include <threadPool.h>

namespace my {
    class Fiber : public std::enable_shared_from_this<Fiber> {
    public:
        using ptr = std::shared_ptr<Fiber>;

        enum State {
            INIT,
            HOLD,
            EXEC,
            TERM,
            READY,
            EXCEPT
        };
    private:
        Fiber();

    public:
        Fiber(std::function<void()> cb, size_t stacksize = 0);

        ~Fiber();

        //重置协程函数，并重置状态
        void reset(std::function<void()> cb);

        //切换到当前协程执行
        void swapIn();

        //切换到后台执行
        void swapOut();

        uint64_t getId() const { return m_id; }

    public:

        static void SetThis(Fiber *f);

        //在当前线程启用协程
        static void EnableFiber();

        //返回当前协程
        static Fiber::ptr GetThis();

        //获取当前协程Id
        static uint64_t GetFiberId();

        //协程切换到后台，并设置为Ready状态
        static void YieldToReady();

        //协程切换到后台，并设置为Hold状态
        static void YieldToHold();

        //总协程数
        static uint64_t TotalFibers();

        static void MainFunc();

    private:
        uint64_t m_id;//协程ID
        uint32_t m_stacksize;
        State m_state = INIT;

        ucontext_t m_ctx;
        void *m_stack = nullptr;
        std::function<void()> m_cb;

    };
}

#endif //MYWS_FIBER_H
