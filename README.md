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
    q->push(i);
  }
}
   
int consumer(BufferPtr q) {
  for (int i = 0; i < 1000000; ++i) {
    int item = q->pop();
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
