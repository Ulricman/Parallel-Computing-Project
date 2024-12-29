#ifndef __PROJECT_BLOCKED_RANGE_HPP__
#define __PROJECT_BLOCKED_RANGE_HPP__

#include <cassert>
#include <vector>

namespace project {

template <typename Value>
class blocked_range {
 public:
  using const_iterator = Value;
  using size_type = std::size_t;

 private:
  Value begin_, end_;
  size_type grainsize_;

 public:
  blocked_range(Value begin, Value end, size_type grainsize = 1)
      : begin_(begin), end_(end), grainsize_(grainsize) {}
  const_iterator begin() const { return begin_; }
  const_iterator end() const { return end_; }
  size_type size() const { return static_cast<size_type>(end_ - begin_); }
  size_type grainsize() const { return grainsize_; }
  bool empty() const { return !(begin_ < end_); }
  bool is_divisible() const { return grainsize_ < size(); }

  blocked_range(const blocked_range& r) = default;

  /**
   * * Split another blocked_range and modify itself inplace.
   */
  blocked_range split();

  /**
   * * Split itself into `num_blocks` blocked_ranges without modifying
   * * itself.
   */
  std::vector<blocked_range> split(size_t num_blocks) const;
};

template <typename Value>
blocked_range<Value> blocked_range<Value>::split() {
  assert(is_divisible() && "cannot split blocked_range that is not disvisible");
  Value middle = (begin_ + end_) / 2;
  blocked_range new_range(middle, end_, grainsize_);
  end_ = middle;
  return new_range;
}

template <typename Value>
std::vector<blocked_range<Value>> blocked_range<Value>::split(
    size_t num_blocks) const {
  assert(size() >= num_blocks * grainsize() &&
         "size() should be no less than 'num_blocks' * grainsize()");
  std::vector<blocked_range> blocked_ranges;
  size_t block_size = static_cast<size_t>(size() / num_blocks);

  size_t begin_idx = begin_;
  for (int block_idx = 0; block_idx < num_blocks - 1; ++block_idx) {
    blocked_ranges.emplace_back(begin_idx, begin_idx + block_size, grainsize_);
    begin_idx += block_size;
  }
  blocked_ranges.emplace_back(begin_idx, end_);

  return blocked_ranges;
}

}  // namespace project

#endif