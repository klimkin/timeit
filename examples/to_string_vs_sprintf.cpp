//
// Created by Alexey Klimkin on 11/26/16.
//

#include <string>
#include <cstdio>
#include "timeit.h"

void std_to_string() {
    for (int i = 0; i < 50; ++i) {
        auto volatile x = std::to_string(i);
    }
}

void std_sprintf() {
    for (int i = 0; i < 50; ++i) {
        char buf[100];
        sprintf(buf, "%d", i);
        auto volatile x = std::string{buf};
    }
}

int main() {
    std::cout << "std::to_string:" << std::endl;
    timeit::timeit_out<>{}(std_to_string);
    std::cout << "std::sprintf:" << std::endl;
    timeit::timeit_out<>{}(std_sprintf);
    return 0;
}