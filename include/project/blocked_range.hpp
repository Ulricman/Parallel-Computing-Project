#ifndef __PROJECT_BLOCKED_RANGE_HPP__
#define __PROJECT_BLOCKED_RANGE_HPP__

#include <cassert>

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
  blocked_range split() {
    assert(is_divisible() &&
           "cannot split blocked_range that is not disvisible");
    Value middle = (begin_ + end_) / 2;
    blocked_range new_range(middle, end_, grainsize_);
    end_ = middle;
    return new_range;
  }
};

}  // namespace project

#endif