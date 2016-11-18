# timeit::timeit

Inspired by `timeit` from Python, this class provides a simple way to time small bits of C/C++ code.
 
Example:

    #include <cmath>
    #include "timeit.h"
    
    int main() {
        timeit::timeit_out<>{}([]() { std::log(2); });
        return 0;
    }

Output:

    1000000 loops, best of 3: 0.101597 usec per loop
