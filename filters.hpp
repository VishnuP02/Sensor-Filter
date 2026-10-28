#ifndef FILTERS_HPP
#define FILTERS_HPP

#include <algorithm>
#include <array>
#include <cstddef>

namespace sf {

// Fixed-window sample store shared by the filters. No dynamic allocation.
template <std::size_t N>
class Window {
    static_assert(N > 0, "window size must be greater than zero");

public:
    Window() : samples_(), next_(0), count_(0) {}
    void push(double sample) {
        samples_[next_] = sample;
        next_ = (next_ + 1) % N;
        if (count_ < N) {
            ++count_;
        }
    }
    void reset() {
        next_ = 0;
        count_ = 0;
    }
    std::size_t count() const { return count_; }
    // Copies the valid samples (oldest-first order is not guaranteed) into out; returns how many.
    std::size_t copy_to(std::array<double, N>& out) const {
        for (std::size_t i = 0; i < count_; ++i) {
            out[i] = samples_[i];
        }
        return count_;
    }

private:
    std::array<double, N> samples_;
    std::size_t next_;
    std::size_t count_;
};

// Mean of the most recent N samples (fewer if fewer have been received).
template <std::size_t N>
class MovingAverage {
public:
    void push(double sample) { window_.push(sample); }
    void reset() { window_.reset(); }
    std::size_t count() const { return window_.count(); }
    // Returns false, leaving out unchanged, when no samples have been received.
    bool value(double& out) const {
        std::array<double, N> tmp{};
        const std::size_t n = window_.copy_to(tmp);
        if (n == 0) {
            return false;
        }
        double sum = 0.0;
        for (std::size_t i = 0; i < n; ++i) {
            sum += tmp[i];
        }
        out = sum / static_cast<double>(n);
        return true;
    }

private:
    Window<N> window_;
};

// Median of the most recent N samples. For an even number of received samples the
// lower of the two middle values is returned, so the result is always a real sample.
template <std::size_t N>
class MedianFilter {
    static_assert(N % 2 == 1, "median window size must be odd");

public:
    void push(double sample) { window_.push(sample); }
    void reset() { window_.reset(); }
    std::size_t count() const { return window_.count(); }
    // Returns false, leaving out unchanged, when no samples have been received.
    bool value(double& out) const {
        std::array<double, N> tmp{};
        const std::size_t n = window_.copy_to(tmp);
        if (n == 0) {
            return false;
        }
        std::sort(tmp.begin(), tmp.begin() + static_cast<std::ptrdiff_t>(n));
        out = tmp[(n - 1) / 2];
        return true;
    }

private:
    Window<N> window_;
};

}  // namespace sf

#endif
