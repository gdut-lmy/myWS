//
// Created by zavier on 2021/12/5.
//

#ifndef MYWS_NONCOPYABLE_H
#define MYWS_NONCOPYABLE_H

class Noncopyable{
public:
    Noncopyable() = default;
    ~Noncopyable() = default;
    Noncopyable(const Noncopyable& noncopyable) = delete;
    Noncopyable& operator=(const Noncopyable& noncopyable) = delete;
};
#endif //MYWS_NONCOPYABLE_H
