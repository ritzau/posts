#include <iostream>
#include <ranges>
#include <vector>

bool foo(const int &a) { return a == 0; }

template <typename T>
concept fooable = requires(const std::remove_reference<T> &a) {
  { foo(a) } -> std::same_as<bool>;
};

template <typename T>
bool bar(const T &a)
  requires fooable<T>
{
  return foo(a);
}

void f() { std::cout << bar(42) << std::endl; }

int main() {
  auto v1 = std::vector<int>{1, 2, 3, 4, 5};

  auto r1 = v1 | std::views::filter([](auto x) { return x % 2 != 0; });

  std::cout << "Hello, world!\n";

  for (auto i : r1)
    std::cout << i << ", ";

  std::cout << std::endl;
}
