//
// Created by Alexey Klimkin on 11/17/16.
//

#include <gmock/gmock.h>
#include "timeit.h"

/**
 * The clock returns exponentially increasing time measures: 1 2 4 etc
 */
struct MockExponentialClock {
    using duration = std::chrono::duration<double>;
    using time_point = std::chrono::time_point<MockExponentialClock>;

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

TEST_F(TimeitTest, CallableWithoutArguments) {
    MockCallable f;
    EXPECT_CALL(f, Call0());
    auto t = timeit::timeit<MockExponentialClock::duration, MockExponentialClock>{1}(f).count();
    // (8-4) - (2-1) = 3
    EXPECT_EQ(t, 3);
}

TEST_F(TimeitTest, CallableWithArgument) {
    MockCallable f;
    EXPECT_CALL(f, Call1(2));
    auto t = timeit::timeit<MockExponentialClock::duration, MockExponentialClock>{1}(f, 2).count();
    // (8-4) - (2-1) = 3
    EXPECT_EQ(t, 3);
}

TEST_F(TimeitTest, MultipleLoops) {
    MockCallable f;
    EXPECT_CALL(f, Call0()).Times(10);
    auto t = timeit::timeit<MockExponentialClock::duration, MockExponentialClock>{10}(f).count();
    EXPECT_EQ(t, 3);
}

TEST_F(TimeitTest, MultipleIterations) {
    MockCallable f;
    EXPECT_CALL(f, Call0()).Times(2);
    auto t = timeit::repeat<MockExponentialClock::duration, MockExponentialClock>{2, 1}(f);
    // (8-4) - (2-1) = 3
    // (128-64) - (32-16) = 48
    std::vector<MockExponentialClock::duration> expected{MockExponentialClock::duration{3},
                                                         MockExponentialClock::duration{48}};
    EXPECT_EQ(t, expected);
}

TEST_F(TimeitTest, BestTime) {
    MockCallable f;
    EXPECT_CALL(f, Call0()).Times(2);
    testing::internal::CaptureStdout();
    auto t = timeit::timeit_out<MockExponentialClock::duration, MockExponentialClock>{2, 1}(f).count();
    testing::internal::GetCapturedStdout();
    EXPECT_EQ(t, 3);
}

TEST_F(TimeitTest, Output) {
    testing::internal::CaptureStdout();
    // the minimum will be on the first iteration: ((8-4)-(2-1))/2=1.5
    timeit::timeit_out<MockExponentialClock::duration, MockExponentialClock>{3, 2}([]() {});
    auto output = testing::internal::GetCapturedStdout();
    EXPECT_EQ(output, "2 loops, best of 3: 1.5e+06 usec per loop\n");
}

TEST_F(TimeitTest, CalibrateNumberOfLoops) {
    // Calls to timer return 1 2 4 8 ...
    // Loop calibration:
    //   empty loop: 2-1=1
    //   10 loops:   (8-4-1)/10 = 0.3
    // Result:
    //   empty loop: 32-16=16
    //   10 loops:   (128-64-16)/10=4.8
    MockCallable f;
    EXPECT_CALL(f, Call0()).Times(20);
    testing::internal::CaptureStdout();
    auto t = timeit::timeit_out<MockExponentialClock::duration, MockExponentialClock>{1}(f).count();
    testing::internal::GetCapturedStdout();
    EXPECT_EQ(t, 4.8);
}