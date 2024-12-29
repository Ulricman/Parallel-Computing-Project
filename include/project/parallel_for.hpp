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

template <typename Value, typename Func>
void parallel_for(const blocked_range<Value>& range, const Func& func,
                  size_t num_threads) {
  size_t max_num_thread =
      (range.size() + range.grainsize() - 1) / range.grainsize();
  num_threads = std::min(num_threads, max_num_thread);
  if (num_threads > 1) {
    std::vector<blocked_range<Value>> blocked_ranges = range.split(num_threads);
    std::vector<std::thread> threads;

    for (const auto& _range : blocked_ranges) {
      threads.push_back(std::thread([&]() { parallel_for(_range, func, 1); }));
    }

    for (auto& thread : threads) {
      thread.join();
    }
  } else {
    for (auto it = range.begin(); it != range.end(); ++it) {
      func(it);
    }
  }
}
}  // namespace project

#endif