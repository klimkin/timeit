//
// Created by Alexey Klimkin on 11/26/16.
//

#include <map>
#include "timeit.h"

std::map<int, int> std_map;

void std_map_emplace() {
    std_map.clear();
    for (int i = 0; i < 50; ++i) {
        std_map.emplace(i, i);
    }
}

int main() {
    timeit::timeit_out<>{}(std_map_emplace);
    return 0;
}