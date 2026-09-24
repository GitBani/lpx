#include <cassert>
#include <iostream>
#include <string>

#include "ModTombMap.hpp"

int main() {
  std::cout << "hello lpx\n";

  ModTombMap<std::string, int> map{k_cap_lg};
  auto inserted = map.insert("hello", 1);
  assert(inserted);
  std::cout << map;
  inserted = map.insert("hello", 2);
  assert(!inserted);
  inserted = map.insert("world", 2);
  assert(inserted);
  std::cout << map;
  inserted = map.insert("world", 3);
  assert(!inserted);
  auto found = map.find("hello");
  assert(found.value());
  found = map.find("world");
  assert(found.value());
  auto erased = map.erase("hello");
  assert(erased);
  std::cout << map;
  erased = map.erase("hello");
  assert(!erased);
  erased = map.erase("world");
  assert(erased);
  std::cout << map;
  erased = map.erase("world");
  assert(!erased);
  found = map.find("hello");
  assert(!found);
  found = map.find("world");
  assert(!found);

  return 0;
}