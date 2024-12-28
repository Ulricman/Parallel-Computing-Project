#include "project/threadpool.hpp"

namespace project {

ThreadPool::ThreadPool(size_t buffer_size, size_t num_threads)
    : tasks_(buffer_size), shutdown_(false) {
  for (size_t thread_idx = 0; thread_idx != num_threads; ++thread_idx) {
    workers_.push_back(std::thread([this]() { this->worker_loop(); }));
  }
}

void ThreadPool::worker_loop() {
  while (!shutdown_.load(std::memory_order_relaxed)) {
    task_t task;
    tasks_.dequeue(task);
    task();
  }
}

void ThreadPool::shutdown() {
  shutdown_.store(true, std::memory_order_relaxed);

  // Make sure all threads are waken up.
  for (int worker_idx = 0; worker_idx < workers_.size(); ++worker_idx) {
    submit([]() {});
  }

  // Waiting until all threads are done.
  for (auto& worker : workers_) {
    worker.join();
  }
}

}  // namespace project