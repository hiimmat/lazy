#include <catch2/catch_test_macros.hpp>

import Lazy.StringLiteral;

TEST_CASE("string literal") {
  SECTION("empty") {
    static constexpr lazy::string_literal<0> sl;

    static_assert(sl.begin() == sl.end());
    static_assert(sl.data() == sl.begin());

    static_assert(sl.size() == 0);
    static_assert(sl.empty());

    CHECK(sl.substr() == lazy::string_literal<0>());

    CHECK(sl.to_string_view() == std::string_view());

    CHECK(filter_if(sl, [](char c) { return c == 's'; }) == lazy::string_literal<0>());
    CHECK(filter(sl, 's') == lazy::string_literal<0>());

    CHECK(count_if(sl, [](char c) { return c == 's'; }) == 0);
    CHECK(count(sl, 's') == 0);

    CHECK(sort(sl) == lazy::string_literal<0>());
  }

  SECTION("populated") {
    static constexpr lazy::string_literal sl{"this is a string literal"};

    static_assert(sl[0] == 't');
    static_assert(sl[1] == 'h');
    static_assert(sl[2] == 'i');
    static_assert(sl[3] == 's');
    static_assert(sl[4] == ' ');
    static_assert(sl[5] == 'i');
    static_assert(sl[6] == 's');
    static_assert(sl[7] == ' ');
    static_assert(sl[8] == 'a');
    static_assert(sl[9] == ' ');
    static_assert(sl[10] == 's');
    static_assert(sl[11] == 't');
    static_assert(sl[12] == 'r');
    static_assert(sl[13] == 'i');
    static_assert(sl[14] == 'n');
    static_assert(sl[15] == 'g');
    static_assert(sl[16] == ' ');
    static_assert(sl[17] == 'l');
    static_assert(sl[18] == 'i');
    static_assert(sl[19] == 't');
    static_assert(sl[20] == 'e');
    static_assert(sl[21] == 'r');
    static_assert(sl[22] == 'a');
    static_assert(sl[23] == 'l');

    static_assert(*sl.begin() == 't');
    static_assert(*sl.end() == '\0');
    static_assert(*(sl.end() - 1) == 'l');

    static_assert(sl.front() == 't');
    static_assert(sl.back() == 'l');

    static_assert(sl.size() == 24);
    static_assert(!sl.empty());

    CHECK(sl.substr() == lazy::string_literal("this is a string literal"));
    CHECK(sl.substr(sl.size()) == lazy::string_literal<0>());
    CHECK(sl.substr(5) == lazy::string_literal("is a string literal"));
    CHECK(sl.substr(0, 4) == lazy::string_literal("this"));
    CHECK(sl.substr(10, 6) == lazy::string_literal("string"));

    CHECK(sl.to_string_view() == std::string_view("this is a string literal"));

    CHECK(filter_if(sl, [](char c) { return c == 's'; }) == lazy::string_literal("thi i a tring literal"));
    CHECK(filter(sl, 's') == lazy::string_literal("thi i a tring literal"));

    CHECK(count_if(sl, [](char c) { return c == 's'; }) == 3);
    CHECK(count(sl, 's') == 3);

    CHECK(find(sl, lazy::string_literal("string")) == 10);
    CHECK(find(sl, lazy::string_literal("string"), sl.size()) == -1);
    CHECK(find(sl, lazy::string_literal("string"), 10, 0) == -1);
    CHECK(find(sl, lazy::string_literal("string"), 10, 6) == 10);

    CHECK(replace(sl, lazy::string_literal("literal"), lazy::string_literal("view")) ==
          lazy::string_literal("this is a string view"));

    CHECK(sort(sl) == lazy::string_literal("    aaeghiiiillnrrsssttt"));

    CHECK(sl ==
          lazy::concatenate<lazy::string_literal("this "), lazy::string_literal("is "), lazy::string_literal("a "),
                            lazy::string_literal("string "), lazy::string_literal("literal")>());

    CHECK(sl ==
          lazy::join<lazy::string_literal(" "), lazy::string_literal("this"), lazy::string_literal("is"),
                     lazy::string_literal("a"), lazy::string_literal("string"), lazy::string_literal("literal")>());
  }
}
