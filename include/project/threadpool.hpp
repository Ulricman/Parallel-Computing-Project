#ifndef __PROJECT_THREADPOOL_HPP__
#define __PROJECT_THREADPOOL_HPP__

#include <atomic>
#include <functional>
#include <thread>

namespace project {
class ThreadPool {
  using task_t = std::function<void()>;

 private:
 public:
  ThreadPool(size_t buffer_size, size_t num_threads = 1);
};
}  // namespace project

#endif