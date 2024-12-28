#include "project/threadpool.hpp"

#include <gtest/gtest.h>

#include <iostream>
#include <numeric>
#include <ranges>
#include <vector>

#include "project/utils.hpp"

TEST(Basic, ThreadPool) {
  project::ThreadPool threadpool(1024, 10);
  threadpool.shutdown();
}

TEST(SimpleTasks, ThreadPool) {
  project::ThreadPool threadpool(1024, 20);
  int64_t num_tasks = 100000;
  std::atomic<int64_t> res = 0;

  for (int64_t task_id = 1; task_id <= num_tasks; ++task_id) {
    threadpool.submit([task_id, &res]() { res += task_id; });
  }
  threadpool.shutdown();
  ASSERT_EQ(res, num_tasks * (num_tasks + 1) / 2);
}

TEST(MatrixParallelSum, ThreadPool) {
  int n_rows = 100, n_cols = 100;
  // float does not satisify the precision requirement.
  using val_t = double;
  std::vector<std::vector<val_t>> matrix(n_rows, std::vector<val_t>(n_cols));
  project::fill_matrix<val_t>(matrix, -5, 5);

  // Calculate correct results first.
  val_t sequential_sum = 0;
  for (const auto& row : matrix) {
    for (const auto& val : row) {
      sequential_sum += val;
    }
  }

  // Use thread-pool to calculate results.
  std::vector<val_t> row_summations(n_rows, 0);
  project::ThreadPool threadpool(100, 2);
  for (int row_idx = 0; row_idx < n_rows; ++row_idx) {
    val_t& row_res = row_summations[row_idx];
    std::vector<val_t>& row = matrix[row_idx];
    auto task = [&row, &row_res]() {
      for (const auto& val : row) {
        row_res += val;
      }
    };
    threadpool.submit(task);
  }
  threadpool.shutdown();

  auto parallel_sum =
      std::accumulate(row_summations.cbegin(), row_summations.cend(), 0.0);
  ASSERT_FLOAT_EQ(sequential_sum, parallel_sum);
}

int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  ::testing::GTEST_FLAG(color) = "yes";
  return RUN_ALL_TESTS();
}