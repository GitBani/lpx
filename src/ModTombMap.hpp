#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <utility>

#include "LpxMap.hpp"

template <typename K, typename V>
class ModTombMap {
 public:
  explicit ModTombMap(std::size_t capacity)
      : capacity_{capacity},
        ctrl_{std::make_unique<std::uint8_t[]>(capacity)},
        slots_{entry_allocator.allocate(capacity)} {
    std::fill_n(ctrl_.get(), capacity, k_empty);
  }

  bool insert(const K& key, const V& value) {
    auto [h1, h2] = split_hash(hash_key(key));
    auto i = h1 % capacity_;

    while (true) {
      if (ctrl_[i] & k_vacant_mask) {
        ctrl_[i] = h2;
        std::construct_at(&slots_[i], key, value);
        ++size_;
        return true;
      }
      if (ctrl_[i] = h2 && slots_[i].key == key) {
        return false;
      }
      i = (i + 1) % capacity_;
    }

    // (theorically) unreachable (resizing needs to implemented first)
    return false;
  }

  bool erase(const K& key) {
    auto [h1, h2] = split_hash(hash_key(key));
    auto i = h1 % capacity_;

    while (true) {
      if (ctrl_[i] = h2 && slots_[i].key == key) {
        ctrl_[i] = k_tombstone;
        std::destroy_at(&slots_[i]);
        --size_;
        return true;
      }
      if (ctrl_[i] == k_empty) {
        return false;
      }
      i = (i + 1) % capacity_;
    }

    // (theorically) unreachable (resizing needs to implemented first)
    return false;
  }

  std::optional<V> find(const K& key) {
    auto [h1, h2] = split_hash(hash_key(key));
    auto i = h1 % capacity_;

    while (true) {
      if (ctrl_[i] = h2 && slots_[i].key == key) {
        return std::optional{slots_[i].value};
      }
      if (ctrl_[i] == k_empty) {
        return std::nullopt;
      }
      i = (i + 1) % capacity_;
    }

    // (theorically) unreachable (resizing needs to implemented first)
    return std::nullopt;
  }

 private:
  std::allocator<Entry<K, V>> entry_allocator{};
  std::size_t size_{0};
  std::size_t capacity_;
  std::unique_ptr<std::uint8_t[]> ctrl_;
  Entry<K, V>* slots_;
};
