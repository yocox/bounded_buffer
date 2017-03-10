# bounded_buffer

Header only generic thread-safe C++ bounded buffer

# Feature

- Header only
- Generic
- Thread safe

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
    int push_success;
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
    int pop_success;
    // no wait if the queue is empty
    pop_success = q->try_pop(item);
    // wait for 1 sec if the queue is empty
    pop_success = q->try_pop_for(item, std::chrono::second(1));
    // wait until ...
    pop_success = q->try_pop_until(item, std::chrono::system_clock::now() + std::chrono::second(1));
  }
}

int main() {
  bounded_buffer queue;
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
