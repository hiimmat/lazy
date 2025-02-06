#pragma once

#if defined(LAZY_ENABLE_EXPECTS) || defined(LAZY_ENABLE_ENSURES)

#if defined(__clang__) || defined(__GNUC__)
#define LAZY_LIKELY(x) __builtin_expect(!!(x), 1)
#else
#define LAZY_LIKELY(x) (!!(x))
#endif

#if defined(LAZY_THROW_ON_CONTRACT_VIOLATION)
#include <stdexcept>

namespace lazy {

template <typename Exception>
[[noreturn]] void throw_exception(Exception&& exception) {
  throw std::forward<Exception>(exception);
}

}  // namespace lazy

#define LAZY_STRINGIFY_DETAIL(x) #x
#define LAZY_STRINGIFY(x) LAZY_STRINGIFY_DETAIL(x)

#define LAZY_CONTRACT_CHECK(type, cond) \
  (LAZY_LIKELY(cond)                    \
       ? static_cast<void>(0)           \
       : lazy::throw_exception(std::logic_error("NT: " type " failure at " __FILE__ ": " LAZY_STRINGIFY(__LINE__))))
#else
#include <exception>
#define LAZY_CONTRACT_CHECK(type, cond) (LAZY_LIKELY(cond) ? static_cast<void>(0) : std::terminate())
#endif

#ifdef LAZY_ENABLE_EXPECTS
#define Expects(cond) LAZY_CONTRACT_CHECK("Precondition", cond)
#endif

#ifdef LAZY_ENABLE_ENSURES
#define Ensures(cond) LAZY_CONTRACT_CHECK("Postcondition", cond)
#endif

#endif
