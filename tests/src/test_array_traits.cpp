#include <catch2/catch_test_macros.hpp>
#include <map>
#include <set>

import Lazy.ArrayTraits;
import Lazy.Expression;

TEST_CASE("obj_rank_v") {
  SECTION("void") { static_assert(lazy::obj_rank_v<void> == -1); }

  SECTION("types only") { static_assert(lazy::obj_rank_v<int> == 0); }

  SECTION("References") {
    static_assert(lazy::obj_rank_v<int&> == 0);
    static_assert(lazy::obj_rank_v<int&&> == 0);
  }

  SECTION("Arrays") {
    static_assert(lazy::obj_rank_v<int[2]> == 1);
    static_assert(lazy::obj_rank_v<int[][2]> == 2);
    static_assert(lazy::obj_rank_v<int[][2][2]> == 3);
  }

  SECTION("Containers") {
    static_assert(lazy::obj_rank_v<std::vector<int>> == 1);
    static_assert(lazy::obj_rank_v<std::vector<std::vector<int>>> == 2);

    static_assert(lazy::obj_rank_v<std::array<int, 1>> == 1);
    static_assert(lazy::obj_rank_v<std::array<std::array<int, 1>, 1>> == 2);

    static_assert(lazy::obj_rank_v<std::vector<std::array<int, 1>>> == 2);
    static_assert(lazy::obj_rank_v<std::array<std::vector<int>, 1>> == 2);

    static_assert(lazy::obj_rank_v<std::set<int>> == 1);
    static_assert(lazy::obj_rank_v<std::set<std::set<int>>> == 2);

    static_assert(lazy::obj_rank_v<std::map<int, int>> == 1);

    static_assert(lazy::obj_rank_v<std::unique_ptr<int>> == 0);
    static_assert(lazy::obj_rank_v<std::shared_ptr<int>> == 0);
  }

  SECTION("Expression") {
    auto fn = [](int v) { return v; };
    auto vec_fn = [](std::vector<int> v) { return v; };

    static_assert(lazy::obj_rank_v<std::remove_cvref_t<decltype(lazy::make_expression([]() {}))>> == 0);
    static_assert(lazy::obj_rank_v<std::remove_cvref_t<decltype(lazy::make_expression(fn, 5))>> == 0);

    {
      std::vector<int> x;
      static_assert(lazy::obj_rank_v<std::remove_cvref_t<decltype(lazy::make_expression(fn, x))>> == 1);
    }

    {
      std::vector<std::vector<int>> x;
      static_assert(lazy::obj_rank_v<std::remove_cvref_t<decltype(lazy::make_expression(fn, x))>> == 2);
    }

    {
      std::vector<std::vector<std::vector<int>>> x;
      static_assert(lazy::obj_rank_v<std::remove_cvref_t<decltype(lazy::make_expression(fn, x))>> == 3);
    }

    {
      std::vector<std::vector<int>> x;
      static_assert(lazy::obj_rank_v<std::remove_cvref_t<decltype(lazy::make_expression(vec_fn, x))>> == 1);
    }

    {
      std::vector<std::vector<std::vector<int>>> x;
      static_assert(lazy::obj_rank_v<std::remove_cvref_t<decltype(lazy::make_expression(vec_fn, x))>> == 2);
    }
  }
}

TEST_CASE("array_size") {
  CHECK(lazy::array_size(7) == 1);

  {
    int arr[7];
    CHECK(lazy::array_size(arr) == 7);
  }

  {
    std::array<int, 7> arr;
    CHECK(lazy::array_size(arr) == 7);
  }

  {
    std::vector<int> arr;
    arr.resize(7);
    CHECK(lazy::array_size(arr) == 7);
  }

  {
    std::array<int, 7> arr;
    auto exp = lazy::make_expression([](int) {}, arr);
    CHECK(lazy::array_size(exp) == 7);
  }

  {
    int arr[5][7];
    CHECK(lazy::array_size(arr) == 5);
    CHECK(lazy::array_size(arr[0]) == 7);
  }

  {
    std::array<std::array<int, 7>, 5> arr;
    CHECK(lazy::array_size(arr) == 5);
    CHECK(lazy::array_size(arr[0]) == 7);
  }

  {
    std::vector<std::vector<int>> arr;
    arr.resize(5);
    arr[0].resize(7);
    CHECK(lazy::array_size(arr) == 5);
    CHECK(lazy::array_size(arr[0]) == 7);
  }

  {
    std::array<std::array<int, 7>, 5> arr;
    auto exp = lazy::make_expression([](int) {}, arr);
    CHECK(lazy::array_size(exp) == 5);
    CHECK(lazy::array_size(exp[0]) == 7);
  }
}

TEST_CASE("array_at") {
  CHECK(lazy::array_at(7, 0) == 7);

  {
    int arr[7];

    for (int i = 0; i < 7; ++i) {
      arr[i] = (i + 1) * 2;
      CHECK(lazy::array_at(arr, i) == arr[i]);
    }
  }

  {
    std::array<int, 7> arr;

    for (int i = 0; i < 7; ++i) {
      arr[i] = (i + 1) * 2;
      CHECK(lazy::array_at(arr, i) == arr[i]);
    }
  }

  {
    std::vector<int> arr;
    arr.resize(7);

    for (int i = 0; i < 7; ++i) {
      arr[i] = (i + 1) * 2;
      CHECK(lazy::array_at(arr, i) == arr[i]);
    }
  }

  {
    std::array<int, 7> arr;
    auto exp = lazy::make_expression([](int e) { return e; }, std::ref(arr));

    for (int i = 0; i < 7; ++i) {
      arr[i] = (i + 1) * 2;
      CHECK(lazy::array_at(exp, i) == arr[i]);
    }
  }

  {
    int arr[5][7];

    for (int j = 0; j < 7; ++j) {
      for (int i = 0; i < 5; ++i) {
        arr[i][j] = (i + 1) * (j + 1) * 2;
        CHECK(lazy::array_at(arr[i], j) == arr[i][j]);
      }
    }

    for (unsigned int i = 0; i < 5; ++i) {
      CHECK(lazy::array_at(arr, i)[0] == arr[i][0]);
    }
  }

  {
    std::array<std::array<int, 7>, 5> arr;

    for (int j = 0; j < 7; ++j) {
      for (int i = 0; i < 5; ++i) {
        arr[i][j] = (i + 1) * (j + 1) * 2;
        CHECK(lazy::array_at(arr[i], j) == arr[i][j]);
      }
    }

    for (unsigned int i = 0; i < 5; ++i) {
      CHECK(lazy::array_at(arr, i) == arr[i]);
    }
  }

  {
    std::vector<std::vector<int>> arr;
    arr = std::vector<std::vector<int>>(5, std::vector<int>(7));

    for (int j = 0; j < 7; ++j) {
      for (int i = 0; i < 5; ++i) {
        arr[i][j] = (i + 1) * (j + 1) * 2;
        CHECK(lazy::array_at(arr[i], j) == arr[i][j]);
      }
    }

    for (unsigned int i = 0; i < 5; ++i) {
      CHECK(lazy::array_at(arr, i) == arr[i]);
    }
  }

  {
    std::array<std::array<int, 7>, 5> arr;

    auto exp = lazy::make_expression([](int e) { return e; }, std::ref(arr));

    for (unsigned int j = 0; j < 7; ++j) {
      for (unsigned int i = 0; i < 5; ++i) {
        arr[i][j] = (i + 1) * (j + 1) * 2;
        CHECK(lazy::array_at(exp[i], j) == arr[i][j]);
      }
    }
  }
}
