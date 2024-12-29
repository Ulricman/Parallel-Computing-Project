#ifndef __PROJECT_PARALLEL_FOR_HPP__
#define __PROJECT_PARALLEL_FOR_HPP__

#include <atomic>
#include <iostream>
#include <thread>

#include "project/blocked_range.hpp"

namespace project {
template <typename Value, typename Func>
void parallel_for(const blocked_range<Value>& range, const Func& func) {
  if (range.is_divisible()) {
    blocked_range<Value> left_range = range;
    blocked_range<Value> right_range = left_range.split();
    std::thread new_worker =
        std::thread([&]() { parallel_for(right_range, func); });
    parallel_for(left_range, func);
    new_worker.join();
  } else {
    for (auto it = range.begin(); it != range.end(); ++it) {
      func(it);
    }
  }
}
}  // namespace project

#endif