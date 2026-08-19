#include <concepts>
#include <optional>

template <typename M, typename K, typename V>
concept LpxMap = requires(M m, const K k, const V v) {
  { m.insert(k, v) }
  ->std::same_as<bool>;
  { m.find(k) }
  ->std::same_as<std::optional<V>>;
  { m.remove(k) }
  ->std::same_as<bool>;
};