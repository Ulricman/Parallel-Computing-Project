#ifndef __PROJECT_RINGBUFFER_HPP__
#define __PROJECT_RINGBUFFER_HPP__

#include <atomic>
#include <condition_variable>
#include <mutex>

namespace project {

template <typename T>
class RingBuffer {
 private:
  T* buffer_;
  int head_, tail_;
  const size_t capacity_;
  std::mutex mtx_;
  std::condition_variable is_full_, is_empty_;

 public:
  // Allocate one extra slot for the judgement of full buffer.
  explicit RingBuffer(size_t buffer_size = 1024)
      : buffer_(new T[buffer_size + 1]),
        capacity_(buffer_size),
        head_(0),
        tail_(0) {}
  ~RingBuffer() { delete[] buffer_; }
  inline size_t size() const {
    int size_ = tail_ - head_;
    return size_ >= 0 ? size_ : capacity_ + size_;
  }
  inline size_t capacity() const { return capacity_; }
  inline bool empty() const { return head_ == tail_; }
  inline bool full() const { return (tail_ + 1) % capacity_ == head_; }
  void enqueue(T& item);
  void enqueue(T&& item) { enqueue(item); }
  void dequeue(T& item);
};

template <typename T>
void RingBuffer<T>::enqueue(T& item) {
  std::unique_lock lock(mtx_);
  is_full_.wait(lock, [this]() { return !this->full(); });
  buffer_[tail_] = item;
  tail_ = (tail_ + 1) % capacity_;
  is_empty_.notify_one();
}

template <typename T>
void RingBuffer<T>::dequeue(T& item) {
  std::unique_lock lock(mtx_);
  is_empty_.wait(lock, [this]() { return !this->empty(); });
  item = buffer_[head_];
  head_ = (head_ + 1) % capacity_;
  is_full_.notify_one();
}

}  // namespace project

#endif