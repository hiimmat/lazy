module;

#include <functional>

export module Lazy.Utilities;

namespace lazy {

/**
 * @brief Trait to check if a type is a "std::reference_wrapper".
 *
 * The "is_ref_wrapper" struct provides a compile-time mechanism to determine if a given type is a
 * specialization of "std::reference_wrapper".
 *
 * @tparam T The type to check.
 */
template <typename T>
struct is_ref_wrapper {
  static constexpr bool value{false};
};

// "is_ref_wrapper" specialization for "std::reference_wrapper"
template <typename T>
struct is_ref_wrapper<std::reference_wrapper<T>> {
  static constexpr bool value{true};
};

/**
 * @brief Concept to check if a type is a `std::reference_wrapper`.
 *
 * @tparam Ref The type to check
 *
 * The "ref_wrapper" concept verifies whether a type is a "std::reference_wrapper".
 */
export template <typename Ref>
concept ref_wrapper = is_ref_wrapper<std::remove_cvref_t<Ref>>::value;

/**
 * @brief Concept to check if a type is not a "std::reference_wrapper".
 *
 * @tparam Ref The type to check.
 *
 * The "unwrapped_value" concept ensures that a given type is not wrapped into a "std::reference_wrapper".
 */
export template <typename Ref>
concept unwrapped_value = !ref_wrapper<Ref>;

/**
 * @brief Unwraps a reference wrapper and removes const, volatile, and reference qualifiers from the resulting type.
 *
 * @tparam T The type to process
 *
 * This template alias unwraps any std::reference wrapper, and then removes any const, volatile, and reference
 * qualifiers from the resulting type.
 */
export template <typename T>
using unwrap_remove_cvref_t = std::remove_cvref_t<std::unwrap_ref_decay_t<T>>;

}  // namespace lazy
