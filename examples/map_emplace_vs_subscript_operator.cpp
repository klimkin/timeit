//
// Created by Alexey Klimkin on 11/17/16.
//

#include <map>
#include "timeit.h"

std::map<int, int> std_map;

void std_map_emplace() {
    for (int i = 0; i < 50; ++i) {
        std_map.emplace(i, i);
    }
    std_map.clear();
}

void std_map_array_subscript_operator() {
    for (int i = 0; i < 50; ++i) {
        std_map[i] = i;
    }
    std_map.clear();
}

int main() {
    std::cout << "std::map emplace:" << std::endl;
    timeit::timeit_out<>{}(std_map_emplace);
    std::cout << "std::map operator[]:" << std::endl;
    timeit::timeit_out<>{}(std_map_array_subscript_operator);
    return 0;
}