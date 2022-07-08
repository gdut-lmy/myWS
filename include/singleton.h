//
// Created by lmy on 2022/7/4.
//

#ifndef MY_SINGLETON_H
#define MY_SINGLETON_H

#include <memory>

namespace my {
    template<typename T>
    class Singleton {
    public:
        static T *GetInstance() {
            static T instance;
            return &instance;
        }

    };

    template<typename T>
    class SingletonPtr {
    public:
        static std::shared_ptr<T> GetInstance() {
            static std::shared_ptr<T> instance(std::make_shared<T>());
            return instance;
        }
    };

}
#endif //MY_SINGLETON_H
