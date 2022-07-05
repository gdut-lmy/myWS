//
// Created by lmy on 2022/6/10.
//

#ifndef MYWS_threadPool_H
#define MYWS_threadPool_H

#include<thread>
#include<condition_variable>
#include<mutex>
#include<vector>
#include<queue>
#include<future>
#include <functional>
#include "log.h"

namespace my {

    class threadPool {

    public:
        explicit threadPool(size_t threadNumber) : m_stop(false) {
            for (size_t i = 0; i < threadNumber; ++i) {
                m_thread.emplace_back(
                        [this]() {
                            for (;;) {
                                std::function<void()> task;
                                {
                                    std::unique_lock<std::mutex> lk(m_mutex);
                                    m_cv.wait(lk, [this]() { return m_stop || !tasks.empty(); });
                                    if (m_stop && tasks.empty()) return;
                                    task = std::move(tasks.front());
                                    tasks.pop();
                                }
                                task();
                            }
                        }
                );
            }
        }

        threadPool(const threadPool &) = delete;

        threadPool(threadPool &&) = delete;

        threadPool &operator=(const threadPool &) = delete;

        threadPool &operator=(threadPool &&) = delete;

        ~threadPool() {
            {
                std::unique_lock<std::mutex> lk(m_mutex);
                m_stop = true;
            }
            m_cv.notify_all();
            for (auto &threads: m_thread) {
                threads.join();
            }
        }

        template<typename F, typename... Args>
        auto submit(F &&f, Args &&... args) -> std::future<decltype(f(args...))> {
            auto taskPtr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(
                    std::bind(std::forward<F>(f), std::forward<Args>(args)...));
            {
                std::unique_lock<std::mutex> lk(m_mutex);
                if (m_stop) throw std::runtime_error("submit on stopped threadPool");
                tasks.emplace([taskPtr]() { (*taskPtr)(); });
            }
            m_cv.notify_one();
            return taskPtr->get_future();

        }

        /**
         * @brief 获取当前的线程名称
         */
        static const std::string &GetName();

        static void SetName(const std::string& name);

    private:
        bool m_stop;
        pid_t m_id = 0;
        std::string m_name;
        std::vector<std::thread> m_thread;
        std::queue<std::function<void()>> tasks;
        std::mutex m_mutex;
        std::condition_variable m_cv;
    };
}
#endif //MYWS_threadPool_H
