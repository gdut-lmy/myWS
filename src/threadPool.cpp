//
// Created by lmy on 2022/7/5.
//

#include "threadPool.h"


namespace my {
    static thread_local threadPool* t_thread = nullptr;
    static thread_local std::string t_thread_name = "UNKNOWN";
    static Logger::ptr g_logger = MY_LOG_NAME("system");


    const std::string &threadPool::GetName() {
        //static std::string main = "MainThread";
        if(t_thread){
            return t_thread_name;
        }
        SetName("main");
        return t_thread_name;
    }

    void threadPool::SetName(const std::string &name) {
        if(t_thread){
            t_thread->m_name = name;
        }
        t_thread_name = name;
    }

}