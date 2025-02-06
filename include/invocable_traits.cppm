module;

#include <functional>
#include <tuple>
#include <type_traits>

export module Lazy.InvocableTraits;

export namespace lazy {

// TODO: Support calling conventions and transaction_safe attribute?

/**
 * @brief: A traits structure for functions, providing type information about their return type and input parameters
 *
 * @tparam R Return type of the function
 * @tparam Ts Input parameters of the function
 *
 * "FunctionTraitsBase" is a utility template structure that extracts type information from function-like types, such as
 * function pointers, function objects, lambdas, etc. This structure provides information about the return type, input
 * types, and the number of inputs.
 */
template <typename R, typename... Ts>
struct FunctionTraitsBase {
  using ResultType = R;
  using InputTypes = std::tuple<Ts...>;
  static constexpr std::size_t InputCount{sizeof...(Ts)};

  template <std::size_t N>
  using NthType = std::tuple_element_t<N, InputTypes>;
};

/**
 * @brief: A traits structure to extract type information from function-like types
 *
 * @tparam F Function-like type from which the information needs to be extracted
 *
 * The "FunctionTraits" template structure extracts type information from function-like types.
 */
template <typename F>
struct FunctionTraits;

// Standalone functions
template <typename R, typename... Ts>
struct FunctionTraits<R(Ts...)> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename... Ts>
struct FunctionTraits<R(Ts...) const> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename... Ts>
struct FunctionTraits<R(Ts...) volatile> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename... Ts>
struct FunctionTraits<R(Ts...) noexcept> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename... Ts>
struct FunctionTraits<R(Ts...) const volatile> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename... Ts>
struct FunctionTraits<R(Ts...) const noexcept> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename... Ts>
struct FunctionTraits<R(Ts...) volatile noexcept> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename... Ts>
struct FunctionTraits<R(Ts...) const volatile noexcept> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename... Ts>
struct FunctionTraits<R(Ts..., ...)> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename... Ts>
struct FunctionTraits<R(Ts..., ...) const> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename... Ts>
struct FunctionTraits<R(Ts..., ...) volatile> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename... Ts>
struct FunctionTraits<R(Ts..., ...) noexcept> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename... Ts>
struct FunctionTraits<R(Ts..., ...) const volatile> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename... Ts>
struct FunctionTraits<R(Ts..., ...) const noexcept> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename... Ts>
struct FunctionTraits<R(Ts..., ...) volatile noexcept> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename... Ts>
struct FunctionTraits<R(Ts..., ...) const volatile noexcept> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename... Ts>
struct FunctionTraits<R(Ts...) &> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename... Ts>
struct FunctionTraits<R(Ts...) const &> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename... Ts>
struct FunctionTraits<R(Ts...) volatile &> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename... Ts>
struct FunctionTraits<R(Ts...) &noexcept> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename... Ts>
struct FunctionTraits<R(Ts...) const volatile &> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename... Ts>
struct FunctionTraits<R(Ts...) const &noexcept> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename... Ts>
struct FunctionTraits<R(Ts...) volatile &noexcept> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename... Ts>
struct FunctionTraits<R(Ts...) const volatile &noexcept> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename... Ts>
struct FunctionTraits<R(Ts..., ...) &> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename... Ts>
struct FunctionTraits<R(Ts..., ...) const &> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename... Ts>
struct FunctionTraits<R(Ts..., ...) volatile &> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename... Ts>
struct FunctionTraits<R(Ts..., ...) &noexcept> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename... Ts>
struct FunctionTraits<R(Ts..., ...) const volatile &> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename... Ts>
struct FunctionTraits<R(Ts..., ...) const &noexcept> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename... Ts>
struct FunctionTraits<R(Ts..., ...) volatile &noexcept> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename... Ts>
struct FunctionTraits<R(Ts..., ...) const volatile &noexcept> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename... Ts>
struct FunctionTraits<R(Ts...) &&> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename... Ts>
struct FunctionTraits<R(Ts...) const &&> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename... Ts>
struct FunctionTraits<R(Ts...) volatile &&> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename... Ts>
struct FunctionTraits<R(Ts...) &&noexcept> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename... Ts>
struct FunctionTraits<R(Ts...) const volatile &&> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename... Ts>
struct FunctionTraits<R(Ts...) const &&noexcept> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename... Ts>
struct FunctionTraits<R(Ts...) volatile &&noexcept> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename... Ts>
struct FunctionTraits<R(Ts...) const volatile &&noexcept> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename... Ts>
struct FunctionTraits<R(Ts..., ...) &&> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename... Ts>
struct FunctionTraits<R(Ts..., ...) const &&> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename... Ts>
struct FunctionTraits<R(Ts..., ...) volatile &&> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename... Ts>
struct FunctionTraits<R(Ts..., ...) &&noexcept> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename... Ts>
struct FunctionTraits<R(Ts..., ...) const volatile &&> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename... Ts>
struct FunctionTraits<R(Ts..., ...) const &&noexcept> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename... Ts>
struct FunctionTraits<R(Ts..., ...) volatile &&noexcept> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename... Ts>
struct FunctionTraits<R(Ts..., ...) const volatile &&noexcept> : FunctionTraitsBase<R, Ts...> {};

// Function Reference
template <typename R, typename... Ts>
struct FunctionTraits<R (&)(Ts...)> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename... Ts>
struct FunctionTraits<R (&)(Ts...) noexcept> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename... Ts>
struct FunctionTraits<R (&)(Ts..., ...)> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename... Ts>
struct FunctionTraits<R (&)(Ts..., ...) noexcept> : FunctionTraitsBase<R, Ts...> {};

// Function Pointer
template <typename R, typename... Ts>
struct FunctionTraits<R (*)(Ts...)> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename... Ts>
struct FunctionTraits<R (*)(Ts...) noexcept> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename... Ts>
struct FunctionTraits<R (*)(Ts..., ...)> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename... Ts>
struct FunctionTraits<R (*)(Ts..., ...) noexcept> : FunctionTraitsBase<R, Ts...> {};

// Member function pointer
template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*)(Ts...)> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*)(Ts...) const> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*)(Ts...) volatile> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*)(Ts...) noexcept> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*)(Ts...) const volatile> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*)(Ts...) const noexcept> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*)(Ts...) volatile noexcept> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*)(Ts...) const volatile noexcept> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*)(Ts..., ...)> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*)(Ts..., ...) const> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*)(Ts..., ...) volatile> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*)(Ts..., ...) noexcept> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*)(Ts..., ...) const volatile> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*)(Ts..., ...) const noexcept> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*)(Ts..., ...) volatile noexcept> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*)(Ts..., ...) const volatile noexcept> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*)(Ts...) &> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*)(Ts...) const &> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*)(Ts...) volatile &> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*)(Ts...) &noexcept> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*)(Ts...) const volatile &> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*)(Ts...) const &noexcept> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*)(Ts...) volatile &noexcept> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*)(Ts...) const volatile &noexcept> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*)(Ts..., ...) &> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*)(Ts..., ...) const &> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*)(Ts..., ...) volatile &> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*)(Ts..., ...) &noexcept> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*)(Ts..., ...) const volatile &> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*)(Ts..., ...) const &noexcept> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*)(Ts..., ...) volatile &noexcept> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*)(Ts..., ...) const volatile &noexcept> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*)(Ts...) &&> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*)(Ts...) const &&> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*)(Ts...) volatile &&> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*)(Ts...) &&noexcept> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*)(Ts...) const volatile &&> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*)(Ts...) const &&noexcept> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*)(Ts...) volatile &&noexcept> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*)(Ts...) const volatile &&noexcept> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*)(Ts..., ...) &&> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*)(Ts..., ...) const &&> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*)(Ts..., ...) volatile &&> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*)(Ts..., ...) &&noexcept> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*)(Ts..., ...) const volatile &&> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*)(Ts..., ...) const &&noexcept> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*)(Ts..., ...) volatile &&noexcept> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*)(Ts..., ...) const volatile &&noexcept> : FunctionTraitsBase<R, C &, Ts...> {};

// Member function pointer reference
template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*&)(Ts...)> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*&)(Ts...) const> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*&)(Ts...) volatile> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*&)(Ts...) noexcept> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*&)(Ts...) const volatile> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*&)(Ts...) const noexcept> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*&)(Ts...) volatile noexcept> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*&)(Ts...) const volatile noexcept> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*&)(Ts..., ...)> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*&)(Ts..., ...) const> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*&)(Ts..., ...) volatile> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*&)(Ts..., ...) noexcept> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*&)(Ts..., ...) const volatile> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*&)(Ts..., ...) const noexcept> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*&)(Ts..., ...) volatile noexcept> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*&)(Ts..., ...) const volatile noexcept> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*&)(Ts...) &> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*&)(Ts...) const &> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*&)(Ts...) volatile &> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*&)(Ts...) &noexcept> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*&)(Ts...) const volatile &> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*&)(Ts...) const &noexcept> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*&)(Ts...) volatile &noexcept> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*&)(Ts...) const volatile &noexcept> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*&)(Ts..., ...) &> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*&)(Ts..., ...) const &> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*&)(Ts..., ...) volatile &> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*&)(Ts..., ...) &noexcept> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*&)(Ts..., ...) const volatile &> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*&)(Ts..., ...) const &noexcept> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*&)(Ts..., ...) volatile &noexcept> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*&)(Ts..., ...) const volatile &noexcept> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*&)(Ts...) &&> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*&)(Ts...) const &&> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*&)(Ts...) volatile &&> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*&)(Ts...) &&noexcept> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*&)(Ts...) const volatile &&> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*&)(Ts...) const &&noexcept> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*&)(Ts...) volatile &&noexcept> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*&)(Ts...) const volatile &&noexcept> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*&)(Ts..., ...) &&> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*&)(Ts..., ...) const &&> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*&)(Ts..., ...) volatile &&> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*&)(Ts..., ...) &&noexcept> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*&)(Ts..., ...) const volatile &&> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*&)(Ts..., ...) const &&noexcept> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*&)(Ts..., ...) volatile &&noexcept> : FunctionTraitsBase<R, C &, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctionTraits<R (C::*&)(Ts..., ...) const volatile &&noexcept> : FunctionTraitsBase<R, C &, Ts...> {};

// Member object pointer
template <typename D, typename T>
struct FunctionTraits<D T::*> {
  using ResultType = std::add_lvalue_reference_t<D>;
  using InputTypes = std::tuple<T const &>;
  static constexpr std::size_t InputCount = 1;
  using class_type = T;
  using invoke_type = T const &;
  using type = D T::*;
  using function_type = std::add_lvalue_reference_t<D>(invoke_type);
  using qualified_function_type = D(invoke_type);

  template <typename C>
  using apply_member_pointer = D C::*;

  template <typename R>
  using apply_return = R T::*;
};

// std::function specializations
template <typename R, typename... Ts>
struct FunctionTraits<std::function<R(Ts...)>> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename... Ts>
struct FunctionTraits<const std::function<R(Ts...)>> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename... Ts>
struct FunctionTraits<volatile std::function<R(Ts...)>> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename... Ts>
struct FunctionTraits<const volatile std::function<R(Ts...)>> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename... Ts>
struct FunctionTraits<std::function<R(Ts...)> &> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename... Ts>
struct FunctionTraits<const std::function<R(Ts...)> &> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename... Ts>
struct FunctionTraits<volatile std::function<R(Ts...)> &> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename... Ts>
struct FunctionTraits<const volatile std::function<R(Ts...)> &> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename... Ts>
struct FunctionTraits<std::function<R(Ts...)> &&> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename... Ts>
struct FunctionTraits<const std::function<R(Ts...)> &&> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename... Ts>
struct FunctionTraits<volatile std::function<R(Ts...)> &&> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename... Ts>
struct FunctionTraits<const volatile std::function<R(Ts...)> &&> : FunctionTraitsBase<R, Ts...> {};

// std::mem_fn specializations
template <typename T>
struct FunctionTraits<std::_Mem_fn<T>> : FunctionTraits<T> {};

template <typename T>
struct FunctionTraits<const std::_Mem_fn<T>> : FunctionTraits<T> {};

template <typename T>
struct FunctionTraits<volatile std::_Mem_fn<T>> : FunctionTraits<T> {};

template <typename T>
struct FunctionTraits<const volatile std::_Mem_fn<T>> : FunctionTraits<T> {};

template <typename T>
struct FunctionTraits<std::_Mem_fn<T> &> : FunctionTraits<T> {};

template <typename T>
struct FunctionTraits<const std::_Mem_fn<T> &> : FunctionTraits<T> {};

template <typename T>
struct FunctionTraits<volatile std::_Mem_fn<T> &> : FunctionTraits<T> {};

template <typename T>
struct FunctionTraits<const volatile std::_Mem_fn<T> &> : FunctionTraits<T> {};

template <typename T>
struct FunctionTraits<std::_Mem_fn<T> &&> : FunctionTraits<T> {};

template <typename T>
struct FunctionTraits<const std::_Mem_fn<T> &&> : FunctionTraits<T> {};

template <typename T>
struct FunctionTraits<volatile std::_Mem_fn<T> &&> : FunctionTraits<T> {};

template <typename T>
struct FunctionTraits<const volatile std::_Mem_fn<T> &&> : FunctionTraits<T> {};

/**
 * @brief: Concept that determines whether a type is a functor (if a type implements a call operator)
 *
 * @tparam T Type to check for an implementation of a call operator
 *
 * @note This concept only checks for the presence of a call operator; it does not constrain the parameters or return
 * type of the operator.
 *
 * The "Functor" concept is satisfied by any type "T" that implements a call operator.
 */
template <typename T>
concept functor = requires() {
  &T::operator();
};

/**
 * @brief A trait structure for extract type information of functors.
 *
 * @tparam F Functor-like type from which the information needs to be extracted.
 *
 * This template struct provides a mechanism to extract information about functors
 * (i.e., callable objects) such as their return type and argument types
 *
 * @note This specialization is required for functors because, for functors, there's no need to pass the object (e.g.,
 * the class instance) as one of the arguments in the operator() call. The functor itself serves as the object being
 * invoked. For example, a lambda is a functor, and we don’t need to explicitly pass the lambda to its operator().
 * Without this specialization, the behavior would be incorrect, as using "FunctionTraits" would expect an additional
 * input argument.
 */
template <typename F>
struct FunctorTraits;

// Member function pointer
template <typename R, typename C, typename... Ts>
struct FunctorTraits<R (C::*)(Ts...)> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctorTraits<R (C::*)(Ts...) const> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctorTraits<R (C::*)(Ts...) volatile> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctorTraits<R (C::*)(Ts...) noexcept> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctorTraits<R (C::*)(Ts...) const volatile> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctorTraits<R (C::*)(Ts...) const noexcept> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctorTraits<R (C::*)(Ts...) volatile noexcept> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctorTraits<R (C::*)(Ts...) const volatile noexcept> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctorTraits<R (C::*)(Ts..., ...)> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctorTraits<R (C::*)(Ts..., ...) const> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctorTraits<R (C::*)(Ts..., ...) volatile> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctorTraits<R (C::*)(Ts..., ...) noexcept> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctorTraits<R (C::*)(Ts..., ...) const volatile> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctorTraits<R (C::*)(Ts..., ...) const noexcept> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctorTraits<R (C::*)(Ts..., ...) volatile noexcept> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctorTraits<R (C::*)(Ts..., ...) const volatile noexcept> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctorTraits<R (C::*)(Ts...) &> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctorTraits<R (C::*)(Ts...) const &> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctorTraits<R (C::*)(Ts...) volatile &> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctorTraits<R (C::*)(Ts...) &noexcept> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctorTraits<R (C::*)(Ts...) const volatile &> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctorTraits<R (C::*)(Ts...) const &noexcept> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctorTraits<R (C::*)(Ts...) volatile &noexcept> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctorTraits<R (C::*)(Ts...) const volatile &noexcept> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctorTraits<R (C::*)(Ts..., ...) &> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctorTraits<R (C::*)(Ts..., ...) const &> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctorTraits<R (C::*)(Ts..., ...) volatile &> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctorTraits<R (C::*)(Ts..., ...) &noexcept> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctorTraits<R (C::*)(Ts..., ...) const volatile &> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctorTraits<R (C::*)(Ts..., ...) const &noexcept> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctorTraits<R (C::*)(Ts..., ...) volatile &noexcept> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctorTraits<R (C::*)(Ts..., ...) const volatile &noexcept> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctorTraits<R (C::*)(Ts...) &&> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctorTraits<R (C::*)(Ts...) const &&> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctorTraits<R (C::*)(Ts...) volatile &&> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctorTraits<R (C::*)(Ts...) &&noexcept> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctorTraits<R (C::*)(Ts...) const volatile &&> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctorTraits<R (C::*)(Ts...) const &&noexcept> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctorTraits<R (C::*)(Ts...) volatile &&noexcept> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctorTraits<R (C::*)(Ts...) const volatile &&noexcept> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctorTraits<R (C::*)(Ts..., ...) &&> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctorTraits<R (C::*)(Ts..., ...) const &&> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctorTraits<R (C::*)(Ts..., ...) volatile &&> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctorTraits<R (C::*)(Ts..., ...) &&noexcept> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctorTraits<R (C::*)(Ts..., ...) const volatile &&> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctorTraits<R (C::*)(Ts..., ...) const &&noexcept> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctorTraits<R (C::*)(Ts..., ...) volatile &&noexcept> : FunctionTraitsBase<R, Ts...> {};

template <typename R, typename C, typename... Ts>
struct FunctorTraits<R (C::*)(Ts..., ...) const volatile &&noexcept> : FunctionTraitsBase<R, Ts...> {};

// Functor
template <functor F>
struct FunctionTraits<F> : FunctorTraits<decltype(&F::operator())> {};

template <functor F>
struct FunctionTraits<const F> : FunctorTraits<decltype(&F::operator())> {};

template <functor F>
struct FunctionTraits<volatile F> : FunctorTraits<decltype(&F::operator())> {};

template <functor F>
struct FunctionTraits<const volatile F> : FunctorTraits<decltype(&F::operator())> {};

template <functor F>
struct FunctionTraits<F &> : FunctorTraits<decltype(&F::operator())> {};

template <functor F>
struct FunctionTraits<const F &> : FunctorTraits<decltype(&F::operator())> {};

template <functor F>
struct FunctionTraits<volatile F &> : FunctorTraits<decltype(&F::operator())> {};

template <functor F>
struct FunctionTraits<const volatile F &> : FunctorTraits<decltype(&F::operator())> {};

template <functor F>
struct FunctionTraits<F &&> : FunctorTraits<decltype(&F::operator())> {};

template <functor F>
struct FunctionTraits<const F &&> : FunctorTraits<decltype(&F::operator())> {};

template <functor F>
struct FunctionTraits<volatile F &&> : FunctorTraits<decltype(&F::operator())> {};

template <functor F>
struct FunctionTraits<const volatile F &&> : FunctorTraits<decltype(&F::operator())> {};

}  // namespace lazy
