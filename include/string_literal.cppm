module;

#include <algorithm>
#include <array>
#include <cstring>
#include <string_view>
#include <tuple>

#include "assert.hpp"

export module Lazy.StringLiteral;

export namespace lazy {

/**
 * @brief Compile-time string literal with a fixed, non-negative length.
 *
 * @tparam N The length of the string literal. Must be a non-negative integer.
 *
 * This struct represents a compile-time string literal with a non-negative length and provides various operations for
 * compile-time string access and manipulation.
 */
template <long long int N>
requires(N >= 0) struct string_literal {
  /**
   * @brief Default constructor.
   *
   * Default constructor for the "string_literal" class.
   */
  [[nodiscard]] constexpr string_literal() noexcept = default;

  /**
   * @brief Constructs a "string_literal" from a const char array.
   *
   * @param str A reference to a const char array of size "N" used to initialize the string.
   *
   * Constructs a "string_literal" instance by copying the contents of a const char array.
   */
  [[nodiscard]] constexpr string_literal(const char (&str)[N]) noexcept requires(N > 0) {
    std::copy_n(str, N, arr.begin());
  }

  /**
   * @brief Constructs a "string_literal" from a char array.
   *
   * @param str A char array of size "N" used to initialize the string.
   *
   * Constructs a "string_literal" instance by copying the contents of a char array.
   */
  [[nodiscard]] constexpr explicit string_literal(char(str)[N]) noexcept requires(N > 0) {
    std::copy_n(str, N, arr.begin());
  }

  /**
   * @brief Returns a reference to the character at specified location.
   *
   * @param i Index of the character to access.
   *
   * @return Reference to the character at the specified index.
   *
   * Returns a reference to the character at specified location. No bounds checking is performed.
   */
  [[nodiscard]] constexpr char& operator[](long long int i) noexcept { return arr[i]; }

  /**
   * @brief Returns a const reference to the character at specified location.
   *
   * @param i Index of the character to access.
   *
   * @return Const reference to the character at the specified index.
   *
   * Returns a const reference to the character at specified location. No bounds checking is performed.
   */
  [[nodiscard]] constexpr const char& operator[](long long int i) const noexcept { return arr[i]; }

  /**
   * @brief Provides a pointer to the underlying array.
   *
   * @return Pointer to the first character in the string literal.
   *
   * This function returns a pointer to the first character of the underlying array.
   */
  [[nodiscard]] constexpr char* data() noexcept { return arr.data(); }

  /**
   * @brief Provides a const pointer to the underlying array.
   *
   * @return Const pointer to the first character in the string literal.
   *
   * This function returns a const pointer to the first character of the underlying array.
   */
  [[nodiscard]] constexpr char const* data() const noexcept { return arr.data(); }

  /**
   * @brief Returns an iterator to the first character of the string literal.
   *
   * @return Iterator to the first character in the string literal.
   *
   * This function returns an iterator to the first character of the string literal.
   */
  [[nodiscard]] constexpr char* begin() noexcept { return data(); }

  /**
   * @brief Returns an iterator to the last character of the string literal.
   *
   * @return Iterator to the last character in the string literal.
   *
   * This function returns an iterator to the last character of the string literal.
   */
  [[nodiscard]] constexpr char* end() noexcept { return data() + size(); }

  /**
   * @brief Returns a const iterator to the first character of the string literal.
   *
   * @return Const iterator to the first character in the string literal.
   *
   * This function returns a const iterator to the first character of the string literal.
   */
  [[nodiscard]] constexpr const char* begin() const noexcept { return data(); }

  /**
   * @brief Returns a const iterator to the last character of the string literal.
   *
   *  @return Const iterator to the last character in the string literal.
   *
   * This function returns a const iterator to the last character of the string literal.
   */
  [[nodiscard]] constexpr const char* end() const noexcept { return data() + size(); }

  /**
   * @brief Implicitly converts the string literal to a char pointer.
   *
   *  @return A pointer to the first character in the string literal.
   *
   * This conversion operator provides a pointer to the beginning of the underlying char array.
   */
  [[nodiscard]] constexpr explicit operator char*() noexcept { return data(); }

  /**
   * @brief Implicitly converts the string literal to a const char pointer.
   *
   * @return A const pointer to the first character in the string literal.
   *
   * This conversion operator provides a const pointer to the beginning of the underlying char array.
   */
  [[nodiscard]] constexpr explicit operator const char*() const noexcept { return data(); }

  /**
   * @brief Compares two string literal objects for equality.
   *
   * This method compares the current string literal with another string literal
   * object to check if they are equal.
   *
   * @tparam sz The size of the string literal
   *
   * @param other The string literal object to compare against the current object.
   *
   * @return true if the compared string literals are equal (same size and content), false otherwise.
   */
  template <long long int sz>
  [[nodiscard]] bool operator==(const string_literal<sz>& other) const noexcept {
    if (this->size() != other.size()) {
      return false;
    }

    return std::memcmp(this->arr.begin(), other.arr.begin(), this->size()) == 0;
  }

  /**
   * @brief Returns the first character in the string literal.
   *
   * @return The first character in the string literal.
   *
   * This function provides access to the first character in the string literal.
   */
  [[nodiscard]] constexpr char front() const noexcept { return *begin(); }

  /**
   * @brief Returns the last character in the string literal.
   *
   * @return The last character in the string literal.
   *
   * This function provides access to the last character in the string literal.
   */
  [[nodiscard]] constexpr char back() const noexcept { return *(end() - 1); }

  /**
   * @brief Returns a substring from the string literal.
   *
   * @param start Index of the first character to include.
   * @param count The number of characters to include in the substring. If set to "-1", the substring will extend to the
   * end of the string literal.
   *
   * @return A new "string_literal" object containing the extracted substring.
   *
   * This function creates a new "string_literal" object containing a substring of the original string,
   * starting from the specified position and extending for a specified length. If the length is set to "-1", the
   * substring extends to the end of the string literal.
   */
  [[nodiscard]] constexpr auto substr(long long int start = 0, long long int count = -1) const noexcept {
#ifdef LAZY_ENABLE_EXPECTS
    Expects(start >= 0 && start <= N);
    Expects(count >= -1 && count <= N);
#endif
    const long long int substr_size{count == -1 ? size() - start : count};

#ifdef LAZY_ENABLE_EXPECTS
    Expects(start + substr_size <= N);
#endif

    const long long int sz = start + substr_size;

    string_literal<N> substr;

    long long int substr_idx{0};

    for (long long int arr_idx{start}; arr_idx < sz; ++arr_idx, ++substr_idx) {
      substr[substr_idx] = arr[arr_idx];
    }

    for (; substr_idx < N; ++substr_idx) {
      substr[substr_idx] = '\0';
    }

    return substr;
  }

  /**
   * @brief Returns the length of the string literal.
   *
   * @return The number of characters in the string literal, excluding the null terminator.
   *
   * This function returns the number of char elements in the string literal.
   */
  [[nodiscard]] constexpr long long int size() const noexcept {
    long long int idx{0};

    while (idx < N && arr[idx] != '\0') {
      ++idx;
    }

    return idx;
  }

  /**
   * @brief Checks whether the string literal is empty.
   *
   * @return "true" if the string literal is empty, "false" otherwise.
   *
   * This function returns "true" if the string literal has no characters, and "false" otherwise.
   */
  [[nodiscard]] constexpr bool empty() const noexcept { return size() == 0; }

  /**
   * @brief Converts the string literal to a "std::string_view".
   * @return A "std::string_view" representing the string literal's content.
   *
   * This function provides a "std::string_view" representation of the string literal.
   */
  [[nodiscard]] constexpr std::string_view to_string_view() const noexcept {
    return std::string_view(arr.data(), size());
  }

  // internal string_literal storage
  std::array<char, N> arr{};
};

/**
 * @brief User-defined literal for converting a "string_literal" to a "const char*".
 *
 * @tparam str A compile-time "string_literal".
 *
 * @return A "const char*" pointing to the data of the "string_literal" object.
 *
 * This function is a user-defined literal operator that converts a compile-time "string_literal" into a "const char*".
 */
template <string_literal str>
[[nodiscard]] consteval auto operator""_sl() noexcept {
  return str.data();
}

/**
 * @brief Removes all characters from a string literal satisfying a specific criteria.
 *
 * @tparam N The length of the "string_literal".
 *
 * @param str The string_literal object to be filtered.
 * @param predicate A unary predicate function or function object. The predicate is applied to each character of the
 * string_literal, and if the result is "false", the character is kept in the output string.
 *
 * @return A new "string_literal<N>" object with characters that do not satisfy the predicate.
 *
 * This function iterates over the characters of the given "string_literal" object and checks each character using the
 * predicate. If the predicate returns "false", the character is included in the output string literal, otherwise it is
 * discarded.
 */
template <long long int N>
[[nodiscard]] constexpr string_literal<N> filter_if(const string_literal<N>& str,
                                                    const std::predicate<char> auto& predicate) noexcept {
  string_literal<N> output_str;
  long long int idx{0};

  for (long long int i{0}; i < N; ++i) {
    if (str[i] != '\0' && !predicate(str[i])) {
      output_str[idx] = str[i];
      ++idx;
    }
  }

  for (; idx < N; ++idx) {
    output_str[idx] = '\0';
  }

  return output_str;
}

/**
 * @brief Removes all occurrences of a specific character from a string literal.
 *
 * @tparam N The length of the string_literal.
 *
 * @param str The string_literal object to be filtered.
 * @param c The character to be removed from the string literal.
 *
 * @return A new "string_literal<N>" object with all occurrences of the character "c" removed.
 *
 * This function filters the input string_literal by removing all occurrences of the specified character from it.
 */
template <long long int N>
[[nodiscard]] constexpr string_literal<N> filter(const string_literal<N>& str, char c) noexcept {
  return filter_if(str, [c](char _c) { return _c == c; });
}

/**
 * @brief Counts the number of characters in a string literal that satisfy the given predicate.
 *
 * @tparam N The length of the string_literal.
 *
 * @param str The string_literal to examine.
 * @param predicate A unary predicate function or function object that returns a true if the character should be
 * counted, false otherwise.
 *
 * @return The number of characters in the string_literal that satisfy the given predicate.
 *
 * This function iterates over the characters in the given string_literal and counts the number of characters that
 * satisfy the given predicate.
 */
template <long long int N>
[[nodiscard]] constexpr long long int count_if(const string_literal<N>& str,
                                               std::predicate<char> auto&& predicate) noexcept {
  long long int cnt{0};

  for (auto c : str) {
    if (predicate(c)) {
      ++cnt;
    }
  }

  return cnt;
}

/**
 * @brief Counts the number of occurrences of a specific character in a string literal.
 *
 * @tparam N The length of the string_literal.
 *
 * @param str The string_literal to examine.
 * @param c The character whose occurrences are to be counted in the string literal.
 *
 * @return The number of occurrences of the character "c" in the string_literal "str".
 *
 * This function counts how many times a specific character "c" appears in the input string_literal "str".
 */
template <long long int N>
[[nodiscard]] constexpr long long int count(const string_literal<N>& str, char c) noexcept {
  return count_if(str, [c](char _c) { return _c == c; });
}

/**
 * @brief Finds the starting position of a substring within a string literal.
 *
 * @tparam N The length of the string literal "str".
 * @tparam M The length of the substring literal "substr".
 *
 * @param str The string literal in which to search for the substring "substr".
 * @param substr The substring that is searched for in the string literal "str".
 * @param start The starting position in the string from where the search begins.
 * @param count The number of characters in the string to consider for the search.
 *
 * @return The starting index of the first occurrence of substr within "str"  or "-1" if the substring is not found.
 *
 * This function searches for the first occurrence of a given substring "substr" within a specified range of the input
 * string literal "str". The search starts from a specified position "start" and considers a given number of characters
 * "count". If the substring is found within the specified range, the function returns the starting index of the
 * substring. Otherwise, it returns "-1" to indicate that the substring was not found.
 */
template <long long int N, long long int M>
[[nodiscard]] constexpr long long int find(const string_literal<N>& str, const string_literal<M>& substr,
                                           long long int start, long long int count) noexcept {
  static_assert(N >= M);

#ifdef LAZY_ENABLE_EXPECTS
  Expects(start >= 0 || start <= N);
  Expects(count >= 0 && count <= N);
  Expects(start + count <= str.size());
#endif

  if (str.empty() || substr.empty()) {
    return -1;
  }

  const long long int search_size{start + count};
  long long int substr_start{0};
  long long int pos{0};
  bool found{false};

  for (long long int i{start}; i < search_size; ++i) {
    if (pos == substr.size()) {
      return substr_start;
    }

    if (search_size - i < substr.size() - pos) {
      return -1;
    }

    if (str[i] == substr[pos]) {
      found ? 0 : (substr_start = i, found = true);
      ++pos;
    } else {
      pos = 0;
      found = false;
    }
  }

  return found ? substr_start : -1;
}

/**
 * @brief Finds the starting position of a substring within a string literal, with an optional starting index.
 *
 * @tparam N The length of the string literal "str".
 * @tparam M The length of the substring literal "substr".
 *
 * @param str The string literal in which to search for the substring "substr".
 * @param substr The substring that is searched for in the string literal "str".
 * @param start The starting position in the string from where the search begins (default is 0).
 *
 * @return The starting index of the first occurrence of substr within "str" or "-1" if the substring is not found.
 *
 * This function searches for the first occurrence of a given substring "substr" within a specified range of the input
 * string literal "str". The search starts from a specified position "start" and considers the entire remainder of the
 * string after that position. If the substring is found, the function returns the starting index of the substring.
 * Otherwise, it returns "-1" to indicate that the substring was not found.
 */
template <long long int N, long long int M>
[[nodiscard]] constexpr long long int find(const string_literal<N>& str, const string_literal<M>& substr,
                                           long long int start = 0) noexcept {
  static_assert(N >= M);

#ifdef LAZY_ENABLE_EXPECTS
  Expects(start >= 0 && start <= N);
#endif

  return find(str, substr, start, str.size() - start);
}

/**
 * @brief Replaces the first occurrence of a substring with a new substring within a string literal.
 *
 * @tparam N The length of the input string literal "str".
 * @tparam M The length of the old substring "old_substr".
 * @tparam O The length of the new substring "new_substr".
 *
 * @param str The string literal in which to search for and replace the string_literal "old_substr".
 * @param old_substr The substring that needs to be replaced with the string_literal "new_substr".
 * @param new_substr The substring that will replace the string_literal "old_substr" in "str".
 *
 * @return A new string literal with the first occurrence of "old_substr" replaced with "new_substr".
 *
 * This function searches for the first occurrence of a specified substring "old_substr" within the
 * input string literal ("str") and replaces it with a new substring ("new_substr"). If the substring
 * is found, the function returns a new string literal where the substring has been replaced. If the
 * substring is not found, an exception is thrown.
 */
template <long long int N, long long int M, long long int O>
[[nodiscard]] constexpr auto replace(const string_literal<N>& str, const string_literal<M>& old_substr,
                                     const string_literal<O>& new_substr) noexcept {
  static_assert(N >= M);

  long long int pos{find(str, old_substr)};

#ifdef LAZY_ENABLE_EXPECTS
  Expects(pos >= 0);
#endif

  string_literal<N - M + O> output_str;

  long long int i{0};

  for (; i < pos; ++i) {
    output_str[i] = str[i];
  }

  for (i = 0; i < new_substr.size(); ++i) {
    output_str[pos + i] = new_substr[i];
  }

  for (i = pos + old_substr.size(); i < str.size(); ++i) {
    output_str[i - old_substr.size() + new_substr.size()] = str[i];
  }

  return output_str;
}

/**
 * @brief Sorts the characters in a string literal in ascending order.
 *
 * @tparam N The length of the input string literal "str".
 *
 * @param str The string literal to be sorted.
 *
 * @return A new string literal with the characters of "str" sorted in ascending order.
 *
 * This function creates a copy of the string_literal "str" with its characters sorted in ascending order.
 */
template <long long int N>
[[nodiscard]] constexpr string_literal<N> sort(string_literal<N> str) noexcept {
  string_literal<N> out{str};

  for (long long int i = 0; i < str.size() - 1; ++i) {
    for (long long int j = 0; j < str.size() - i - 1; ++j) {
      if (out[j] > out[j + 1]) {
        char tmp{out[j]};
        out[j] = out[j + 1];
        out[j + 1] = tmp;
      }
    }
  }

  return out;
}

/**
 * @brief Concatenates a pack of string literals into a single string literal.
 *
 * @tparam strings A pack of string literals to be concatenated.
 *
 * @return A new string literal containing all input string literals concatenated together.
 *
 * This function takes multiple string literals and concatenates them into a single string literal. It returns a new
 * string literal containing the concatenated characters from all the input string literals.
 */
template <string_literal... strings>
[[nodiscard]] constexpr auto concatenate() noexcept {
  if constexpr (constexpr std::size_t num_inputs{sizeof...(strings)}; num_inputs == 1) {
    return (strings, ...);
  } else {
    constexpr std::size_t sz = (0 + ... + strings.size());
    string_literal<sz> output_str;
    std::size_t pos{0};

    auto concatenate_string = [&]<std::size_t idx>() {
      for (constexpr auto str = std::get<idx>(std::forward_as_tuple(strings...)); auto c : str) {
        output_str[pos++] = c;
      }
    };

    [&]<std::size_t... is>(std::index_sequence<is...>) { (concatenate_string.template operator()<is>(), ...); }
    (std::make_index_sequence<num_inputs>());

    return output_str;
  }
}

/**
 * @brief Joins multiple string literals separated by a specified delimiter.
 *
 * @tparam delimiter A string literal used as the delimiter to insert between each pair of adjacent strings.
 * @tparam strings A pack of string literals to be joined.
 *
 * @return A new string literal containing the concatenated result of all input string literals, with the delimiter
 * inserted between them.
 *
 * This function concatenates a pack of string literals, inserting the specified  delimiter between each pair of
 * adjacent strings.
 */
template <string_literal delimiter, string_literal... strings>
[[nodiscard]] constexpr auto join() noexcept {
  constexpr std::size_t num_inputs{sizeof...(strings)};

  if constexpr (num_inputs == 1) {
    return (strings, ...);
  } else {
    constexpr std::size_t sz = (((num_inputs - 1) * delimiter.size()) + ... + strings.size());
    string_literal<sz> output_str;
    std::size_t pos{0};

    auto concatenate_string = [&]<std::size_t idx>() {
      constexpr auto str = std::get<idx>(std::forward_as_tuple(strings...));

      if constexpr (idx > 0) {
        for (auto c : delimiter) {
          output_str[pos++] = c;
        }
      }

      for (auto c : str) {
        output_str[pos++] = c;
      }
    };

    [&]<std::size_t... is>(std::index_sequence<is...>) { (concatenate_string.template operator()<is>(), ...); }
    (std::make_index_sequence<num_inputs>());

    return output_str;
  }
}

}  // namespace lazy
