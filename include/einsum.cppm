module;

#include <algorithm>
#include <array>
#include <concepts>
#include <utility>

import Lazy.ArrayTraits;

export module Lazy.Einsum;
export import Lazy.StringLiteral;

namespace lazy {

/**
 * @brief Checks if a character is a valid einsum character.
 *
 * @param ch The character to check for validity.
 *
 * @return "true" if the character is valid according to Einsum notation, "false" otherwise.
 *
 * This function checks if a given character "ch" is a valid character in the context of Einsum notation.
 */
[[nodiscard]] consteval bool is_valid_einsum_char(char ch) noexcept {
  constexpr auto valid = []() constexpr {
    constexpr std::size_t size{127};
    std::array<bool, size> result{};

    for (std::size_t c = 0; c < 127; ++c) {
      result[c] = c == 'a' || c == 'b' || c == 'c' || c == 'd' || c == 'e' || c == 'f' || c == 'g' || c == 'h' ||
                  c == 'i' || c == 'j' || c == 'k' || c == 'l' || c == 'm' || c == 'n' || c == 'o' || c == 'p' ||
                  c == 'q' || c == 'r' || c == 's' || c == 't' || c == 'u' || c == 'v' || c == 'w' || c == 'x' ||
                  c == 'y' || c == 'z' || c == 'A' || c == 'B' || c == 'C' || c == 'D' || c == 'E' || c == 'F' ||
                  c == 'G' || c == 'H' || c == 'I' || c == 'J' || c == 'K' || c == 'L' || c == 'M' || c == 'N' ||
                  c == 'O' || c == 'P' || c == 'Q' || c == 'R' || c == 'S' || c == 'T' || c == 'U' || c == 'V' ||
                  c == 'W' || c == 'X' || c == 'Y' || c == 'Z' || c == ',' || c == '-' || c == '>' || c == '.';
    }

    return result;
  }
  ();

  return ch < 127 && valid[ch];
}

/**
 * @brief Checks if all characters in a given string_literal are valid einsum characters.
 *
 * @tparam subscripts The string_literal whose characters need to be checked.
 *
 * @return "true" if all characters in the "subscripts" string_literal are valid einsum characters, "false" otherwise.
 *
 * This function checks each character in the provided "subscripts" string_literal to ensure that it is a valid
 * character for Einsum notation.
 */
template <string_literal subscripts>
[[nodiscard]] consteval bool has_valid_einsum_chars_only() noexcept {
  for (auto c : subscripts) {
    if (!is_valid_einsum_char(c)) {
      return false;
    }
  }
  return true;
}

/**
 * @brief Get the symbol corresponding to "i".
 *
 * @param i The index of the symbol to be retrieved.
 *
 * @return "true" if all characters in the "subscripts" string_literal are valid einsum characters, "false" otherwise.
 *
 * Retrieves the symbol corresponding to "i". Runs through the usual 52 letters before resorting to Unicode characters,
 * starting at char(192).
 */
[[nodiscard]] consteval char get_symbol(int i) noexcept {
  if (i < 52) {
    constexpr std::array<char, 52> einsum_symbols{'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
                                                  'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
                                                  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
                                                  'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};
    return einsum_symbols[i];
  } else if (i >= 55296)
    // Skip chr(57343) - chr(55296) as surrogates
    return static_cast<char>(i + 2048);
  else
    return static_cast<char>(i + 140);
}

/**
 * @brief Generates a list of unused symbols based on a set of used symbols and a filtering predicate.
 *
 * @tparam size The number of unused symbols to generate.
 * @tparam used_symbols The string literal containing the symbols that are already in use.
 * @param ignore A predicate function that returns "true" for symbols that needs to be ignored (excluded).
 *
 * @return An array of unused symbols whose length is equal to "size".
 *
 * This function generates a set of unused symbols by iterating through a sequence of symbols and excluding those that
 * are present in a given set of "used_symbols", or those that satisfy a filtering condition specified by the "ignore"
 * predicate. It continues until the desired number of unused symbols ("size") is reached.
 */
template <int size, string_literal used_symbols>
[[nodiscard]] consteval auto generate_unused_symbols(std::predicate<char> auto ignore) noexcept {
  std::size_t cnt = 0;
  int i = 0;
  std::array<char, size> symbols;

  while (cnt < size) {
    char s = get_symbol(i);
    ++i;

    if (count(used_symbols, s) > 0 && !ignore(s)) {
      continue;
    }

    symbols[cnt] = s;
    ++cnt;
  }

  return symbols;
}

/**
 * @brief Finds the output string based on the einsum input indices
 *
 * @tparam subscripts The input subscripts used in Einstein summation for which the output string needs to be determined
 *
 * @return output string determined based on the input "subscripts"
 *
 * Find the output string for the inputs "subscripts" under canonical einstein summation rules. That is, repeated
 * indices are summed over by default.
 */
template <string_literal subscripts>
[[nodiscard]] consteval auto find_output_str() noexcept {
  constexpr auto tmp_subscripts = filter_if(subscripts, [](char c) { return c == ',' || count(subscripts, c) > 1; });
  return sort(tmp_subscripts);
}

/**
 * @brief Parses a substring of subscripts from an einsum string.
 *
 * @tparam str The string literal representing the subscripts to be parsed.
 * @tparam ellipse_inds The string literal containing indices representing ellipses.
 * @tparam ranks An array of ranks for each dimension.
 * @tparam current_rank The current obj_rank_v being parsed.
 * @tparam longest The length of the longest ellipsis encountered.
 *
 * @return A std::pair where:
 * - The first element is the parsed subscript string with ellipses replaced by indices.
 * - The second element is the length of the longest ellipsis encountered.
 *
 * This function is used to parse a substring of subscripts from an einsum string.
 */
template <string_literal str, string_literal ellipse_inds, std::array ranks, std::size_t current_rank, int longest>
[[nodiscard]] consteval auto parse_current_subscripts() {
  constexpr long long int comma_pos{find(str, string_literal(","))};
  constexpr bool contains_comma{comma_pos != -1};
  constexpr string_literal sub(contains_comma ? str.substr(0, comma_pos) : str);

  if constexpr (constexpr long long int dots_count{count(sub, '.')}; dots_count > 0) {
    static_assert(dots_count == 3 && find(sub, string_literal{"..."}) != -1, "Invalid ellipses");

    constexpr auto ellipse_count = std::max(ranks[current_rank], 1) - static_cast<int>(sub.size() - 3);

    static_assert(ellipse_count >= 0, "Ellipses lengths do not match");
    static_assert(ellipse_inds.size() > ellipse_count);

    if constexpr (ellipse_count == 0) {
      if constexpr (contains_comma) {
        constexpr auto res = parse_current_subscripts<str.substr(comma_pos + 1), ellipse_inds, ranks, current_rank + 1u,
                                                      (ellipse_count > longest ? ellipse_count : longest)>();
        return std::pair{join<string_literal(","), filter(sub, '.'), res.first>(),
                         longest > res.second ? longest : res.second};
      } else {
        return std::pair{filter(sub, '.'), ellipse_count > longest ? ellipse_count : longest};
      }
    } else {
      if constexpr (contains_comma) {
        constexpr auto res = parse_current_subscripts<str.substr(comma_pos + 1), ellipse_inds, ranks, current_rank + 1u,
                                                      (ellipse_count > longest ? ellipse_count : longest)>();
        return std::pair{
            join<string_literal(","),
                 replace(sub, string_literal{"..."}, ellipse_inds.substr(ellipse_inds.size() - ellipse_count)),
                 res.first>(),
            longest > res.second ? longest : res.second};
      } else {
        return std::pair{replace(sub, string_literal{"..."}, ellipse_inds.substr(ellipse_inds.size() - ellipse_count)),
                         ellipse_count > longest ? ellipse_count : longest};
      }
    }
  } else {
    return std::pair{sub, 0};
  }
};

/**
 * @brief Replaces a substring consisting of ellipses with valid einsum characters.
 *
 * @tparam subscripts A string literal representing subscripts in which the ellipses are searched for.
 * @tparam Operands Variadic template parameter representing the input operands for Einstein summation.
 *
 * @return A string literal containing the input subscripts in which the ellipses are replaced by generated indices.
 *
 * This function parses a subscripts string, replacing ellipses with valid einsum characters, and optionally generating
 * a valid output subscripts string.
 */
template <string_literal subscripts, typename... Operands>
[[nodiscard]] consteval auto parse_ellipses() noexcept {
  if constexpr (find(subscripts, string_literal{"."}) == -1) {
    return subscripts;
  }

  constexpr std::array<int, sizeof...(Operands)> ranks{obj_rank_v<Operands>...};
  constexpr auto ignore = [](char c) { return c == '.' || c == ',' || c == '-' || c == '>'; };
  constexpr auto ellipse_inds_arr =
      generate_unused_symbols<*std::max_element(ranks.begin(), ranks.end()), subscripts>(ignore);

  constexpr auto ellipse_inds = [ellipse_inds_arr]<std::size_t... is>(std::index_sequence<is...>) {
    constexpr char arr[ellipse_inds_arr.size()]{ellipse_inds_arr[is]...};
    return string_literal{arr};
  }
  (std::make_index_sequence<ellipse_inds_arr.size()>());

  constexpr long long int out_str_pos{find(subscripts, string_literal{"->"})};
  constexpr bool contains_out_str{out_str_pos != -1};
  constexpr auto input_str = subscripts.substr(0, out_str_pos);

  constexpr auto res = parse_current_subscripts<input_str, ellipse_inds, ranks, 0u, 0>();
  constexpr auto input_subscripts{res.first};
  constexpr auto longest{res.second};

  return [&]() {
    constexpr auto out_ellipse = [&]() {
      if constexpr (longest > 0) {
        return ellipse_inds.substr(ellipse_inds.size() - longest);
      } else {
        return string_literal<0>{};
      }
    }();

    if constexpr (contains_out_str) {
      constexpr auto output_str = replace(subscripts.substr(out_str_pos), string_literal{"..."}, out_ellipse);
      return concatenate<input_subscripts, output_str>();
    } else {
      constexpr auto output_str = find_output_str<input_subscripts>();
      constexpr auto normal_inds = filter_if(output_str, [out_ellipse](char c) { return count(out_ellipse, c) > 0; });
      return concatenate<input_subscripts, "->", out_ellipse, normal_inds>();
    }
  }();
}

/**
 * @brief Parses the input string for an Einstein summation notation (einsum) operation.
 *
 * @tparam subs A string literal representing the subscript string to be parsed.
 * @tparam Operands A variadic template representing the operand types.
 *
 * @return A string literal representing the final parsed and validated einsum string.
 *
 * This function takes a string of subscripts representing the operands in an Einstein summation expression and
 * validates its format according to the einsum notation. It performs various checks to ensure that the string is
 * correctly formed, handles ellipses, and if needed, computes the output string.
 */
export template <string_literal subs, typename... Operands>
[[nodiscard]] consteval auto parse_einsum_input() noexcept {
  constexpr std::size_t max_operands{64};

  static_assert(sizeof...(Operands) > 0 && sizeof...(Operands) < max_operands,
                "Number of operands must be greater than 0, and less than 64");

  static_assert(!std::all_of(subs.begin(), subs.end(), [](char c) { return c == ' '; }),
                "Subscripts cannot be represented exclusively using whitespace");

  static_assert(!subs.empty(), "Subscripts cannot be empty");

  constexpr auto subscripts = filter_if(subs, [](char c) { return c == ' '; });

  static_assert(has_valid_einsum_chars_only<subscripts>());

  static_assert(subscripts.front() != ',' && subscripts.front() != '-' && subscripts.front() != '>',
                "Invalid subscripts string");

  static_assert(subscripts.back() != ',' && subscripts.back() != '-', "Invalid subscripts string");

  static_assert(find(subscripts, string_literal{",,"}) == -1, "Invalid subscripts string");
  static_assert(find(subscripts, string_literal{",-"}) == -1, "Invalid subscripts string");
  static_assert(find(subscripts, string_literal{">,"}) == -1, "Invalid subscripts string");

  if constexpr (find(subscripts, string_literal{"-"}) != -1 || find(subscripts, string_literal{">"}) != -1) {
    static_assert(count(subscripts, '-') == 1, "Invalid subscripts string");
    static_assert(count(subscripts, '>') == 1, "Invalid subscripts string");
    static_assert(find(subscripts, string_literal{"->"}) != -1, "Invalid subscripts string");
  }

  constexpr auto final_subscripts = parse_ellipses<subscripts, Operands...>();

  constexpr long long int out_str_pos{find(final_subscripts, string_literal{"->"})};
  constexpr bool contains_out_str{out_str_pos != -1};
  constexpr auto input_str = final_subscripts.substr(0, out_str_pos);

  constexpr auto output_str = [&]() {
    if constexpr (contains_out_str) {
      return final_subscripts.substr(out_str_pos + 2);
    } else {
      return find_output_str<final_subscripts>();
    }
  }();

  [&]<long long int... is>(std::integer_sequence<long long int, is...>) {
    static_assert(((count(input_str, output_str[is]) > 0) && ...));
  }
  (std::make_integer_sequence<long long int, output_str.size()>());

  static_assert(count(input_str, ',') + 1 == sizeof...(Operands));

  return final_subscripts;
}

}  // namespace lazy
