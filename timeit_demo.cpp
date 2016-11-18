//
// Created by Alexey Klimkin on 11/17/16.
//

#include <cmath>
#include "timeit.h"

int main() {
    timeit::timeit_out<>{}([]() { std::log(2); });
    return 0;
}