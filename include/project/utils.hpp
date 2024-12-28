#ifndef __PROJECT_UTILS_HPP__
#define __PROJECT_UTILS_HPP__

#include <iostream>
#include <random>
#include <type_traits>
#include <vector>

namespace project {

template <typename T>
void fill_matrix(std::vector<std::vector<T>>& matrix, T lower_bound,
                 T upper_bound) {
  std::random_device rd;
  std::mt19937 gen(rd());

  using dist_t = std::conditional_t<std::is_integral_v<T>,
                                    std::uniform_int_distribution<T>,
                                    std::uniform_real_distribution<T>>;
  dist_t dist(lower_bound, upper_bound);

  for (int row_idx = 0; row_idx < matrix.size(); ++row_idx) {
    for (int col_idx = 0; col_idx < matrix[0].size(); ++col_idx) {
      matrix[row_idx][col_idx] = dist(gen);
    }
  }
}

}  // namespace project

#endif