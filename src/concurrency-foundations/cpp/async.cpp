#include <algorithm>
#include <atomic>
#include <future>
#include <iostream>
#include <thread>
#include <vector>

int main() {
  auto f = [] {
    std::cout << "Hello, " << std::this_thread::get_id() << std::endl;
  };

  auto future = std::async(std::launch::async, f);

  f();

  future.get();

  return 0;
}
