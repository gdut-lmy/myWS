//
// Created by lmy on 2022/7/7.
//

#include <fiber.h>
#include <atomic>
#include <macro.h>
#include "config.h"


namespace my {

    static Logger::ptr g_logger = MY_LOG_NAME("system");

    static std::atomic<uint64_t> s_fiber_id{0};
    static std::atomic<uint64_t> s_fiber_count{0};

    //当前协程指针
    static thread_local Fiber *t_fiber = nullptr;
    //当前线程的主协程指针
    static thread_local Fiber::ptr t_threadFiber = nullptr;

    static ConfigVar<uint32_t>::ptr g_fiber_stack_size =
            Config::Lookup<uint32_t>("fiber.stack_size", 128 * 1024, "Fiber stack size");

    class MallocStackAllocator {
    public:
        static void *Alloc(size_t size) {
            return malloc(size);
        }

        static void Dealloc(void *vp, size_t size) {
            free(vp);
        }
    };

    using StackAllocator = MallocStackAllocator;

    Fiber::Fiber() {
        m_state = EXEC;
        SetThis(this);

        if (getcontext(&m_ctx)) {
            MY_ASSERT2(false, "getcontext failed");
        }
        ++s_fiber_count;
    }

    void Fiber::SetThis(Fiber *f) {
        t_fiber = f;

    }

    Fiber::Fiber(std::function<void()> cb, size_t stacksize) : m_id(++s_fiber_id), m_cb(cb) {
        ++s_fiber_count;
        m_stacksize = stacksize ? stacksize : g_fiber_stack_size->getValue();

        m_stack = StackAllocator::Alloc(m_stacksize);
        if (getcontext(&m_ctx)) {
            MY_ASSERT2(false, "getcontext failed");
        }
        m_ctx.uc_link = nullptr;
        m_ctx.uc_stack.ss_sp = m_stack;
        m_ctx.uc_stack.ss_size = m_stacksize;

        makecontext(&m_ctx, &Fiber::MainFunc, 0);

    }

    Fiber::~Fiber() {
        --s_fiber_count;
        if (m_stack) {
            MY_ASSERT(m_state == TERM || m_state == INIT);
            StackAllocator::Dealloc(m_stack, m_stacksize);
        } else {
            MY_ASSERT(!m_cb);
            MY_ASSERT(m_state == EXEC);
            Fiber *cur = t_fiber;
            if (cur == this) {
                SetThis(nullptr);
            }
        }
    }


    void my::Fiber::reset(std::function<void()> cb) {

        MY_ASSERT(m_stack);
        MY_ASSERT(m_state == TERM || m_state == EXCEPT || m_state == INIT);
        m_cb = cb;
        if (getcontext(&m_ctx)) {
            MY_ASSERT2(false, "getcontext failed");
        }
        m_ctx.uc_link = nullptr;
        m_ctx.uc_stack.ss_sp = m_stack;
        m_ctx.uc_stack.ss_size = m_stacksize;

        makecontext(&m_ctx, &Fiber::MainFunc, 0);
        m_state = INIT;
    }

    //从主线程切换到当前协程
    void my::Fiber::swapIn() {

        SetThis(this);
        MY_ASSERT(m_state != EXEC);
        if (swapcontext(&t_threadFiber->m_ctx, &m_ctx)) {
            MY_ASSERT2(false, "swapcontext failed");
        }
    }

    //从当前协程切换到主协程
    void my::Fiber::swapOut() {
        SetThis(t_threadFiber.get());
        if (swapcontext(&m_ctx, &t_threadFiber->m_ctx)) {
            MY_ASSERT2(false, "system error: swapcontext() fail");
        }
    }

    my::Fiber::ptr my::Fiber::GetThis() {
        if (t_fiber) {
            return t_fiber->shared_from_this();
        }
        Fiber::ptr main_fiber(new Fiber);
        MY_ASSERT(t_fiber == main_fiber.get());
        t_threadFiber = main_fiber;
        return t_fiber->shared_from_this();
    }

    void my::Fiber::YieldToReady() {
        Fiber::ptr cur = GetThis();
        cur->m_state = READY;
        cur->swapOut();
    }

    void my::Fiber::YieldToHold() {
        Fiber::ptr cur = GetThis();
        cur->m_state = HOLD;
        cur->swapOut();
    }

    uint64_t my::Fiber::TotalFibers() {
        return s_fiber_count;
    }

    void my::Fiber::MainFunc() {
        Fiber::ptr cur = GetThis();
        MY_ASSERT(cur);
        try {
            cur->m_cb();
            cur->m_cb = nullptr;
            cur->m_state = TERM;
        } catch (std::exception &ex) {
            cur->m_state = EXCEPT;
            MY_LOG_ERROR(g_logger) << "Fiber Except: " << ex.what();
        } catch (...) {
            cur->m_state = EXCEPT;
            MY_LOG_ERROR(g_logger) << "Fiber Except: ";
        }
        auto ptr = cur.get();
        cur = nullptr;
        ptr->swapOut();
    }

    uint64_t Fiber::GetFiberId() {
        if (t_fiber) {
            return t_fiber->getId();
        }
        return 0;
    }

    void Fiber::EnableFiber() {
        if (!t_fiber) {
            Fiber::ptr main_fiber(new Fiber);
            MY_ASSERT(t_fiber == main_fiber.get());
            t_threadFiber = main_fiber;
        }
    }
}