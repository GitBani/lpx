#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <memory>
#include <optional>

#include "LpxMap.hpp"

template <typename K, typename V>
class ModTombMap {
public:
  explicit ModTombMap(std::size_t capacity)
      : capacity_{capacity}, ctrl_{std::make_unique_for_overwrite<std::uint8_t[]>(capacity)},
        slots_{entry_allocator.allocate(capacity)} {
    std::fill_n(ctrl_.get(), capacity, k_empty);
  }

  ~ModTombMap() {
    for (std::size_t i{0}; i < capacity_; ++i) {
      if (!(ctrl_[i] & k_vacant_mask)) {
        std::destroy_at(&slots_[i]);
      }
    }
    entry_allocator.deallocate(slots_, capacity_);
  }

  ModTombMap(const ModTombMap& other) = delete;
  ModTombMap& operator=(const ModTombMap& other) = delete;
  ModTombMap(ModTombMap&& other) = delete;
  ModTombMap& operator=(ModTombMap&& other) = delete;

  bool insert(const K& key, const V& value) {
    auto [h1, h2] = split_hash(hash_key(key));
    auto i = h1 % capacity_;
    std::optional<std::size_t> tombstone_idx;

    for (;; i = (i + 1) % capacity_) {
      if (ctrl_[i] == k_empty) {
        if (tombstone_idx.has_value()) {
          i = *tombstone_idx;
        }
        std::construct_at(&slots_[i], key, value);
        ctrl_[i] = h2;
        ++size_;
        return true;
      }
      if (ctrl_[i] == k_tombstone && !tombstone_idx) {
        // take note of available tombstone, keep probing
        // to check if this is duplicate insert
        tombstone_idx = i;
        continue;
      }
      if (ctrl_[i] == h2 && slots_[i].key == key) {
        return false;
      }
    }

    // unreachable (resizing needs to implemented first)
    return false;
  }

  bool erase(const K& key) {
    auto [h1, h2] = split_hash(hash_key(key));
    auto i = h1 % capacity_;

    for (;; i = (i + 1) % capacity_) {
      if (ctrl_[i] == h2 && slots_[i].key == key) {
        ctrl_[i] = k_tombstone;
        std::destroy_at(&slots_[i]);
        --size_;
        return true;
      }
      if (ctrl_[i] == k_empty) {
        return false;
      }
    }

    // unreachable (resizing needs to implemented first)
    return false;
  }

  std::optional<V> find(const K& key) const {
    auto [h1, h2] = split_hash(hash_key(key));
    auto i = h1 % capacity_;

    for (;; i = (i + 1) % capacity_) {
      if (ctrl_[i] == h2 && slots_[i].key == key) {
        return std::optional{slots_[i].value};
      }
      if (ctrl_[i] == k_empty) {
        return std::nullopt;
      }
    }

    // unreachable (resizing needs to implemented first)
    return std::nullopt;
  }

  std::size_t size() const { return size_; }

private:
  std::size_t size_{0};
  std::size_t capacity_;
  std::unique_ptr<std::uint8_t[]> ctrl_;
  std::allocator<Entry<K, V>> entry_allocator{};
  Entry<K, V>* slots_;

  friend std::ostream& operator<<(std::ostream& os, const ModTombMap& map) {
    os << '{';
    std::size_t i{0};
    std::size_t counter{0};
    for (; counter < map.size_ && i < map.capacity_; ++i) {
      if (!(map.ctrl_[i] & k_vacant_mask)) {
        os << map.slots_[i].key << ": " << map.slots_[i].value;
        ++counter;
        break;
      }
    }
    for (++i; counter < map.size_ && i < map.capacity_; ++i) {
      if (!(map.ctrl_[i] & k_vacant_mask)) {
        os << ", " << map.slots_[i].key << ": " << map.slots_[i].value;
        ++counter;
      }
    }
    os << "} (Size: " << map.size_ << ")\n";
    return os;
  }
};
