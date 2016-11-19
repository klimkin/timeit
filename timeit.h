//
// Created by Alexey Klimkin on 11/17/16.
//

#ifndef _TIMEIT_H_
#define _TIMEIT_H_

#include <chrono>
#include <utility>
#include <algorithm>
#include <iostream>

/**
 * Inspired by [timeit](https://docs.python.org/3.0/library/timeit.html) Python module,
 * this class provides a simple way to time small bits of C/C++ code.
 */
namespace timeit {

    using default_duration = std::chrono::duration<double, std::micro>;
    using default_clock = std::chrono::high_resolution_clock;

    /**
     * Measure execution time of a callable.
     *
     * For details see: http://stackoverflow.com/questions/2808398/easily-measure-elapsed-time
     *
     * @tparam TimeT    time interval type
     * @tparam ClockT   clock type
     */
    template<typename TimeT = default_duration, class ClockT = default_clock>
    struct measure {
        template<typename F, typename... Args>
        static TimeT duration(F &&func, Args &&... args) {
            auto start = ClockT::now();
            std::forward<F>(func)(std::forward<Args>(args)...);
            return std::chrono::duration_cast<TimeT>(ClockT::now() - start);
        }
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
        timeit(int number = 1000000) : num_loops{number} {}

        template<typename F, typename... Args>
        TimeT operator()(F &&func, Args &&... args) const {
            TimeT overall = {};
            for (int loop = 0; loop < num_loops; ++loop)
                overall += measure<TimeT, ClockT>::duration(std::forward<F>(func), std::forward<Args>(args)...);
            return overall / num_loops;
        }

    protected:
        const int num_loops;
    };

    /**
     * Call timeit() a few times.
     *
     * This is a convenience function that calls the timeit() repeatedly, returning a list of results.
     * The first argument specifies how many times to call timeit(), defaulting to 3.
     * The second argument specifies the number argument for timeit().

     * @tparam TimeT    time interval type
     * @tparam ClockT   clock type
     * @param execute   how many times to call timeit()
     * @param number    number argument for timeit()
     */
    template<typename TimeT = default_duration, class ClockT = default_clock>
    class repeat : protected timeit<TimeT, ClockT> {
    public:
        repeat(int execute = 3, int number = 1000000) : timeit<TimeT, ClockT>{number}, num_iterations{execute} {}

        template<typename F, typename... Args>
        auto operator()(F &&func, Args &&... args) const {
            std::vector<TimeT> results;
            results.reserve(num_iterations);
            for (int iteration = 0; iteration < num_iterations; ++iteration) {
                TimeT t = timeit<TimeT, ClockT>::operator()(std::forward<F>(func), std::forward<Args>(args)...);
                results.emplace_back(t);
            }
            return results;
        }

    protected:
        const int num_iterations;
    };

    /**
     * Call timeit() a few times, print, and return the best result.
     *
     * @tparam TimeT    time interval type
     * @tparam ClockT   clock type
     * @param number    number argument for repeat()
     * @param execute   execute argument for repeat()
     */
    template<typename TimeT = default_duration, class ClockT = default_clock>
    class timeit_out : protected repeat<TimeT, ClockT> {
    public:
        timeit_out(int number = 1000000, int execute = 3) : repeat<TimeT, ClockT>{execute, number} {}

        template<typename F, typename... Args>
        TimeT operator()(F &&func, Args &&... args) const {
            auto results = repeat<TimeT, ClockT>::operator()(std::forward<F>(func), std::forward<Args>(args)...);
            auto best = *std::min_element(results.begin(), results.end());
            std::cout << repeat<TimeT, ClockT>::num_loops << " loops, best of " << repeat<TimeT, ClockT>::num_iterations
                      << ": " << std::chrono::duration<typename TimeT::rep, std::micro>{best}.count()
                      << " usec per loop" << std::endl;
            return best;
        }
    };
}

#endif //_TIMEIT_H_