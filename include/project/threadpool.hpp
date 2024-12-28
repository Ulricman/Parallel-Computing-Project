#ifndef __PROJECT_THREADPOOL_HPP__
#define __PROJECT_THREADPOOL_HPP__

#include <atomic>
#include <chrono>
#include <functional>
#include <thread>

#include "project/ringbuffer.hpp"

namespace project {
class ThreadPool {
  using task_t = std::function<void()>;

 private:
  RingBuffer<task_t> tasks_;
  std::vector<std::thread> workers_;
  std::atomic<bool> shutdown_;

  void worker_loop();

 public:
  ThreadPool(size_t buffer_size, size_t num_threads = 1);
  ~ThreadPool() {
    if (!shutdown_.load(std::memory_order_relaxed)) shutdown();
  }
  void submit(std::function<void()>&& task) { tasks_.enqueue(task); }
  void shutdown();
};
}  // namespace project

#endif