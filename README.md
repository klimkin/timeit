# timeit::timeit

Inspired by [timeit](https://docs.python.org/3.0/library/timeit.html) Python module,
this class provides a simple way to time small bits of C/C++ code.
 
Example:

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

Output:

    1000000 loops, best of 3: 1.5497 usec per loop
