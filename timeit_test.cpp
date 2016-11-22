//
// Created by Alexey Klimkin on 11/17/16.
//

#include <gmock/gmock.h>
#include "timeit.h"

/**
 * The clock returns monotonically increasing time measures: 1 2 3 etc
 */
struct MockMonotonicClock {
    using duration = std::chrono::duration<double>;
    using time_point = std::chrono::time_point<MockMonotonicClock>;

    static time_point now() {
        tp += duration{1};
        return tp;
    }

    static void reset() {
        tp = {};
    }

    static time_point tp;
};

MockMonotonicClock::time_point MockMonotonicClock::tp = {};

/**
 * The clock returns exponentially increasing time measures: 1 2 4 etc
 */
struct MockExponentialClock {
    using duration = std::chrono::duration<double>;
    using time_point = std::chrono::time_point<MockMonotonicClock>;

    static time_point now() {
        if (step >= 0) {
            tp -= duration{1 << step};
        }
        step += 1;
        tp += duration{1 << step};
        return tp;
    }

    static void reset() {
        tp = {};
        step = -1;
    }

    static time_point tp;
    static int step;
};

MockExponentialClock::time_point MockExponentialClock::tp = {};
int MockExponentialClock::step = -1;

class TimeitTest : public ::testing::Test {
protected:
    void SetUp() override {
        MockMonotonicClock::reset();
        MockExponentialClock::reset();
    }
};


class SomeCallable {
public:
    virtual void operator()() = 0;

    virtual void operator()(int arg) = 0;
};

class MockCallable : public SomeCallable {
public:
    MOCK_METHOD0(Call0, void());

    void operator()() override { Call0(); }

    MOCK_METHOD1(Call1, void(int));

    void operator()(int arg) override { Call1(arg); }
};

TEST_F(TimeitTest, TestCallableWithoutArguments) {
    MockCallable f;
    EXPECT_CALL(f, Call0());
    auto t = timeit::timeit<MockMonotonicClock::duration, MockMonotonicClock>{1}(f).count();
    EXPECT_EQ(t, 1);
}

TEST_F(TimeitTest, TestCallableWithArgument) {
    MockCallable f;
    EXPECT_CALL(f, Call1(2));
    auto t = timeit::timeit<MockMonotonicClock::duration, MockMonotonicClock>{1}(f, 2).count();
    EXPECT_EQ(t, 1);
}

TEST_F(TimeitTest, TestMultipleLoops) {
    MockCallable f;
    EXPECT_CALL(f, Call0()).Times(10);
    auto t = timeit::timeit<MockMonotonicClock::duration, MockMonotonicClock>{10}(f).count();
    EXPECT_EQ(t, 0.1);
}

TEST_F(TimeitTest, TestRepeat) {
    MockCallable f;
    EXPECT_CALL(f, Call0()).Times(3);
    auto t = timeit::repeat<MockExponentialClock::duration, MockExponentialClock>{3, 1}(f);
    std::vector<MockExponentialClock::duration> expected{MockExponentialClock::duration{1},
                                                         MockExponentialClock::duration{4},
                                                         MockExponentialClock::duration{16}};
    EXPECT_EQ(t, expected);
}

TEST_F(TimeitTest, TestTimeBestTime) {
    MockCallable f;
    EXPECT_CALL(f, Call0()).Times(2);
    testing::internal::CaptureStdout();
    auto t = timeit::timeit_out<MockExponentialClock::duration, MockExponentialClock>{1, 2}(f).count();
    testing::internal::GetCapturedStdout();
    EXPECT_EQ(t, 1);
}

TEST_F(TimeitTest, TestOutput) {
    testing::internal::CaptureStdout();
    // the minimum will be on the first iteration: (2-1)/2=0.5
    timeit::timeit_out<MockExponentialClock::duration, MockExponentialClock>{2, 3}([]() {});
    auto output = testing::internal::GetCapturedStdout();
    EXPECT_EQ(output, "2 loops, best of 3: 500000 usec per loop\n");
}