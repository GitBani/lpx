#pragma once

#include <concepts>
#include <cstdint>
#include <optional>
#include <utility>

template <typename M, typename K, typename V>
concept LpxMap = requires(M m, const K& k, const V& v) {
  { m.insert(k, v) } -> std::same_as<bool>;
  { m.find(k) } -> std::same_as<std::optional<V>>;
  { m.erase(k) } -> std::same_as<bool>;
};

template <typename K, typename V>
struct Entry {
  K key;
  V value;

  template <typename KeyType, typename ValueType>
  Entry(KeyType&& k, ValueType&& v)
      : key{std::forward<KeyType>(k)}, value{std::forward<ValueType>(v)} {}
};

// metadata ctrl bytes
inline constexpr std::uint8_t k_empty = 0b1000'0000;
inline constexpr std::uint8_t k_tombstone = 0b1100'0000;
// k_occupied = 0b0xxxxxxx, xxxxxxx = 7 lowest bits of hash (h2)
inline constexpr std::uint8_t k_vacant_mask = 0x80;

template <typename K>
std::size_t hash_key(K key) {
  return std::hash<K>{}(key);
}

struct SplitHash {
  std::size_t h1;  // upper 57 bits
  std::uint8_t h2; // lower 7 bits
};

inline constexpr SplitHash split_hash(std::size_t hash) noexcept {
  return {hash >> 7, static_cast<uint8_t>(hash & 0x7F)};
}

// using fixed capacity for the maps
inline constexpr std::size_t k_cap_sm = 1 << 14;
inline constexpr std::size_t k_cap_md = 1 << 20;
inline constexpr std::size_t k_cap_lg = 1 << 24;