#include <iostream>
#include <vector>

#include "project/blocked_range.hpp"
#include "project/parallel_for.hpp"
#include "project/threadpool.hpp"
#include "project/utils.hpp"

int main() {
  int n = 2000;
  using value_type = double;
  std::vector<std::vector<value_type>> lhs(n, std::vector<value_type>(n));
  std::vector<std::vector<value_type>> rhs(n, std::vector<value_type>(n));
  std::vector<std::vector<value_type>> res(n, std::vector<value_type>(n));

  project::fill_matrix<value_type>(lhs, -5.0, 5.0);
  project::fill_matrix<value_type>(rhs, -5.0, 5.0);

  // Sequential calculation.
  auto start = project::get_ts();
  for (int row_idx = 0; row_idx < n; ++row_idx) {
    for (int k = 0; k < n; ++k) {
      for (int col_idx = 0; col_idx < n; ++col_idx) {
        res[row_idx][col_idx] += lhs[row_idx][k] * rhs[k][col_idx];
      }
    }
  }
  auto end = project::get_ts();
  std::cout << "\033[32m      Sequential\033[0m: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(end -
                                                                     start)
            << std::endl;

  // Parallel calculation with threadpool.
  project::ThreadPool threadpool(100, 5);
  start = project::get_ts();
  for (int row_idx = 0; row_idx < n; ++row_idx) {
    for (int col_idx = 0; col_idx < n; ++col_idx) {
      auto task = [&lhs, &rhs, &res, row_idx, col_idx, n]() {
        double value = 0.0;
        for (int k = 0; k < n; ++k) {
          value += lhs[row_idx][k] * rhs[k][col_idx];
        }
        res[row_idx][col_idx] = value;
      };
      threadpool.submit(task);
    }
  }
  threadpool.shutdown();
  end = project::get_ts();
  std::cout << "\033[32m        Parallel\033[0m: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(end -
                                                                     start)
            << std::endl;

  // Parallel calculation with parallel_for().
  start = project::get_ts();
  auto task = [&lhs, &rhs, &res, n](int row_idx) {
    for (int col_idx = 0; col_idx < n; ++col_idx) {
      double value = 0;
      for (int k = 0; k < n; ++k) {
        value += lhs[row_idx][k] * rhs[k][col_idx];
      }
      res[row_idx][col_idx] = value;
    }
  };
  project::parallel_for(project::blocked_range(0, n), task);
  end = project::get_ts();
  std::cout << "\033[32m     ParallelFor\033[0m: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(end -
                                                                     start)
            << std::endl;

  // Parallel calculation with parallel_for() with fixed number of threads.
  start = project::get_ts();
  project::parallel_for(project::blocked_range(0, n), task, 40);
  end = project::get_ts();
  std::cout << "\033[32mParallelForFixed\033[0m: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(end -
                                                                     start)
            << std::endl;
}