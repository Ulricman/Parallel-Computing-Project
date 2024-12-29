#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include <string>
#include <thread>
#include <vector>

#include "project/parallel_for.hpp"
#include "project/threadpool.hpp"
#include "project/utils.hpp"

void test_sum(int n_rows, int n_cols) {
  using val_t = double;
  std::vector<std::vector<val_t>> matrix(n_rows, std::vector<val_t>(n_cols));
  project::fill_matrix<val_t>(matrix, 10, 10);

  // Sequental Summation.
  auto start = project::start();
  val_t res = 0;
  for (const auto& row : matrix) {
    res += std::accumulate(row.cbegin(), row.cend(), 0.0);
  }
  auto end = project::end();
  std::cout << "\033[32m      Sequential\033[0m: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(end -
                                                                     start)
            << std::endl;

  // Parallel summation with threadpool.
  project::ThreadPool threadpool(100, 10);
  start = project::start();
  res = 0;

  std::vector<double> row_summation(n_rows, 0);
  for (int row_idx = 0; row_idx < n_rows; ++row_idx) {
    val_t& row_res = row_summation[row_idx];
    auto& row = matrix[row_idx];
    auto task = [&row_res, &row]() {
      for (const auto& val : row) row_res += val;
    };
    threadpool.submit(task);
  }
  threadpool.shutdown();
  res = std::accumulate(row_summation.cbegin(), row_summation.cend(), 0.0);
  end = project::end();
  std::cout << "\033[32m        Parallel\033[0m: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(end -
                                                                     start)
            << std::endl;

  // Parallel summation with parallel_for().
  start = project::start();
  auto task = [&matrix, &row_summation](int idx) {
    row_summation[idx] =
        std::accumulate(matrix[idx].cbegin(), matrix[idx].cend(), 0);
  };
  project::parallel_for(project::blocked_range(0, n_rows, n_rows / 10), task);
  res = std::accumulate(row_summation.cbegin(), row_summation.cend(), 0.0);
  end = project::end();
  std::cout << "\033[32m     ParallelFor\033[0m: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(end -
                                                                     start)
            << std::endl;

  // Parallel summation with parallel_for() with fixed number of threads.
  start = project::start();
  project::parallel_for(project::blocked_range(0, n_rows), task, 15);
  res = std::accumulate(row_summation.cbegin(), row_summation.cend(), 0.0);
  end = project::end();
  std::cout << "\033[32mParallelForFixed\033[0m: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(end -
                                                                     start)
            << std::endl;
}

int main() {
  int n_rows = 1000, n_cols = 1000000;
  test_sum(n_rows, n_cols);
}