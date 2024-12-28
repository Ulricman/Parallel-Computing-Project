#ifndef __PROJECT_RINGBUFFER_HPP__
#define __PROJECT_RINGBUFFER_HPP__

#include <atomic>
#include <vector>

namespace project {

template <typename T>
class RingBuffer {
 private:
  std::vector<T> buffer_;
  std::atomic<int> head_, tail_;
  const int capacity_;

 public:
  explicit RingBuffer(size_t buffer_size = 1024)
      : buffer_(buffer_size), capacity_(buffer_size) {}
  size_t size() const { return buffer_.size(); }
  void enqueue(T&& item);
  void dequeue(T& item);
};

template <typename T>
void RingBuffer<T>::enqueue(T&& item) {}

template <typename T>
void RingBuffer<T>::dequeue(T& item) {}

}  // namespace project

#endif