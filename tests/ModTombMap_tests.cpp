#include <gtest/gtest.h>
#include <random>
#include <unordered_map>

#include "LpxMap.hpp"
#include "ModTombMap.hpp"

template <LpxMap<int, int> M>
class CompareToStd : public testing::Test {};

using Maps = testing::Types<ModTombMap<int, int>>;
TYPED_TEST_SUITE(CompareToStd, Maps);

TYPED_TEST(CompareToStd, MatchStd) {
  TypeParam mine{k_cap_lg};
  std::unordered_map<int, int> reference;

  std::random_device rd;
  std::mt19937 rng(rd());

  for (int i = 0; i < 100'000; ++i) {
    int op = rng() % 3;
    // small key range gets collisions/duplicates
    int key = rng() % 200;
    int val = rng() % 1000;

    switch (op) {
    case 0: {
      bool actual = mine.insert(key, val);
      auto [_, expected] = reference.insert({key, val});
      ASSERT_EQ(actual, expected) << "insert mismatch at op " << i;
      break;
    }
    case 1: {
      auto actual = mine.find(key);
      auto expected = reference.find(key);
      if (expected == reference.end())
        ASSERT_FALSE(actual.has_value());
      else
        ASSERT_EQ(*actual, expected->second);
      break;
    }
    case 2: {
      bool actual = mine.erase(key);
      bool expected = reference.erase(key) > 0;
      ASSERT_EQ(actual, expected) << "erase mismatch at op " << i;
      break;
    }
    }
  }
}
