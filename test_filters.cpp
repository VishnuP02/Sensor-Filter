// Requirements-based tests. Each check names the requirement (see README.md) it verifies.
#include <cmath>
#include <iostream>
#include "filters.hpp"

static int tests_run = 0;
static int tests_failed = 0;

#define CHECK(req, cond)                                                                   \
    do {                                                                                   \
        ++tests_run;                                                                       \
        if (!(cond)) {                                                                     \
            ++tests_failed;                                                                \
            std::cout << "FAIL [" << req << "] " << __FILE__ << ":" << __LINE__ << ": "    \
                      << #cond << "\n";                                                    \
        }                                                                                  \
    } while (0)

template <typename Filter>
static bool has_value(const Filter& f) {
    double v = 0.0;
    return f.value(v);
}

template <typename Filter>
static bool near(const Filter& f, double expected) {
    double v = 0.0;
    return f.value(v) && std::fabs(v - expected) < 1e-9;
}

static void test_empty() {
    sf::MovingAverage<3> avg;
    sf::MedianFilter<3> med;
    CHECK("SF-001", !has_value(avg));
    CHECK("SF-001", !has_value(med));
    CHECK("SF-001", avg.count() == 0 && med.count() == 0);
}

static void test_partial_window() {
    sf::MovingAverage<4> avg;
    avg.push(2.0);
    CHECK("SF-002", near(avg, 2.0));
    avg.push(4.0);
    CHECK("SF-002", near(avg, 3.0));
    CHECK("SF-002", avg.count() == 2);
}

static void test_window_slides() {
    sf::MovingAverage<3> avg;
    avg.push(1.0);
    avg.push(2.0);
    avg.push(3.0);
    CHECK("SF-003", near(avg, 2.0));
    avg.push(10.0);  // drops 1.0 -> window is 2, 3, 10
    CHECK("SF-003", near(avg, 5.0));
    avg.push(10.0);  // drops 2.0 -> window is 3, 10, 10
    CHECK("SF-003", near(avg, 23.0 / 3.0));
    CHECK("SF-003", avg.count() == 3);
}

static void test_median_values() {
    sf::MedianFilter<5> med;
    med.push(9.0);
    CHECK("SF-004", near(med, 9.0));
    med.push(1.0);  // two samples -> lower middle
    CHECK("SF-004", near(med, 1.0));
    med.push(5.0);
    CHECK("SF-004", near(med, 5.0));
    med.push(7.0);
    med.push(3.0);  // full window: 1, 3, 5, 7, 9
    CHECK("SF-004", near(med, 5.0));
}

static void test_median_rejects_spike() {
    sf::MedianFilter<5> med;
    sf::MovingAverage<5> avg;
    const double stream[5] = {10.0, 10.1, 9000.0, 9.9, 10.0};  // one sensor glitch
    for (int i = 0; i < 5; ++i) {
        const double s = stream[i];
        med.push(s);
        avg.push(s);
    }
    CHECK("SF-005", near(med, 10.0));
    {
        double mean = 0.0;
        avg.value(mean);
        CHECK("SF-005", mean > 1000.0);
    }  // the mean is pulled off by the spike
}

static void test_median_window_slides() {
    sf::MedianFilter<3> med;
    const double values[5] = {1.0, 2.0, 3.0, 100.0, 101.0};
    for (int i = 0; i < 5; ++i) {
        med.push(values[i]);
    }
    CHECK("SF-003", near(med, 100.0));  // window is 3, 100, 101
}

static void test_reset() {
    sf::MovingAverage<3> avg;
    sf::MedianFilter<3> med;
    avg.push(5.0);
    med.push(5.0);
    avg.reset();
    med.reset();
    CHECK("SF-006", !has_value(avg) && !has_value(med));
    avg.push(8.0);
    med.push(8.0);
    CHECK("SF-006", near(avg, 8.0) && near(med, 8.0));
}


// Exercise every window size used in this project so each instantiation is verified.
template <std::size_t N>
static void test_average_for_size() {
    sf::MovingAverage<N> avg;
    CHECK("SF-001", !has_value(avg));
    for (std::size_t i = 0; i < N + 2; ++i) {
        avg.push(4.0);
    }
    CHECK("SF-003", avg.count() == N);  // never exceeds the window size
    CHECK("SF-003", near(avg, 4.0));
}

template <std::size_t N>
static void test_median_for_size() {
    sf::MedianFilter<N> med;
    CHECK("SF-001", !has_value(med));
    for (std::size_t i = 0; i < N + 2; ++i) {
        med.push(6.0);
    }
    CHECK("SF-003", med.count() == N);
    CHECK("SF-003", near(med, 6.0));
}

int main() {
    test_empty();
    test_partial_window();
    test_window_slides();
    test_median_values();
    test_median_rejects_spike();
    test_median_window_slides();
    test_reset();
    test_average_for_size<1>();
    test_average_for_size<3>();
    test_average_for_size<4>();
    test_average_for_size<5>();
    test_median_for_size<1>();
    test_median_for_size<3>();
    test_median_for_size<5>();
    std::cout << tests_run << " checks, " << tests_failed << " failed\n";
    return tests_failed == 0 ? 0 : 1;
}
