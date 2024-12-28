#include "project/ringbuffer.hpp"

#include <gtest/gtest.h>

#include <iostream>

TEST(SampleTest, RingBuffer) {
  project::RingBuffer<int> ringbuffer(2048);
  ASSERT_EQ(ringbuffer.size(), 2048);
}

int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  ::testing::GTEST_FLAG(color) = "yes";
  return RUN_ALL_TESTS();
}