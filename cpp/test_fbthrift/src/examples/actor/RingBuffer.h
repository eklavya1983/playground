#pragma once
#include <array>

template <class T, int sz>
struct RingBuffer {
    RingBuffer() {
        idx_ = 0;
        totalTraced_ = 0;
    }
    void push(const T &entry) {
        buffer_[idx_] = entry;
        idx_ = (idx_ + 1) % buffer_.size();
        totalTraced_++;
    }
 protected:
    std::array<T, sz> buffer_;
    int idx_;
    int totalTraced_;
};
