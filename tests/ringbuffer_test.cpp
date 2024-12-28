#include "project/ringbuffer.hpp"

#include <gtest/gtest.h>

#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

void producer(project::RingBuffer<int>& ringbuffer, int n) {
  for (int i = 0; i < n; ++i) {
    ringbuffer.enqueue(i);
  }
}

void consumer(project::RingBuffer<int>& ringbuffer, int n) {
  int val;
  for (int i = 0; i < n; ++i) {
    ringbuffer.dequeue(val);
  }
}

TEST(MPMC, RingBuffer) {
  project::RingBuffer<int> ringbuffer(2048);
  ASSERT_EQ(ringbuffer.capacity(), 2048);
  int n = 100000, num_threads = 20;
  ASSERT_EQ(ringbuffer.size(), 0);
  ASSERT_EQ(ringbuffer.full(), false);
  ASSERT_EQ(ringbuffer.empty(), true);

  std::vector<std::thread> producers, consumers;
  for (int i = 0; i < num_threads; ++i) {
    producers.push_back(std::thread(producer, std::ref(ringbuffer), n));
    consumers.push_back(std::thread(consumer, std::ref(ringbuffer), n));
  }

  for (int i = 0; i < num_threads; ++i) {
    producers[i].join();
    consumers[i].join();
  }
  ASSERT_EQ(ringbuffer.size(), 0);
}

int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  ::testing::GTEST_FLAG(color) = "yes";
  return RUN_ALL_TESTS();
}