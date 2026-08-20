#pragma once

#include <concepts>
#include <cstdint>
#include <optional>
#include <utility>

template <typename M, typename K, typename V>
concept LpxMap = requires(M m, const K& k, const V& v) {
  { m.insert(k, v) }
  ->std::same_as<bool>;
  { m.find(k) }
  ->std::same_as<std::optional<V>>;
  { m.erase(k) }
  ->std::same_as<bool>;
};

template <typename K, typename V>
struct Entry {
  K key;
  V value;

  template <typename KeyType, typename ValueType>
  Entry(KeyType&& k, ValueType&& v)
      : key{std::forward<KeyType>(k)}, value{std::forward<ValueType>(v)} {}
};

inline constexpr uint8_t kEmpty = 0;
inline constexpr uint8_t kTombstone = 1;
// kOccupied = 0b1xxxxxxx, xxxxxxx = 7 lowest bits of hash (h2)

// ? Maybe cast to uint64_t
template <typename K>
std::size_t hash_key(K key) {
  return std::hash<K>{}();
}
std::pair<std::size_t, std::size_t> split_hash(std::size_t hash);