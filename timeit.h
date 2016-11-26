//
// Created by Alexey Klimkin on 11/17/16.
//

#ifndef _TIMEIT_TIMEIT_H
#define _TIMEIT_TIMEIT_H

#include <algorithm>
#include <chrono>
#include <iostream>
#include <vector>

/**
 * Inspired by [timeit](https://docs.python.org/3.0/library/timeit.html) Python module,
 * this class provides a simple way to time small bits of C/C++ code.
 */
namespace timeit {
    using default_duration = std::chrono::duration<double, std::micro>;
    using default_clock = std::chrono::high_resolution_clock;

    enum {
        default_number = 1000000,
        default_repeat = 3
    };

    /**
         * Time number executions of the callable.
         *
         * The argument is the number of times through the loop, defaulting to one million.
         *
         * @tparam TimeT    time interval type
         * @tparam ClockT   clock type
         * @param number    number of times through the loop
         */
    template<typename TimeT = default_duration, class ClockT = default_clock>
    class timeit {
    public:
        explicit timeit(int number = default_number) : num_loops{number} {}

        template<typename F, typename... Args>
        TimeT operator()(F &&func, Args &&... args) const {
            auto start = ClockT::now();
            for (volatile int loop = 0; loop < num_loops; ++loop) {
                // DO NOTHING
            }
            auto loop_overhead = ClockT::now() - start;
            start = ClockT::now();
            for (volatile int loop = 0; loop < num_loops; ++loop) {
                std::forward<F>(func)(std::forward<Args>(args)...);
            }
            auto sum_time = ClockT::now() - start;
            return std::chrono::duration_cast<TimeT>(sum_time - loop_overhead);
        }

    protected:
        const int num_loops;    // number of times through the loop
    };

    /**
         * Call timeit() a few times.
         *
         * This is a convenience function that calls the timeit() repeatedly, returning a list of results.
         * The first argument specifies how many times to call timeit(), defaulting to 3.
         * The second argument specifies the number argument for timeit().

         * @tparam TimeT    time interval type
         * @tparam ClockT   clock type
         * @param repeat    how many times to call timeit()
         * @param number    number argument for timeit()
         */
    template<typename TimeT = default_duration, class ClockT = default_clock>
    class repeat {
    public:
        explicit repeat(int repeat = default_repeat, int number = default_number)
                : num_iterations{repeat}, num_loops{number} {}

        template<typename F, typename... Args>
        auto operator()(F &&func, Args &&... args) const {
            std::vector<TimeT> results;
            results.reserve(num_iterations);
            for (int iteration = 0; iteration < num_iterations; ++iteration) {
                results.emplace_back(
                        timeit<TimeT, ClockT>{num_loops}(std::forward<F>(func), std::forward<Args>(args)...));
            }
            return results;
        }

    protected:
        const int num_iterations;  // how many times to call timeit()
        const int num_loops;       // number of times through the loop
    };

    /**
     * Call timeit() a few times and return the best result.
     *
     * If number is zero, a suitable number of loops is calculated by trying
     * successive powers of 10 until the total time is at least 0.2 seconds.
     *
     * @tparam TimeT    time interval type
     * @tparam ClockT   clock type
     * @param repeat    repeat argument for repeat()
     * @param number    number argument for repeat()
     */
    template<typename TimeT = default_duration, class ClockT = default_clock>
    class timeit_out {
    public:
        explicit timeit_out(int repeat = default_repeat, int number = 0, bool verbose = false)
                : num_iterations{repeat}, num_loops{number}, verbose{verbose} {}

        template<typename F, typename... Args>
        TimeT operator()(F &&func, Args &&... args) const {
            int number = num_loops;
            if (number == 0) {
                number = 10;
                for (int i = 0; i < 10; ++i, number *= 10) {
                    auto x = timeit<TimeT, ClockT>{number}(std::forward<F>(func), std::forward<Args>(args)...);
                    double x_seconds = std::chrono::duration<typename TimeT::rep>{x}.count();
                    if (verbose)
                        std::cout << number << " loops -> " << x_seconds << " secs" << std::endl;
                    if (x_seconds >= 0.2)
                        break;
                }
            }
            auto results{repeat<TimeT, ClockT>{num_iterations, number}(
                    std::forward<F>(func), std::forward<Args>(args)...)};
            if (verbose) {
                std::cout << "raw times:";
                for (auto &r : results)
                    std::cout << " " << std::chrono::duration<typename TimeT::rep, std::micro>{r}.count();
                std::cout << std::endl;
            }
            auto best = *std::min_element(results.begin(), results.end()) / number;
            std::cout << number << " loops, best of " << num_iterations
                      << ": " << std::chrono::duration<typename TimeT::rep, std::micro>{best}.count()
                      << " usec per loop" << std::endl;
            return best;
        }

    protected:
        const int num_iterations;   // how many times to call timeit()
        const int num_loops;    // number of times through the loop
        bool verbose;
    };
}  // namespace timeit

#endif //_TIMEIT_H_