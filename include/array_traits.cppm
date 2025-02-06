module;

#include <concepts>
#include <iterator>

#include "assert.hpp"

import Lazy.Utilities;

export module Lazy.ArrayTraits;

namespace lazy {

// Forward declare expression
export template <typename Invocable, typename... Ts>
class expression;

/**
 * @brief Concept that checks if a type is an instance of the class "expression".
 *
 * @tparam T The type to check
 *
 * This concept is designed to determine whether a given type "T" is an instance of the class "expression". The
 * "expression" class is a custom class that represents an expression-like object.
 *
 */
template <typename T>
concept expression_like = requires(T obj) {
  []<typename Invocable, typename... Ts>(expression<Invocable, Ts...>){}(obj);
  requires(std::same_as<unwrap_remove_cvref_t<decltype(T::rank)>, int>);
};

/**
 * @brief Concept that checks if a type has a "size()" method returning "std::size_t".
 *
 * @tparam T The type to check for the presence of a "size()" method
 *
 * This concept checks that the type "T" has a member function "size()" that returns a value of type "std::size_t".
 */
template <typename T>
concept has_size_method = requires() {
  { std::declval<T>().size() }
  ->std::same_as<std::size_t>;
};

/**
 * @brief: Determines if a type supports random access via the subscript operator
 *
 * @tparam T The type being checked for random access support
 *
 * This concept is satisfied if:
 * The type "T" provides access to elements via the "operator[]"" with an integer index.
 */
template <typename T>
concept subscriptable = requires() {
  std::declval<std::remove_const_t<T>>()[0];
};

/**
 * @brief: Concept that determines if a type supports standard iteration with specific iterator properties
 *
 * @tparam T The type being checked for iteration support
 *
 * The concept is satisfied if:
 * The type "T" can be iterated by using the methods std::begin(T) and std::end(T)
 * The iterator obtained from std::begin(T) can be dereferenced and is incrementable, weakly incrementable, or supports
 * random access.
 */
export template <typename T>
concept iterable = requires(T && container) {
  *std::begin(container);
  *std::end(container);
  requires(std::incrementable<decltype(std::begin(container))> ||
           std::weakly_incrementable<decltype(std::begin(container))> ||
           std::random_access_iterator<decltype(std::begin(container))>);
};

/**
 * @brief: A concept that matches different variations of a void*
 *
 * @tparam T The type that we compare with void*
 *
 * @warning:
 * The only method that works with void pointers is the "rank" method. Other methods can't work with them, because we
 * can't dereference them without knowing their underlying type.
 *
 * The void_pointer concept is satisfied if and only if the type T is a pointer to void
 * (i.e., void*, const void*, etc.)
 */
template <typename T>
concept void_pointer = std::same_as<unwrap_remove_cvref_t<T>, void*>;

/**
 * @brief Concept to check if a type is a stack-allocated array.
 *
 * @tparam T The type to check
 *
 * The "stack_array" concept is used to check whether a type represents a stack-allocated array type.
 */
export template <typename T>
concept stack_array = std::is_array_v<std::remove_cvref_t<T>>;

/**
 * @brief Concept to check if a type is a dynamically-allocated array.
 *
 * @tparam T The type to check
 *
 * The "DynamicArray" concept is used to check whether a type is a dynamically-allocated array type that does not
 * support iterators.
 */
export template <typename T>
concept dinamic_array = (subscriptable<T> && has_size_method<T>)&&!iterable<T>;

/**
 * @brief: Checks whether a type is an array-like type
 *
 * @tparam T The type being checked for array-like behavior
 *
 * The "array_like" concept is satisfied if a type "T" either satisfies the "BasicArray" concept or the "iterable"
 * concept.
 */
template <typename T>
concept array_like = iterable<T> || stack_array<T> || dinamic_array<T>;

/**
 * @brief: Checks whether a type is a scalar-like type
 *
 * @tparam T The type being checked for scalar-like behavior
 *
 * The "scalar_like" concept is satisfied if a type "T" doesn't satisfy the "array_like" concept, and is not void.
 */
template <typename T>
concept scalar_like = !array_like<T> && !std::same_as<T, void>;

/**
 * @brief: Struct used to determine the rank of a type
 *
 * @tparam T The type whose rank is being determined
 *
 * The rank_impl struct provides an integer value that represents the rank of the specified type.
 */
template <typename T>
struct rank_impl {
  static constexpr int value{0};
};

// rank_impl struct specialization for types that satisfy the "scalar_like" concept
template <scalar_like T>
requires(!expression_like<T>) struct rank_impl<T> {
  static constexpr int value{0};
};

// rank_impl struct specialization for types that satisfy the "void_pointer" concept
template <void_pointer T>
struct rank_impl<T> {
  static constexpr int value{1};
};

// rank_impl struct specialization for types that satisfy the "stack_array" concept
template <stack_array T>
struct rank_impl<T> {
  static constexpr int value{1 + rank_impl<decltype(std::declval<T>()[0])>::value};
};

// rank_impl struct specialization for types that satisfy the "DynamicArray" concept, and are not an instance of the
// "Expression" class
template <dinamic_array T>
requires(!expression_like<T>) struct rank_impl<T> {
  static constexpr int value{1 + rank_impl<decltype(std::declval<T>()[0])>::value};
};

// rank_impl struct specialization for types that satisfy the "iterable" concept, and are not a stack-allocated array
// type
template <iterable T>
requires(!stack_array<T>) struct rank_impl<T> {
  static constexpr int value{1 + rank_impl<decltype(*std::begin(std::declval<T>()))>::value};
};

// rank_impl struct specialization for types that are an instance of the "Expression" class
template <expression_like T>
struct rank_impl<T> {
  static constexpr int value{T::rank};
};

// rank_impl struct specialization for the "void" type
template <>
struct rank_impl<void> {
  static constexpr int value{-1};
};

/**
 * @brief: A static constexpr value representing the rank of a specific type
 *
 * @tparam T The type for which the rank is computed
 *
 * static constexpr integer variable that provides the rank of the type "T" as determined by the
 * "rank_impl" struct.
 *
 */
export template <typename T>
constexpr int obj_rank_v = rank_impl<T>::value;

/**
 * @brief: "array_size" method specialization for 0-dimensional arrays
 *
 * @param scalar The 0-dimensional array (scalar)
 *
 * @return The method always returns 1 for 0-dimensional arrays
 *
 * This specialization of the "array_size" function accepts a 0-dimensional array (or scalar), and always returns 1,
 * since it is treated as an array of only 1 element.
 */
export [[nodiscard]] constexpr std::size_t array_size(const scalar_like auto&) noexcept { return 1; }

/**
 * @brief: Returns the size (number of elements) of a C-style array
 *
 * @param arr The C-style array whose size needs to be retrieved
 *
 * @return The number of elements in the C-style array
 *
 * @note This function only works for statically-allocated arrays
 *
 * This specialization of the "array_size" function determines the size of a statically-allocated C-style array.
 */
export template <typename T, std::size_t N>
[[nodiscard]] constexpr std::size_t array_size(const T (&)[N]) noexcept {
  return N;
}

/**
 * @brief Retrieve the size of a dynamic array.
 *
 * @param array The dynamic array whose size is to be retrieved
 *
 * @return The size of the dynamic array
 *
 * This specialization of the "array_size" function determines the size of a dynamically-allocated array that does not
 * support iterators.
 */
export [[nodiscard]] constexpr std::size_t array_size(const dinamic_array auto& array) noexcept { return array.size(); }

/**
 * @brief: Returns the size of an array by calculating the distance between its "begin()" and "end()"
 * iterators.
 *
 * @param array The array whose size is to be determined.
 *
 * @return The number of elements in the array
 *
 * This specialization of the "array_size" function determines the size of an array-like object by calculating the
 * distance between the "begin()" and "end()" iterators.
 */
export [[nodiscard]] constexpr std::size_t array_size(const iterable auto& array) {
  return static_cast<std::size_t>(std::distance(std::begin(array), std::end(array)));
}

/**
 * @brief: Specialization of the "array_at" function for scalars (0-dimensional arrays)
 *
 * @param scalar The scalar whose value is being accessed
 * @param N The index (must be 0, as there is only one element)
 *
 * @return The scalar value.
 *
 * This specialization of the method "array_at" handles the case where the array is a scalar (effectively a
 * 0-dimensional array). It returns the value of the scalar, treating it as an array with a single element.
 */
export [[nodiscard]] constexpr decltype(auto) array_at(scalar_like auto&& scalar, std::size_t N) {
#ifdef LAZY_ENABLE_EXPECTS
  Expects(N == 0);
#endif
  return std::forward<decltype(scalar)>(scalar);
}

/**
 * @brief: Returns the element at the Nth position of an array
 *
 * @param arr The array whose element is being accessed
 * @param N The index of the element to return
 *
 * @return The element at the "Nth" position of the array
 *
 * @note The function uses a customizable Expects statement for bound checking. Depending on the configuration, it can
 * either terminate the application, throw an exception, or have no effect (resulting in undefined behavior) if
 * out-of-bounds access is attempted.
 *
 * This specialization of the method "array_at" returns the element at the "Nth" position of an array-like object.
 */
export [[nodiscard]] decltype(auto) array_at(array_like auto&& arr, std::size_t N) {
#ifdef LAZY_ENABLE_EXPECTS
  Expects(N < array_size(std::forward<decltype(arr)>(arr)));
#endif

  if constexpr (subscriptable<decltype(arr)>) {
    return arr[N];
  } else if constexpr (iterable<decltype(arr)>) {
    if constexpr (std::random_access_iterator<decltype(arr.begin())>) {
      return *(arr.begin() + N);
    } else if constexpr (std::incrementable<decltype(arr.begin())> ||
                         std::weakly_incrementable<decltype(arr.begin())>) {
      auto it = arr.begin();
      std::advance(it, N);
      return *it;
    } else {
      static_assert(false, "Failed to iterate the array");
    }
  } else {
    static_assert(false, "Failed to iterate the array");
  }
}

}  // namespace lazy
