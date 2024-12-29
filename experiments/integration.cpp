#include <chrono>
#include <iostream>
#include <thread>

#include "project/blocked_range.hpp"
#include "project/parallel_for.hpp"
#include "project/threadpool.hpp"
#include "project/utils.hpp"

inline double func(double x) {
  return 100.0 / (1.0 + x * x) + 1 / (1.0 + x * x * x * x);
}

int main() {
  double lower_limit = -10.0, upper_limit = 10.0;
  //   size_t num_rects = 1000000000;
  size_t num_rects = 1000000000;
  double width = (upper_limit - lower_limit) / num_rects;

  // Sequential integration.
  auto start = project::start();
  double area = 0.0;
  for (size_t rect_idx = 0; rect_idx != num_rects; ++rect_idx) {
    area += width * func(lower_limit + (rect_idx + 0.5) * width);
  }
  auto end = project::end();
  std::cout << "\033[32m      Sequential\033[0m: "
            << std::chrono::duration_cast<std::chrono::microseconds>(end -
                                                                     start)
            << std::endl;

  // Parallel integration with parallel_for().
  start = project::start();
  auto task = [&](size_t rect_idx) {
    area += width * func(lower_limit + (rect_idx + 0.5) * width);
  };
  project::parallel_for(
      project::blocked_range<size_t>(0, num_rects, num_rects / 15), task);
  end = project::end();
  std::cout << "\033[32m     ParallelFor\033[0m: "
            << std::chrono::duration_cast<std::chrono::microseconds>(end -
                                                                     start)
            << std::endl;

  // Parallel integration with parallel_for() with fixed number of threads.
  start = project::start();
  int num_threads = 10;
  int block_size = num_rects / num_threads;
  std::vector<double> sub_areas(num_threads, 0.0);
  auto new_task = [&](size_t rect_idx) {
    sub_areas[rect_idx / block_size] +=
        width * func(lower_limit + (rect_idx + 0.5) * width);
  };
  project::parallel_for(project::blocked_range<size_t>(0, num_rects), new_task,
                        num_threads);
  area = std::accumulate(sub_areas.cbegin(), sub_areas.cend(), 0.0);
  end = project::end();
  std::cout << "\033[32mParallelForFixed\033[0m: "
            << std::chrono::duration_cast<std::chrono::microseconds>(end -
                                                                     start)
            << std::endl;
}
