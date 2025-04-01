// copyright
#include "Semaphore.h"

void Semaphore::post() {
  std::unique_lock<std::mutex> lock(mutex_);
  resLimit_++;
  cond_.notify_all();
}

void Semaphore::wait() {
  std::unique_lock<std::mutex> lock(mutex_);
  cond_.wait(lock, [this]() -> bool {
    return resLimit_ > 0;
  });
  resLimit_--;
}
