module;

#include <concepts>
#include <functional>
#include <utility>

#include "assert.hpp"

import Lazy.ArrayTraits;
import Lazy.InvocableTraits;
import Lazy.Utilities;

export module Lazy.Expression;

namespace lazy {

inline namespace internal {

/**
 * @brief Checks if the function "std::tuple_element_t" can be used with a type
 *
 * @tparam T The type to be checked
 * @tparam N The index of the element to check within the type "T"
 *
 * The "has_tuple_element" concept verifies whether the function "std::tuple_element_t" can be used on a type "T".
 */
template <class T, std::size_t N>
concept has_tuple_element = requires(T t) {
  typename std::tuple_element_t<N, std::remove_const_t<T>>;
  { std::get<N>(t) }
  ->std::convertible_to<const std::tuple_element_t<N, T>&>;
};

/**
 * @brief Checks if a type behaves like a tuple
 *
 * @tparam T The type to be checked for tuple-like behavior
 *
 * The "tuple_like" concept verifies whether a type "T" has tuple-like properties.
 */
template <class T>
concept tuple_like = requires(T t) {
  typename std::tuple_size<std::remove_reference_t<T>>::type;
  requires std::derived_from<std::tuple_size<std::remove_reference_t<T>>,
                             std::integral_constant<std::size_t, std::tuple_size_v<std::remove_reference_t<T>>>>;
}
&&[]<std::size_t... N>(std::index_sequence<N...>) { return (has_tuple_element<std::remove_reference_t<T>, N> && ...); }
(std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<T>>>());

/**
 * @brief Determines the return type based on reference qualifiers
 *
 * @tparam T The input type used to determine the appropriate return type
 *
 * The "return_policy_t" type alias determines an appropriate return type based on
 * the input type "T" reference qualifiers:
 * - If "T" is a rvalue reference, the returned type is passed by value
 * - If "T" is a lvalue reference, the returned type is passed by reference
 *
 * This is used to avoid dangling references when returning a rvalue reference, but not storing its result.
 */
template <typename T>
using return_policy_t = std::conditional_t<std::is_rvalue_reference_v<T>, std::remove_reference_t<T>, T>;

/**
 * @brief Checks if a type behaves like a pointer
 *
 * @tparam Ptr The type to be checked for pointer-like behavior
 *
 * The "pointer_like" concept determines whether a type "Ptr" exhibits pointer-like behavior.
 * This includes both raw pointers and smart pointer types that mimic pointer semantics.
 */
template <typename Ptr>
concept pointer_like = std::is_pointer_v<std::remove_reference_t<Ptr>> || requires(Ptr p) {
  {*p};
  {static_cast<bool>(p)};
  { p.operator->() }
  ->std::convertible_to<decltype(&*p)>;
};

/**
 * @brief Checks if a type behaves like a regular non-pointer value
 *
 * @tparam T The type to be checked for value-like behavior.
 *
 * The "value_like" concept determines whether a type "T" represents a value type,
 * excluding pointer-like types and "void".
 */
template <typename T>
concept value_like = !pointer_like<T> && !std::same_as<T, void>;

// Base case: Assure that the InputType is convertible to the TargetType, or that the TargetType can be constructed
// from the InputType. Or in the very least, that the types are same after removing const, volatile and reference
// qualifiers
template <typename TargetType, typename InputType>
struct unwrap_type {
  static_assert(std::is_convertible_v<InputType, TargetType> || std::is_constructible_v<TargetType, InputType> ||
                std::is_same_v<std::remove_cvref_t<InputType>, std::remove_cvref_t<TargetType>>);
  using type = TargetType;
};

// Specialization for when the InputType already matches the TargetType
template <typename TargetType>
struct unwrap_type<TargetType, TargetType> {
  using type = TargetType;
};

// Specialization used to unwrap reference wrappers
template <unwrapped_value TargetType, ref_wrapper InputType>
struct unwrap_type<TargetType, InputType> {
  using type = typename unwrap_type<TargetType, std::unwrap_ref_decay_t<InputType>>::type;
};

// Specialization used to unwrap pointer-like types
template <value_like TargetType, pointer_like InputType>
struct unwrap_type<TargetType, InputType> {
  using type = typename unwrap_type<TargetType, decltype(*std::declval<InputType>())>::type;
};

// Specialization used to unwrap stack arrays
template <typename TargetType, stack_array InputType>
struct unwrap_type<TargetType, InputType> {
  using type = typename unwrap_type<TargetType, decltype(std::declval<InputType>()[0])>::type;
};

// Specialization used to unwrap dynamic arrays
template <typename TargetType, dinamic_array InputType>
struct unwrap_type<TargetType, InputType> {
  using type = typename unwrap_type<TargetType, decltype(std::declval<InputType>()[0])>::type;
};

// Specialization used to unwrap iterable types
template <typename TargetType, iterable InputType>
struct unwrap_type<TargetType, InputType> {
  using type = typename unwrap_type<TargetType, decltype(*std::begin(std::declval<InputType>()))>::type;
};

// Specialization used to unwrap callable objects
template <typename TargetType, std::invocable InputType>
struct unwrap_type<TargetType, InputType> {
  using type = typename unwrap_type<TargetType, typename std::remove_reference_t<InputType>::invocable_result_t>::type;
};

/**
 * @brief Type trait to recursively unwrap an InputType until it matches the TargetType
 *
 * @tparam TargetType The type we want "InputType" to be unwrapped to.
 * @tparam InputType The type that will be recursively unwrapped.
 *
 * The "unwrap_type" structure recursively removes layers of indirection, such as pointers, reference wrappers, arrays,
 * iterables, and callable objects, from "InputType" until it becomes equivalent to the "TargetType".
 */
template <typename TargetType, typename InputType>
using unwrap_type_t = typename unwrap_type<TargetType, InputType>::type;

// Base case for checking whether an invocable can be invoked with unwrapped types
template <typename...>
struct is_unwrap_invocable;

// Specialization that converts the "InputTypes" to match the InvocableTypes, before calling the Invocable with them
template <typename Invocable, typename... InvocableTypes, typename... InputTypes>
struct is_unwrap_invocable<Invocable, std::tuple<InvocableTypes...>, InputTypes...> {
  static constexpr bool value = std::is_invocable_v<Invocable, unwrap_type_t<InvocableTypes, InputTypes>...>;
};

/**
 * @brief Determines if an invocable can be called with the specified types after unwrapping
 *
 * @tparam Invocable The callable object to be invoked
 * @tparam Ts The input types that may require conversion before being passed to the invocable
 *
 * Evaluates whether the invocable can be called with the provided input types after converting them to match the
 * expected parameter types of the invocable
 */
template <typename Invocable, typename... Ts>
inline constexpr bool is_unwrap_invocable_v =
    is_unwrap_invocable<std::unwrap_ref_decay_t<Invocable>,
                        typename FunctionTraits<std::unwrap_ref_decay_t<Invocable>>::InputTypes, Ts...>::value;

// Base case for invoke_unwrapped_result
template <typename...>
struct invoke_unwrapped_result;

// Specialization that determines the return type of the invocable
template <typename Invocable, typename... InvocableTypes, typename... InputTypes>
struct invoke_unwrapped_result<Invocable, std::tuple<InvocableTypes...>, InputTypes...> {
  using type = std::invoke_result_t<std::unwrap_ref_decay_t<Invocable>, unwrap_type_t<InvocableTypes, InputTypes>...>;
};

/**
 * @brief Determine the result type of invoking an invocable with unwrapped input types
 *
 * @tparam Invocable The callable object to be invoked
 * @tparam Ts The input types that may require conversion before being passed to the invocable
 *
 * Identifies the result type of the invocable when called with the given input types, after converting them to align
 * with the Invocable's expected parameter types.
 */
template <typename Invocable, typename... Ts>
using invoke_unwrapped_result_t =
    typename invoke_unwrapped_result<std::unwrap_ref_decay_t<Invocable>,
                                     typename FunctionTraits<std::unwrap_ref_decay_t<Invocable>>::InputTypes,
                                     Ts...>::type;

/**
 * @brief Unwraps a reference wrapper or returns the object as-is.
 *
 * @param obj The object to be unwrapped, which can be a reference wrapper or a regular object
 *
 * @return The unwrapped object or the original object
 *
 * This function checks if the provided object is a reference wrapper.
 * If it is, the underlying referenced object is retrieved using and returned. Otherwise, the object itself is returned.
 *
 * @warning rvalue objects will be passed as temporaries instead, to avoid possible dangling references.
 */
[[nodiscard]] constexpr decltype(auto) unwrap_reference(auto&& obj) {
  if constexpr (ref_wrapper<decltype(obj)>) {
    return static_cast<return_policy_t<decltype(obj.get())>>(obj.get());
  } else {
    return static_cast<return_policy_t<decltype(obj)>>(obj);
  }
}

/**
 * @brief Transforms an object to the specified target type using the least intrusive method possible.
 *
 * @tparam TargetType The desired type to which the input object should be transformed
 * @param obj The object to be transformed
 *
 * @return The transformed object as "TargetType"
 *
 * This function attempts to convert or adapt the provided object to the "TargetType" through a series of checks,
 * starting from the least intrusive operations to more complex transformations.
 *
 * @warning If none of the transformation rules apply, a "static_assert" failure will be triggered, indicating
 * unsupported transformations.
 */
template <typename TargetType>
[[nodiscard]] constexpr decltype(auto) transform_object(auto&& obj) {
  using InputType = decltype(obj);

  if constexpr (std::same_as<TargetType, InputType>) {
    if constexpr (std::is_rvalue_reference_v<TargetType> ||
                  (!std::is_lvalue_reference_v<TargetType> &&
                   !std::is_copy_constructible_v<std::remove_reference_t<std::unwrap_ref_decay_t<TargetType>>>)) {
      // This may lead to accessing a moved object
      return std::move(obj);
    } else {
      return obj;
    }
  }

  if constexpr (unwrapped_value<TargetType> && ref_wrapper<InputType>) {
    return transform_object<TargetType>(std::forward<decltype(obj.get())>(obj.get()));
  } else if constexpr (std::is_convertible_v<InputType, TargetType>) {
    return static_cast<TargetType>(obj);
  } else if constexpr (std::is_constructible_v<TargetType, InputType>) {
    return TargetType{obj};
  } else if constexpr (std::is_same_v<std::remove_reference_t<InputType>, std::remove_reference_t<TargetType>> &&
                       std::is_rvalue_reference_v<TargetType>) {
    // This may lead to accessing a moved object
    return std::move(obj);
  } else if constexpr (std::is_invocable_v<InputType>) {
    static_assert(!std::same_as<std::invoke_result_t<InputType>, void>);
    return transform_object<TargetType, std::invoke_result_t<InputType>>(obj());
  } else if constexpr (pointer_like<std::remove_cvref_t<InputType>> && value_like<std::remove_cvref_t<TargetType>>) {
    return transform_object<TargetType, decltype(*obj)>(*obj);
  } else {
    static_assert(false, "Unsupported transformation from the input type to the target type");
  }
}

/**
 * @brief Invokes a callable object with arguments unpacked from a tuple
 *
 * @param invocable The callable object to be invoked
 * @param args A tuple-like object containing the arguments to be passed to the invocable
 *
 * @return The result of invoking the callable with the provided arguments
 *
 * This function takes a callable object ("invocable") and a tuple of arguments ("args"), unpacks the tuple, transforms
 * each argument as needed, and invokes the callable with the transformed arguments.
 */
template <typename Invocable, typename InvocableArgs>
requires(tuple_like<InvocableArgs>) [[nodiscard]] constexpr decltype(auto)
    invoke_with_tuple(Invocable&& invocable, InvocableArgs&& args) {
  if constexpr (std::tuple_size_v<std::remove_cvref_t<InvocableArgs>> == 0) {
    return std::invoke(unwrap_reference(invocable));
  } else {
    return [&]<std::size_t... is>(std::index_sequence<is...>)->decltype(auto) {
      return std::invoke(
          unwrap_reference(invocable),
          transform_object<typename FunctionTraits<std::unwrap_ref_decay_t<Invocable>>::template NthType<is>>(
              std::get<is>(std::forward<InvocableArgs>(args)))...);
    }
    (std::make_index_sequence<std::tuple_size_v<std::remove_cvref_t<InvocableArgs>>>());
  }
}

/**
 * @brief Invokes a callable object with a variadic list of arguments
 *
 * @param invocable The callable object to be invoked
 * @param args The arguments to be passed to the invocable
 *
 * @return The result of invoking the callable with the provided arguments
 *
 * This function takes a callable object ("invocable") and a variadic list of arguments ("args..."),
 * transforms each argument as needed, and invokes the callable with the transformed arguments.
 */
template <typename Invocable, typename... InvocableArgs>
[[nodiscard]] constexpr decltype(auto) invoke_with_args(Invocable&& invocable, InvocableArgs&&... args) {
  if constexpr (sizeof...(args) == 0) {
    return std::invoke(unwrap_reference(invocable));
  } else {
    return [&]<std::size_t... is>(std::index_sequence<is...>)->decltype(auto) {
      return std::invoke(
          unwrap_reference(invocable),
          transform_object<typename FunctionTraits<std::unwrap_ref_decay_t<Invocable>>::template NthType<is>>(
              std::forward<InvocableArgs>(args))...);
    }
    (std::make_index_sequence<sizeof...(args)>());
  }
}

/**
 * @brief Computes the difference in array ranks between input types and the corresponding parameter types of an
 * invocable
 *
 * @tparam Invocable The callable object whose parameter types are compared
 * @tparam InputTypes The arguments to be compared with the invocable's expected parameter types
 *
 * @return A std::integer_sequence object containing the rank differences between each "InputType" and the corresponding
 * parameter type of the "Invocable"
 *
 * This function computes the difference in array rank between the provided input types and the expected parameter types
 * of the given callable. It returns a std::integer_sequence representing the rank differences for each argument.
 *
 */
template <typename Invocable, typename... InputTypes>
[[nodiscard]] consteval auto compute_arrays_rank_difference() {
  using InvocableTraits = FunctionTraits<std::unwrap_ref_decay_t<Invocable>>;

  static_assert(InvocableTraits::InputCount == sizeof...(InputTypes));

  if constexpr (sizeof...(InputTypes) == 0) {
    return std::integer_sequence<int>();
  } else {
    return []<std::size_t... is>(std::index_sequence<is...>) {
      static_assert(((obj_rank_v<unwrap_remove_cvref_t<InputTypes>> >=
                      obj_rank_v<std::remove_cvref_t<typename InvocableTraits::template NthType<is>>>)&&...));

      return std::integer_sequence<
          int, obj_rank_v<unwrap_remove_cvref_t<InputTypes>> -
                   obj_rank_v<std::remove_cvref_t<typename InvocableTraits::template NthType<is>>>...>();
    }
    (std::make_index_sequence<sizeof...(InputTypes)>());
  }
}

/**
 * @brief Computes the maximum rank difference from a sequence of rank differences
 *
 * @tparam Invocable The callable object
 * @tparam rank_diffs A variadic list of integer rank differences, representing the difference in rank between input
 * types and corresponding invocable parameters.
 *
 * @return The maximum rank difference among the provided values
 *
 * This function takes a std::integer_sequence representing the rank differences between input types and the
 * corresponding parameter types of an invocable. Using them, it computes the maximum rank difference.
 */
template <typename Invocable, int... rank_diffs>
[[nodiscard]] consteval int compute_max_rank_difference(std::integer_sequence<int, rank_diffs...>) {
  static_assert(FunctionTraits<std::unwrap_ref_decay_t<Invocable>>::InputCount == sizeof...(rank_diffs));

  if constexpr (sizeof...(rank_diffs) == 0) {
    return 0;
  } else {
    return []<int first_rank_diff, int... rest_rank_diffs>() {
      constexpr bool is_member_ptr{std::is_member_function_pointer_v<unwrap_remove_cvref_t<Invocable>> ||
                                   std::is_member_pointer_v<unwrap_remove_cvref_t<Invocable>>};

      if constexpr (sizeof...(rest_rank_diffs) == 0) {
        return is_member_ptr ? 0 : first_rank_diff;
      } else {
        int max_diff = is_member_ptr ? (rest_rank_diffs, ...) : first_rank_diff;
        ((rest_rank_diffs > max_diff ? max_diff = rest_rank_diffs, 0 : 0), ...);
        return max_diff;
      }
    }
    .template operator()<rank_diffs...>();
  }
}

/**
 * @brief Computes the size of the current (outermost) axis for the given objects
 *
 * @tparam Invocable The callable object whose parameter types are used in the axis computation
 * @tparam rank The rank (dimensionality) of the target axis
 * @tparam is A parameter pack of indices representing the parameter positions
 *
 * @param objs The objects whose axis size is being determined
 *
 * @return The size of the current (outermost) axis based on the provided input objects
 *
 * This function calculates the size of the current (outermost) axis by comparing the rank of each input object
 * ("objs...") with the expected rank for each corresponding parameter of the provided invocable ("Invocable"). It
 * ensures that the axis size is consistent with the dimensionality of the input objects, considering the differences in
 * ranks.
 */
template <typename Invocable, int rank, std::size_t... is, typename... Objs>
[[nodiscard]] std::size_t current_axis_size(std::index_sequence<is...>, const Objs&... objs) {
  static_assert(sizeof...(is) == sizeof...(Objs));

  using InvocableTraits = FunctionTraits<std::unwrap_ref_decay_t<Invocable>>;

  static constexpr bool is_member_ptr{std::is_member_function_pointer_v<unwrap_remove_cvref_t<Invocable>> ||
                                      std::is_member_pointer_v<unwrap_remove_cvref_t<Invocable>>};

  constexpr auto rank_diff = []<typename InputType, std::size_t target_idx>() {
    return obj_rank_v<unwrap_remove_cvref_t<InputType>> -
           obj_rank_v<std::remove_cvref_t<typename InvocableTraits::template NthType<target_idx>>>;
  };

  std::size_t axis = 1;

  (((is > 0 || !is_member_ptr) &&
            ((rank - rank_diff.template operator()<Objs, is>() ==
              obj_rank_v<std::remove_cvref_t<typename InvocableTraits::template NthType<is>>>) ||
             (rank == rank_diff.template operator()<Objs, is>() && rank > 0)) &&
            array_size(unwrap_reference(objs)) != 1
        ? (axis = array_size(unwrap_reference(objs)), true)
        : false) ||
   ...);

  return axis;
}

/**
 * @brief Transforms an input object if it represents an array, by reducing its dimensions
 *
 * @tparam Invocable The callable object used to determine the number of dimensional reductions performed on the object
 * @tparam expression_rank The rank (dimensionality) of the expression being evaluated
 * @tparam obj_idx The index of the object in the parameter list of the invocable
 *
 * @param obj The input object to transform
 * @param axis_offset The current (outermost) axis offset to apply during the transformation
 * @param rest_axes_offsets Additional axis offsets applied during the transformation
 *
 * @return Array with a reduced dimensionality, or the original object if no transformation is needed
 *
 * This function selectively transforms the provided object (obj) if it is an array-like structure. The transformation
 * reduces the object's dimensionality to align with the expected dimensionality of the corresponding Invocable
 * argument, while also accounting for broadcasting rules.
 */
template <typename Invocable, int expression_rank, std::size_t obj_idx, typename Obj>
[[nodiscard]] decltype(auto) transform_if_array(Obj&& obj, std::unsigned_integral auto axis_offset,
                                                std::unsigned_integral auto... rest_axes_offsets) {
  using InvocableTraits = FunctionTraits<std::unwrap_ref_decay_t<Invocable>>;
  using StrippedInputType = std::remove_cvref_t<Obj>;
  using StrippedTargetType = std::remove_cvref_t<typename InvocableTraits::template NthType<obj_idx>>;

  static_assert(obj_rank_v<StrippedInputType> >= obj_rank_v<StrippedTargetType>);

  static constexpr int rank_diff = obj_rank_v<StrippedInputType> - obj_rank_v<StrippedTargetType>;
  static constexpr bool is_member_ptr{std::is_member_function_pointer_v<unwrap_remove_cvref_t<Invocable>> ||
                                      std::is_member_pointer_v<unwrap_remove_cvref_t<Invocable>>};

  if constexpr (obj_idx == 0 && is_member_ptr) {
    return static_cast<return_policy_t<Obj>>(obj);
  } else if constexpr ((expression_rank == rank_diff && expression_rank > 0) ||
                       (expression_rank - rank_diff == obj_rank_v<StrippedTargetType> && rank_diff > 0)) {
    // Necessary for broadcasting
    axis_offset = array_size(obj) == 1 && axis_offset > 0 ? 0 : axis_offset;

    if constexpr (sizeof...(rest_axes_offsets) == 0) {
      return array_at(std::forward<Obj>(obj), axis_offset);
    } else {
      return transform_if_array<Invocable, expression_rank - 1, obj_idx>(array_at(std::forward<Obj>(obj), axis_offset),
                                                                         rest_axes_offsets...);
    }
  } else {
    return static_cast<return_policy_t<Obj>>(obj);
  }
}

// Implementation of the "are_arrays_broadcastable" method
template <typename Invocable, int expression_rank, std::size_t... is, int... input_target_ranks_diff>
[[nodiscard]] constexpr bool are_arrays_broadcastable_impl(std::index_sequence<is...> seq,
                                                           std::integer_sequence<int, input_target_ranks_diff...>,
                                                           auto&&... objs) {
  using InvocableTraits = FunctionTraits<std::unwrap_ref_decay_t<Invocable>>;

  // Check if the invocable is a member function or pointer
  constexpr bool is_member_ptr{std::is_member_function_pointer_v<unwrap_remove_cvref_t<Invocable>> ||
                               std::is_member_pointer_v<unwrap_remove_cvref_t<Invocable>>};

  // Ensure the number of objects matches the number of invocable input parameters
  static_assert(InvocableTraits::InputCount == sizeof...(objs));
  static_assert(sizeof...(is) == sizeof...(objs));

  if constexpr (sizeof...(objs) <= 1) {
    return true;
  };

  if constexpr (expression_rank > 0) {
    // For multidimensional inputs, check the size of the current axis
    const std::size_t axis_size = current_axis_size<Invocable, expression_rank>(seq, objs...);

    // Verify broadcasting conditions for the current dimension, and recursively verify the next lower dimension
    return (((is_member_ptr && (is == 0)) ||
             (expression_rank > input_target_ranks_diff &&
              expression_rank - input_target_ranks_diff !=
                  obj_rank_v<std::remove_cvref_t<typename InvocableTraits::template NthType<is>>>) ||
             (((expression_rank - input_target_ranks_diff ==
                obj_rank_v<std::remove_cvref_t<typename InvocableTraits::template NthType<is>>>) ||
               (expression_rank == input_target_ranks_diff && expression_rank > 0)) &&
              (array_size(objs) == 1 || array_size(objs) == axis_size))) &&
            ...) &&
           are_arrays_broadcastable_impl<Invocable, expression_rank - 1>(
               seq,
               std::integer_sequence<
                   int, (((expression_rank - input_target_ranks_diff ==
                               obj_rank_v<std::remove_cvref_t<typename InvocableTraits::template NthType<is>>> &&
                           input_target_ranks_diff > 0) ||
                          (expression_rank == input_target_ranks_diff && expression_rank > 0))
                             ? input_target_ranks_diff - 1
                             : input_target_ranks_diff)...>(),
               transform_if_array<Invocable, expression_rank, is>(objs, 0u)...);
  } else {
    // Base case: expression whose rank is equal to 0
    static_assert((((is_member_ptr && (is == 0)) ||
                    (obj_rank_v<unwrap_remove_cvref_t<decltype(objs)>> ==
                     obj_rank_v<std::remove_cvref_t<typename InvocableTraits::template NthType<is>>>) ||
                    (obj_rank_v<unwrap_remove_cvref_t<decltype(objs)>> - 1 ==
                     obj_rank_v<std::remove_cvref_t<typename InvocableTraits::template NthType<is>>>)) &&
                   ...));

    std::size_t axis_size{1};

    (((is > 0 || !is_member_ptr) && array_size(objs) != 1 ? (axis_size = array_size(objs), true) : false) || ...);

    return (
        ((is_member_ptr && (is == 0)) ||
         ((obj_rank_v<unwrap_remove_cvref_t<decltype(objs)>> ==
           obj_rank_v<std::remove_cvref_t<typename InvocableTraits::template NthType<is>>>)&&(array_size(objs) == 1 ||
                                                                                              array_size(objs) ==
                                                                                                  axis_size))) &&
        ...);
  }
}

/**
 * @brief Checks if multiple array-like objects are broadcastable to a common shape
 *
 * @tparam Invocable The callable type whose parameter types determine the target shapes
 * @tparam expression_rank The rank (dimension) of the output object
 * @tparam input_target_ranks_diff Parameter pack representing the rank differences between input types and the
 * corresponding Invocable argument types
 *
 * @param objs The objects that need to be evaluated for broadcasting compatibility
 * @param rank_diffs std::integer_sequence representing the rank difference between input types and the corresponding
 * Invocable argument types
 *
 * @return "true" if all objects can be broadcasted to a compatible shape; "false" otherwise.
 *
 * This function evaluates whether the provided array-like objects can be broadcasted to match the shape requirements of
 * an "Invocable"'s parameters. It verifies the compatibility of dimensions based on broadcasting rules, ensuring that
 * either:
 * - The sizes are equal along each dimension, or
 * - One of the dimensions has size 1 (allowing broadcasting)
 */
template <typename Invocable, int expression_rank, int... input_target_ranks_diff>
[[nodiscard]] constexpr bool are_arrays_broadcastable(
    auto&& objs,
    std::integer_sequence<int, input_target_ranks_diff...> rank_diffs) requires(tuple_like<decltype(objs)>) {
  return [&]<std::size_t... is>(std::index_sequence<is...> seq) {
    return are_arrays_broadcastable_impl<Invocable, expression_rank>(seq, rank_diffs,
                                                                     unwrap_reference(std::get<is>(objs))...);
  }
  (std::make_index_sequence<std::tuple_size_v<unwrap_remove_cvref_t<decltype(objs)>>>());
}

};  // namespace internal

export {
  // Predeclared "make_expression" function so that it can be used in the "expression" class
  template <typename Invocable, typename... Objects>
  [[nodiscard]] auto make_expression(Invocable && invocable, Objects && ... objects);

  /**
   * @brief Class representing the result of an unevaluated invocable
   *
   * @tparam Invocable The callable object to be lazily evaluated
   * @tparam Objects The types of the input parameters passed down to the invocable
   *
   * The "expression" class encapsulates an invocable and its corresponding input parameters. The invocable is not
   * executed immediately; instead, evaluation is deferred until its result is explicitly requested.
   *
   * The class supports broadcasting of array-like inputs, allowing operations on arrays of different shapes, as long as
   * they are compatible according to broadcasting rules. Additionally, it allows nesting of expressions, enabling
   * composition of complex operations from simpler ones.
   *
   * @warning The class does not support memoization, meaning repeated evaluations will recompute the result
   *
   * Note:
   * One of the future enhancements aims to integrate the einsum notation to support defining the iteration order of
   * input parameters when dealing with arrays.
   */
  template <typename Invocable, typename... Objects>
  requires(is_unwrap_invocable_v<Invocable, Objects...> &&
           FunctionTraits<std::unwrap_ref_decay_t<Invocable>>::InputCount ==
               sizeof...(Objects)) class expression<Invocable, Objects...> {
    using InvocableTraits = FunctionTraits<std::unwrap_ref_decay_t<Invocable>>;

    // Represents rank differences between the input objects and the corresponding Invocable argument types
    static constexpr auto input_target_ranks_diff{compute_arrays_rank_difference<Invocable, Objects...>()};

    // The number of input arguments that the "Invocable" receives
    static constexpr std::size_t num_inv_inputs{sizeof...(Objects)};

    // Variable indicating whether the "Invocable" is a member function pointer or a member object pointer
    static constexpr bool is_member_ptr{std::is_member_function_pointer_v<unwrap_remove_cvref_t<Invocable>> ||
                                        std::is_member_pointer_v<unwrap_remove_cvref_t<Invocable>>};

   public:
    // TODO: Support iterators

    // The type of the result produced when the Invocable is evaluated
    using invocable_result_t = invoke_unwrapped_result_t<Invocable, Objects...>;

    // The rank (dimensionality) of the expression after evaluation
    static constexpr int rank{compute_max_rank_difference<Invocable>(input_target_ranks_diff)};

    /**
     * @brief Constructs an expression with the given invocable and input objects
     *
     * @tparam _Objects Parameter pack representing the types of the input objects. These must match the types of the
     * corresponding `Objects` template parameters after removing cv-ref qualifiers.
     *
     * @param invocable The callable object that will be lazily invoked when the expression is evaluated
     * @param objects The input arguments to be passed to the invocable
     *
     * This constructor initializes the expression with an invocable and its corresponding input objects
     */
    template <typename... _Objects>
    requires((std::is_same_v<std::remove_cvref_t<Objects>, std::remove_cvref_t<_Objects>>)&&...)
        [[nodiscard]] explicit expression(Invocable invocable, _Objects&&... objects)
        : invocable{invocable}, objs{std::forward<_Objects>(objects)...} {
#ifdef LAZY_ENABLE_EXPECTS
      Expects((are_arrays_broadcastable<Invocable, rank>(objs, input_target_ranks_diff)));
#endif
    }

    // Default copy constructor
    [[nodiscard]] expression(const expression& other) = default;

    // Default move constructor
    [[nodiscard]] expression(expression&& other) noexcept = default;

    // Deleted copy/move operators (due to const members)
    expression& operator=(const expression& other) = delete;
    expression& operator=(expression&& other) = delete;

    /**
     * @brief Invokes the stored invocable with the provided input objects
     *
     * @return The result of invoking the invocable with the provided input objects. The exact return type is the same
     * as the one from the invocable itself.
     *
     * This function call operator triggers the evaluation of the expression when the expression's rank is zero,
     * indicating that no further iteration or broadcasting is required.
     */
    [[nodiscard]] decltype(auto) operator()() requires(rank == 0) {
      return invoke_with_tuple(std::forward<Invocable>(invocable), unwrap_reference(objs));
    }

    /**
     * @brief Invokes the stored invocable with the provided input objects in a read-only context
     *
     * @return The result of invoking the invocable with the provided input objects. The exact return type is the same
     * as the one from the invocable itself.
     *
     * This function call operator triggers the evaluation of the expression when the expression's rank is zero,
     * indicating that no further iteration or broadcasting is required.
     */
    [[nodiscard]] decltype(auto) operator()() const requires(rank == 0) {
      return invoke_with_tuple(std::forward<Invocable>(invocable), unwrap_reference(objs));
    }

    /**
     * @brief Evaluates the expression by applying the stored invocable to the input objects
     *
     * @tparam step The step size for iteration along the innermost dimension. Defaults to 1, meaning each element is
     * processed sequentially. Larger values allow skipping elements for custom iteration patterns.
     *
     * This function triggers the evaluation of the expression when the expression has a non-zero rank,
     * meaning the computation involves iterating over one or more dimensions.
     *
     * @note This function is only meaningful for expressions with a rank greater than zero. For scalar
     *       expressions (rank == 0), consider using the function call operator instead.
     */
    template <std::size_t step = 1>
    void eval() {
      static constexpr auto seq = std::make_index_sequence<num_inv_inputs>();
      [this]<std::size_t... is>(std::index_sequence<is...>) {
        eval_impl<rank, step>(seq, unwrap_reference(std::get<is>(objs))...);
      }
      (seq);
    }

    /**
     * @brief Evaluates the expression at a specific set of offsets along each axis
     *
     * @tparam offsets A variadic pack of unsigned integral values representing the index for each dimension of the
     * expression. The number of offsets must be equal to the rank of the expression
     *
     * @return The evaluated result at the specified offsets
     *
     * This function allows direct evaluation of the expression at the specified multidimensional coordinates (offsets).
     *
     * @note This function requires the expression to have a rank greater than zero. For scalar (rank == 0) expressions,
     * use the call operator instead.
     */
    [[nodiscard]] decltype(auto) eval_at(std::unsigned_integral auto... offsets) requires(rank > 0 &&
                                                                                          sizeof...(offsets) == rank) {
      // Ensure there is at least one input parameter for evaluation
      static_assert(num_inv_inputs > 0);

      // For member function pointer or member pointer invocable, ensure that more than one input exists, as the first
      // input represents the class instance and is not treated as an array.
      static_assert(num_inv_inputs > 1 || !is_member_ptr);

      return eval_at_impl(std::make_index_sequence<num_inv_inputs>(), offsets...);
    }

    /**
     * @brief Provides indexed access to the expression along the current (outermost) axis
     *
     * @param axis_offset The offset along the current axis at which the expression should be evaluated or sliced
     *
     * @return A new expression representing the sub-slice (if rank > 1) or the evaluated result (if rank == 1) at the
     * specified "axis_offset".
     *
     * This operator enables partial evaluation or slicing of the expression along the specified axis.
     * For multidimensional expressions, it returns a new expression representing a sub-slice at the given
     * "axis_offset". For one-dimensional expressions, it directly evaluates the expression at the specified offset.
     */
    [[nodiscard]] decltype(auto) operator[](std::size_t axis_offset) requires(
        rank > 0 && ((std::is_copy_constructible_v<std::remove_reference_t<Objects>>)&&...)) {
      static_assert(num_inv_inputs > 1 || !is_member_ptr);

      auto transform_ref_array = [&]<std::size_t obj_idx>() -> decltype(auto) {
        decltype(auto) obj = std::get<obj_idx>(objs);
        decltype(auto) res = transform_if_array<Invocable, rank, obj_idx>(unwrap_reference(obj), axis_offset);
        using res_t = decltype(res);

        if constexpr (ref_wrapper<decltype(obj)> && (std::is_lvalue_reference_v<res_t> && !ref_wrapper<res_t>)) {
          return std::ref(res);
        } else {
          return static_cast<return_policy_t<res_t>>(res);
        }
      };

      // Apply the transformation to all input objects
      return [&]<std::size_t... is>(std::index_sequence<is...>)->decltype(auto) {
        if constexpr (rank > 1) {
          // For multidimensional expressions, return a new expression representing the sub-slice
          return make_expression(invocable, transform_ref_array.template operator()<is>()...);
        } else {
          // For one-dimensional expressions, directly evaluate the expression at the given offset
          return invoke_with_args(invocable, transform_ref_array.template operator()<is>()...);
        }
      }
      (std::make_index_sequence<num_inv_inputs>());
    }

    /**
     * @brief Returns the size of the expression along its current (outermost) axis
     *
     * @return The size of the expression along the current axis, or 0 if the rank of the expression is 0
     *
     * This function computes the number of elements along the current axis of the expression
     */
    [[nodiscard]] constexpr std::size_t size() const {
      if constexpr (rank == 0) {
        return 0;
      } else {
        return [&]<std::size_t... is>(std::index_sequence<is...> seq) {
          return current_axis_size<Invocable, rank>(seq, std::get<is>(objs)...);
        }
        (std::make_index_sequence<num_inv_inputs>());
      }
    }

   private:
    // Implementation of the "eval" method
    template <int eval_rank, std::size_t step, std::size_t... is, typename... _Objs>
    void eval_impl(std::index_sequence<is...> seq, _Objs&&... _objs) {
      if constexpr (num_inv_inputs == 0 || num_inv_inputs == 1 && is_member_ptr) {
        if constexpr (std::same_as<void, invocable_result_t>) {
          invoke_with_args(std::forward<Invocable>(invocable), std::forward<_Objs>(_objs)...);
        } else {
          std::ignore = invoke_with_args(std::forward<Invocable>(invocable), std::forward<_Objs>(_objs)...);
        }
        return;
      }

      if constexpr (eval_rank > 0) {
        const std::size_t axis_size = current_axis_size<Invocable, eval_rank>(seq, _objs...);

#ifdef LAZY_ENABLE_EXPECTS
        if constexpr (eval_rank == 1) {
          Expects(step <= axis_size && axis_size % step == 0);
        }
#endif

        for (std::size_t axis_offset = 0; axis_offset < axis_size;
             eval_rank == 1 ? axis_offset += step : ++axis_offset) {
          eval_impl<eval_rank - 1, step>(seq, transform_if_array<Invocable, eval_rank, is>(
                                                  unwrap_reference(std::forward<_Objs>(_objs)), axis_offset)...);
        }
      } else {
        if constexpr (std::same_as<void, invocable_result_t>) {
          invoke_with_args(std::forward<Invocable>(invocable), std::forward<_Objs>(_objs)...);
        } else {
          std::ignore = invoke_with_args(std::forward<Invocable>(invocable), std::forward<_Objs>(_objs)...);
        }
      }
    }

    // Implementation of the "eval_at" method
    template <std::size_t... is>
    [[nodiscard]] decltype(auto) eval_at_impl(std::index_sequence<is...>, std::unsigned_integral auto... offsets) {
      auto try_reduce_array = []<int expression_rank, std::size_t obj_idx, typename Obj>(
                                  Obj&& obj, auto&& try_reduce_array, std::size_t current_axis_offset,
                                  std::unsigned_integral auto... rest_axes_offsets) -> decltype(auto) {
        static constexpr int target_rank =
            obj_rank_v<std::remove_cvref_t<typename InvocableTraits::template NthType<obj_idx>>>;
        static constexpr int rank_diff = obj_rank_v<std::remove_cvref_t<Obj>> - target_rank;

        if constexpr (rank_diff > 0) {
          if constexpr (expression_rank == rank_diff || (expression_rank - rank_diff == target_rank)) {
            return transform_if_array<Invocable, expression_rank, obj_idx>(std::forward<Obj>(obj), current_axis_offset,
                                                                           rest_axes_offsets...);
          } else {
            return try_reduce_array.template operator()<expression_rank - 1, obj_idx>(
                std::forward<Obj>(obj), try_reduce_array, rest_axes_offsets...);
          }
        } else {
          return static_cast<return_policy_t<Obj>>(obj);
        }
      };

      return invoke_with_args(invocable, try_reduce_array.template operator()<rank, is>(
                                             unwrap_reference(std::get<is>(objs)), try_reduce_array, offsets...)...);
    }

    Invocable invocable;                               // The callable object to be evaluated
    std::tuple<std::remove_cvref_t<Objects>...> objs;  // The input parameters passed to the invocable
  };

  /**
   * @brief Creates an expression object from the given invocable and input objects
   *
   * @tparam Invocable The type of the callable object
   * @tparam Objects The types of the input parameters to be passed to the invocable
   *
   * @param invocable The callable object
   * @param objects The input arguments to be associated with the invocable
   *
   * @return An "expression<Invocable, Objects...>" object representing the unevaluated invocation of the callable with
   * the provided inputs
   *
   * This function serves as a convenience factory for constructing an "expression" instance.
   */
  template <typename Invocable, typename... Objects>
  [[nodiscard]] auto make_expression(Invocable && invocable, Objects && ... objects) {
    return expression<Invocable, Objects...>(std::forward<Invocable>(invocable), std::forward<Objects>(objects)...);
  }
}

}  // namespace lazy
