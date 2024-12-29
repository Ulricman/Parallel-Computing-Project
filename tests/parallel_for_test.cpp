#include "project/parallel_for.hpp"

#include <gtest/gtest.h>

#include <iostream>
#include <numeric>
#include <vector>

#include "project/utils.hpp"

TEST(BasicTest, ParallelFor) {
  int n = 100;
  std::vector<int> v(n);
  auto task = [&v](int idx) { v[idx - 1] = idx; };
  project::parallel_for(project::blocked_range(1, n + 1, 10), task);
  int v_sum = std::accumulate(v.cbegin(), v.cend(), 0);
  ASSERT_EQ(v_sum, n * (n + 1) / 2);
}

TEST(MatrixParallelSumInt, ParallelFor) {
  int n_rows = 100, n_cols = 100;
  using value_type = int;
  std::vector<std::vector<value_type>> matrix(n_rows,
                                              std::vector<value_type>(n_cols));
  project::fill_matrix<value_type>(matrix, -5, 5);

  // Calculate correct result first.
  value_type sequential_sum = 0;
  for (const auto& row : matrix) {
    sequential_sum += std::accumulate(row.cbegin(), row.cend(), 0);
  }

  // Use parallel_for() to calculate result.
  std::vector<value_type> row_sum(n_rows);
  auto task = [&matrix, &row_sum](int idx) {
    row_sum[idx] = std::accumulate(matrix[idx].cbegin(), matrix[idx].cend(), 0);
  };
  project::parallel_for(project::blocked_range(0, n_rows), task);
  value_type parallel_sum =
      std::accumulate(row_sum.cbegin(), row_sum.cend(), 0);

  ASSERT_EQ(sequential_sum, parallel_sum);
}

TEST(MatrixParallelSumDouble, ParallelFor) {
  int n_rows = 100, n_cols = 100;
  using value_type = double;
  std::vector<std::vector<value_type>> matrix(n_rows,
                                              std::vector<value_type>(n_cols));
  project::fill_matrix<value_type>(matrix, -5, 5);

  // Calculate correct result first.
  value_type sequential_sum = 0;
  for (const auto& row : matrix) {
    sequential_sum += std::accumulate(row.cbegin(), row.cend(), 0);
  }

  // Use parallel_for() to calculate result.
  std::vector<value_type> row_sum(n_rows);
  auto task = [&matrix, &row_sum](int idx) {
    row_sum[idx] = std::accumulate(matrix[idx].cbegin(), matrix[idx].cend(), 0);
  };
  project::parallel_for(project::blocked_range(0, n_rows), task);
  value_type parallel_sum =
      std::accumulate(row_sum.cbegin(), row_sum.cend(), 0);

  ASSERT_EQ(sequential_sum, parallel_sum);
}

TEST(MatrixParallelSumFixedNumThreadsInt, ParallelFor) {
  int n_rows = 100, n_cols = 100;
  using value_type = int;
  std::vector<std::vector<value_type>> matrix(n_rows,
                                              std::vector<value_type>(n_cols));
  project::fill_matrix<value_type>(matrix, -5, 5);

  // Calculate correct result first.
  value_type sequential_sum = 0;
  for (const auto& row : matrix) {
    sequential_sum += std::accumulate(row.cbegin(), row.cend(), 0);
  }

  // Use parallel_for() to calculate result.
  std::vector<value_type> row_sum(n_rows);
  auto task = [&matrix, &row_sum](int idx) {
    row_sum[idx] = std::accumulate(matrix[idx].cbegin(), matrix[idx].cend(), 0);
  };
  project::parallel_for(project::blocked_range(0, n_rows), task, 5);
  value_type parallel_sum =
      std::accumulate(row_sum.cbegin(), row_sum.cend(), 0);

  ASSERT_EQ(sequential_sum, parallel_sum);
}

TEST(MatrixParallelSumFixedNumThreadsDouble, ParallelFor) {
  int n_rows = 100, n_cols = 100;
  using value_type = double;
  std::vector<std::vector<value_type>> matrix(n_rows,
                                              std::vector<value_type>(n_cols));
  project::fill_matrix<value_type>(matrix, -5, 5);

  // Calculate correct result first.
  value_type sequential_sum = 0;
  for (const auto& row : matrix) {
    sequential_sum += std::accumulate(row.cbegin(), row.cend(), 0);
  }

  // Use parallel_for() to calculate result.
  std::vector<value_type> row_sum(n_rows);
  auto task = [&matrix, &row_sum](int idx) {
    row_sum[idx] = std::accumulate(matrix[idx].cbegin(), matrix[idx].cend(), 0);
  };
  project::parallel_for(project::blocked_range(0, n_rows), task, 5);
  value_type parallel_sum =
      std::accumulate(row_sum.cbegin(), row_sum.cend(), 0);

  ASSERT_EQ(sequential_sum, parallel_sum);
}

int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  ::testing::GTEST_FLAG(color) = "yes";
  return RUN_ALL_TESTS();
}