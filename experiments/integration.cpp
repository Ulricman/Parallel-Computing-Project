#include <chrono>
#include <iostream>
#include <thread>
#include <tuple>
#include <utility>

#include "project/blocked_range.hpp"
#include "project/parallel_for.hpp"
#include "project/threadpool.hpp"
#include "project/utils.hpp"

inline double func(double x) {
  return 10.0 * x * x / (x + x * x * x + 0.2) + x;
}

std::vector<std::tuple<double, double, size_t>> split_limits(
    double lower_limit, double upper_limit, size_t num_rects,
    size_t num_intervals) {
  std::vector<std::tuple<double, double, size_t>> intervals;
  double interval_width = (upper_limit - lower_limit) / num_intervals;
  size_t num_rects_per_interval = num_rects / num_intervals;

  for (int interval_idx = 0; interval_idx < num_intervals - 1; ++interval_idx) {
    double left = lower_limit + interval_idx * interval_width;
    double right = lower_limit + (interval_idx + 1) * interval_width;
    intervals.emplace_back(left, right, num_rects_per_interval);
  }
  double left = lower_limit + (num_intervals - 1) * interval_width;
  double right = upper_limit;
  intervals.emplace_back(
      left, right, num_rects - (num_intervals - 1) * num_rects_per_interval);

  return intervals;
}

double integrate(double lower_limit, double upper_limit, size_t num_rects) {
  double area = 0.0;
  double width = (upper_limit - lower_limit) / num_rects;
  for (size_t rect_idx = 0; rect_idx != num_rects; ++rect_idx) {
    area += width * func(lower_limit + (rect_idx + 0.5)) * width;
  }
  return area;
}

int main() {
  double lower_limit = -10.0, upper_limit = 10.0;
  size_t num_rects = 10000000000;
  double width = (upper_limit - lower_limit) / num_rects;

  // Sequential integration.
  auto start = project::get_ts();
  double area = 0.0;
  for (size_t rect_idx = 0; rect_idx != num_rects; ++rect_idx) {
    area += width * func(lower_limit + (rect_idx + 0.5) * width);
  }
  auto end = project::get_ts();
  std::cout << "\033[32m      Sequential\033[0m: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(end -
                                                                     start)
            << std::endl;

  // Pre-split the integration interval.
  size_t num_intervals = 20;
  auto intervals =
      split_limits(lower_limit, upper_limit, num_rects, num_intervals);
  std::vector<double> area_per_interval(num_intervals, 0.);
  auto task = [&area_per_interval, &intervals](size_t interval_idx) {
    auto [left, right, num_rects] = intervals[interval_idx];
    area_per_interval[interval_idx] = integrate(left, right, num_rects);
  };

  // Parallel integration with threadpool.
  project::ThreadPool threadpool(100, 10);
  start = project::get_ts();
  for (size_t interval_idx = 0; interval_idx != num_intervals; ++interval_idx) {
    threadpool.submit([&task, interval_idx]() { task(interval_idx); });
  }
  threadpool.shutdown();
  area = std::accumulate(area_per_interval.cbegin(), area_per_interval.cend(),
                         0.0);
  end = project::get_ts();
  std::cout << "\033[32m      ThreadPool\033[0m: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(end -
                                                                     start)
            << std::endl;

  // Parallel integration with parallel_for().
  start = project::get_ts();
  project::parallel_for(
      project::blocked_range<size_t>(0, num_intervals, num_intervals / 10),
      task);
  area = std::accumulate(area_per_interval.cbegin(), area_per_interval.cend(),
                         0.0);
  end = project::get_ts();
  std::cout << "\033[32m     ParallelFor\033[0m: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(end -
                                                                     start)
            << std::endl;

  // Parallel integration with parallel_for() with fixed number of threads.
  start = project::get_ts();
  int num_threads = 10;
  project::parallel_for(project::blocked_range<size_t>(0, num_intervals), task,
                        num_threads);
  area = std::accumulate(area_per_interval.cbegin(), area_per_interval.cend(),
                         0.0);
  end = project::get_ts();
  std::cout << "\033[32mParallelForFixed\033[0m: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(end -
                                                                     start)
            << std::endl;
}
