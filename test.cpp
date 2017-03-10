#include <iostream>
#include <thread>
#include <memory>
#include <future>
#include <atomic>
#include <boost/optional.hpp>

#include "gtest/gtest.h"
#include "bounded_buffer.h"

std::atomic_int ai(2);
using BufferPtr = std::shared_ptr<Orbit::BoundedBuffer<int>>;
using ThreadPtr = std::shared_ptr<std::thread>;

void sleep(int ms) {
  std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

void producer(BufferPtr q) {
  for (int i = 0; i < 1000000; ++i) {
    q->push(i);
  }
}

int consumer(BufferPtr q) {
  for (int i = 0; i < 1000000; ++i) {
    q->pop();
  }
}

void push5(BufferPtr q) {
  for (int i = 0; i < 5; ++i) {
    sleep(500);
    q->push(i);
  }
}

TEST(BoundedBufferTest, HappyPath) {
  auto q = std::make_shared<Orbit::BoundedBuffer<int>>(1000);
  ThreadPtr p1 = std::make_shared<std::thread>(producer, q);
  ThreadPtr p2 = std::make_shared<std::thread>(producer, q);
  ThreadPtr c1 = std::make_shared<std::thread>(consumer, q);
  ThreadPtr c2 = std::make_shared<std::thread>(consumer, q);
  p1->join();
  p2->join();
  c1->join();
  c2->join();
  EXPECT_EQ(0, q->size());
}

TEST(BoundedBufferTest, PopFromEmptyQueueAndWait) {
  auto start = std::chrono::system_clock::now();
  auto q = std::make_shared<Orbit::BoundedBuffer<int>>(1000);
  int item = 0;
  EXPECT_FALSE(q->try_pop_for(std::chrono::seconds(1), item));;
  EXPECT_FALSE(q->try_pop_until(std::chrono::system_clock::now() + std::chrono::seconds(1), item));;
  auto end = std::chrono::system_clock::now();
  int c = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
  EXPECT_GE(c, 2000);
  EXPECT_LE(c, 2500);
}

TEST(BoundedBufferTest, PopFromQueueAndWait) {
  auto start = std::chrono::system_clock::now();
  auto q = std::make_shared<Orbit::BoundedBuffer<int>>(1000);
  q->push(7);
  q->push(8);
  int item = 0;
  EXPECT_TRUE(q->try_pop_for(std::chrono::seconds(1), item));;
  EXPECT_EQ(7, item);
  EXPECT_TRUE(q->try_pop_for(std::chrono::seconds(1), item));;
  EXPECT_EQ(8, item);
  auto end = std::chrono::system_clock::now();
  int c = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
  EXPECT_LE(c, 500);
}

TEST(BoundedBufferTest, FiveEntry) {
  auto start = std::chrono::system_clock::now();
  auto q = std::make_shared<Orbit::BoundedBuffer<int>>(1000);
  ThreadPtr g = std::make_shared<std::thread>(push5, q);
  int item;
  q->try_pop_until(std::chrono::system_clock::now() + std::chrono::seconds(5), item);
  g->join();
  auto end = std::chrono::system_clock::now();
  int c = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
  EXPECT_GE(c, 2500);
  EXPECT_LE(c, 3000);
}

