# bounded_buffer

Header only generic thread-safe C++ bounded buffer

# Feature

- Header only
- Generic
- Thread safe

# Prerequest

- C++11 compiler
- [boost/circular_buffer](http://boost.org)

# Sample Usage

```C++
#include <memory>
#include <thread>
#include "bounded_buffer.hpp"

using BufferPtr = std::shared_ptr<BoundedBuffer<int>>;

void producer(BufferPtr q) {
  for (int i = 0; i < 1000000; ++i) {
    // wait if the queue is full
    q->push(i);
    bool push_success;
    // no wait if the queue if full
    push_success = q->try_push(i);
    // wait for 1 sec if the queue is full
    push_success = q->try_push_for(i, std::chrono::second(1));
    // wait until ...
    push_success = q->try_push_until(i, std::chrono::system_clock::now() + std::chrono::second(1));
  }
}
   
int consumer(BufferPtr q) {
  for (int i = 0; i < 1000000; ++i) {
    // wait if the queue is empty
    int item = q->pop();
    bool pop_success;
    // no wait if the queue is empty
    pop_success = q->try_pop(item);
    // wait for 1 sec if the queue is empty
    pop_success = q->try_pop_for(item, std::chrono::second(1));
    // wait until ...
    pop_success = q->try_pop_until(item, std::chrono::system_clock::now() + std::chrono::second(1));
  }
}

int main() {
  // queue capacity 1000
  bounded_buffer queue(1000);
  // create 2 producer & 2 consumer
  std::thread p0(producer, q);
  std::thread p1(producer, q);
  std::thread c0(consumer, q);
  std::thread c1(consumer, q);
  p0.join();
  p1.join();
  c0.join();
  c1.join();
}
```
