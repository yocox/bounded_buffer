#ifndef __BOUNDED_BUFFER_H__
#define __BOUNDED_BUFFER_H__

#include <deque>
#include <mutex>
#include <condition_variable>
#include <boost/circular_buffer.hpp>

template <typename T>
class BoundedBuffer {
 public:
  using base_container_type = boost::circular_buffer<T>;
  using size_type = typename base_container_type::size_type;

  BoundedBuffer(size_type max_capacity)
    : buffer_(max_capacity)
  {}

  void clear() {
    {
      std::unique_lock<std::mutex> lock(mutex_);
      buffer_.clear();
    }
  }

  void push(T v) {
    {
      std::unique_lock<std::mutex> lock(mutex_);
      cv_not_full_.wait(lock, [this]() { return this->buffer_.size() < buffer_.capacity(); });
      buffer_.push_front(v);
    }
    cv_not_empty_.notify_one();
  }

  void try_push(T v) {
    {
      std::unique_lock<std::mutex> lock(mutex_);
      if (buffer_.full()) {
        return false;
      }
      buffer_.push_front(v);
    }
    cv_not_empty_.notify_one();
  }

  template <typename Duration>
  bool try_push_for(T v, Duration&& d, T& item) {
    {
      std::unique_lock<std::mutex> lock(mutex_);
      if(!cv_not_full_.wait_for(lock, d, [this]() { return this->buffer_.size() < buffer_.capacity(); })) {
        return false;
      }
      buffer_.push_front(v);
    }
    cv_not_empty_.notify_one();
  }

  template <typename Timepoint>
  bool try_push_untul(T v, Timepoint&& tp, T& item) {
    {
      std::unique_lock<std::mutex> lock(mutex_);
      if(!cv_not_full_.wait_until(lock, tp, [this]() { return this->buffer_.size() < buffer_.capacity(); })) {
        return false;
      }
      buffer_.push_front(v);
    }
    cv_not_empty_.notify_one();
  }

  T pop() {
    std::unique_lock<std::mutex> lock(mutex_);
    cv_not_empty_.wait(lock, [this]() { return this->buffer_.size() > 0; });
    T v = buffer_.back();
    buffer_.pop_back();
    lock.unlock();
    cv_not_full_.notify_one();
    return v;
  }

  bool try_pop(T& item) {
    std::unique_lock<std::mutex> lock(mutex_);
    if (buffer_.empty()) {
      return false;
    }
    item = buffer_.back();
    buffer_.pop();
    return true;
  }

  template <typename Duration>
  bool try_pop_for(Duration&& d, T& item) {
    std::unique_lock<std::mutex> lock(mutex_);
    if (!cv_not_empty_.wait_for(lock, d, [this]() { return this->buffer_.size() > 0; })) {
      return false;
    }
    item = buffer_.back();
    buffer_.pop_back();
    lock.unlock();
    cv_not_full_.notify_one();
    return true;
  }

  template <typename Timepoint>
  bool try_pop_until(Timepoint&& tp, T& item) {
    std::unique_lock<std::mutex> lock(mutex_);
    if (!cv_not_empty_.wait_until(lock, tp, [this]() { return this->buffer_.size() > 0; })) {
      return false;
    }
    item = buffer_.back();
    buffer_.pop_back();
    lock.unlock();
    cv_not_full_.notify_one();
    return true;
  }

  bool empty() const {
     return buffer_.empty();
  }

  size_type size() {
    return buffer_.size();
  }

  size_type capacity() {
    return buffer_.capacity();
  }

  base_container_type get_queue() {
    std::unique_lock<std::mutex> lock(mutex_);
    return buffer_;
  }

 private:
  // for locking
  std::mutex mutex_;
  std::condition_variable cv_not_empty_;
  std::condition_variable cv_not_full_;

  // queue
  base_container_type buffer_;
} ;

#endif // __BOUNDED_BUFFER_H__
