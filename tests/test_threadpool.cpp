//
// Created by lmy on 2022/6/20.
//

#include <iostream>
#include "threadPool.h"
#include<future>
#include<functional>
#include<random>
#include <algorithm>

std::random_device rd;
std::mt19937 mt(rd());
std::uniform_int_distribution<int> dist(-1000, 1000);
auto rnd = std::bind(dist, mt);

void simulate_hard_computation() {
    std::this_thread::sleep_for(std::chrono::milliseconds(2000 + rnd()));
}

void multiply(const int a, const int b) {
    simulate_hard_computation();
    const int res = a * b;
    std::cout << a << " * " << b << " = " << res << std::endl;
}

void multiply_output(int &out, const int a, const int b) {
    simulate_hard_computation();
    out = a * b;
    std::cout << a << " * " << b << " = " << out << std::endl;
}

int multiply_return(const int a, const int b) {
    simulate_hard_computation();
    const int res = a * b;
    std::cout << a << " * " << b << " = " << res << std::endl;
    return res;
}


int main() {


    my::threadPool pool(4);

    for (int i = 0; i < 8; i++) {
        pool.submit(multiply_return, i, i + 10);
    }


    return 0;
}