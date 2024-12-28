#include "project/threadpool.hpp"

#include <gtest/gtest.h>

#include <iostream>

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

int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  ::testing::GTEST_FLAG(color) = "yes";
  return RUN_ALL_TESTS();
}