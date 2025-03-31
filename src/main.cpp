// Copyright 2024
#include <chrono>
#include <thread>

#include "ThreadPool.h"

int main(int argc, char* argv[]) {
  ThreadPool pool;
  pool.start();

  std::this_thread::sleep_for(std::chrono::seconds(5));
  return 0;
}
