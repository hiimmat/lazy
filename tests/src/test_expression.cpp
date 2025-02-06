#include <catch2/catch_test_macros.hpp>
#include <functional>

import Lazy.Expression;


namespace {

void no_return_test(int& x) { x = 7; }

[[nodiscard]] int no_params_test() { return 7; }

[[nodiscard]] int value_test(int x) { return x; }

[[nodiscard]] int ref_test(int& val) { return val; }

void ref_test_two_params(int& x, int& y) {
  x = 5;
  y = 7;
}

[[nodiscard]] int const_ref_test(const int& val) { return val; }

[[nodiscard]] int rvalue_ref_test(int&& val) { return val; }

[[nodiscard]] int pointer_test(int* val) { return *val; }

[[nodiscard]] int pointer_ref_test(int*& val) { return *val; }

[[nodiscard]] int mixed_params_test(int x, int& y, const int& z, int&& v, int* w, int*& q) {
  return x + y + z + v + *w + *q;
}

void add_y_to_x(int& x, const int& y) { x += y; }

void store_result(int x, int y, int& z) { z = x + y; }

[[nodiscard]] int addition(int x, int y) { return x + y; }

void multiplication(int x, int y, int& z) { z = x * y; }

void different_input_ranks_1(const std::vector<int>& x, int y, int& z) { z = x[0] + y; }

void different_input_ranks_2(int x, const std::vector<int>& y, int& z) { z = x + y[0]; }

void arrays_as_inputs(const std::vector<int>& x, const std::vector<int>& y, std::vector<int>& z) { z[0] = x[0] + y[0]; }

struct struct_test {
  int fn(int x, int y) { return x + y; }
};

class constructible_test {
 private:
  int x;

 public:
  explicit constructible_test(int x) : x{x} {}

  [[nodiscard]] int get_x() const { return x; }
};

int construct_test(constructible_test c) { return c.get_x(); }

}  // namespace

TEST_CASE("Expression") {
  SECTION("function with no return") {
    int x = 5;
    auto exp = lazy::make_expression(no_return_test, std::ref(x));
    exp();
    CHECK(x == 7);
  }

  SECTION("function with no parameters") {
    auto exp = lazy::make_expression(no_params_test);
    CHECK(exp() == 7);
  }

  SECTION("function with parameters passed by value") {
    auto exp = lazy::make_expression(value_test, 5);
    CHECK(exp() == 5);
  }

  SECTION("function with parameters passed by reference") {
    auto exp = lazy::make_expression(ref_test, 5);
    CHECK(exp() == 5);
  }

  SECTION("function with parameters passed by const reference") {
    auto exp = lazy::make_expression(const_ref_test, 5);
    CHECK(exp() == 5);
  }

  SECTION("function with rvalue parameters") {
    auto exp = lazy::make_expression(rvalue_ref_test, 5);
    CHECK(exp() == 5);
  }

  SECTION("function with pointer parameter") {
    int value = 5;
    int* ptr = &value;
    auto exp = lazy::make_expression(pointer_test, ptr);
    CHECK(exp() == 5);
  }

  SECTION("function with reference pointer parameter") {
    int value = 5;
    int* ptr = &value;
    auto exp = lazy::make_expression(pointer_ref_test, ptr);
    CHECK(exp() == 5);
  }

  SECTION("function with mixed parameters") {
    int y = 2, w = 5, q = 6;
    const int z = 3;
    int* q_ptr = &q;
    auto exp = lazy::make_expression(mixed_params_test, 1, y, z, 4, &w, q_ptr);
    CHECK(exp() == 21);
  }

  SECTION("unwrap rvalue pointer") {
    auto x = 5;

    auto exp = lazy::make_expression(ref_test, &x);
    CHECK(exp() == 5);
  }

  SECTION("unwrap lvalue pointer") {
    auto x = 5;
    auto* y = &x;

    auto exp = lazy::make_expression(ref_test, y);
    CHECK(exp() == 5);
  }

  SECTION("unwrap std::unique_ptr") {
    auto x = std::make_unique<int>(5);

    auto exp = lazy::make_expression(ref_test, std::move(x));
    CHECK(exp() == 5);
  }

  SECTION("unwrap std::shared_ptr") {
    auto x = std::make_shared<int>(5);

    auto exp = lazy::make_expression(ref_test, std::move(x));
    CHECK(exp() == 5);
  }

  SECTION("unwrap std::ref") {
    auto x = 5;

    auto exp = lazy::make_expression(ref_test, std::ref(x));
    CHECK(exp() == 5);
  }

  SECTION("unwrap std::cref") {
    auto x = 5;

    auto exp = lazy::make_expression(const_ref_test, std::cref(x));
    CHECK(exp() == 5);
  }

  SECTION("convert invocable input") {
    auto exp = lazy::make_expression(value_test, 'c');
    CHECK(exp() == 99);
  }

  SECTION("construct invocable input") {
    auto exp = lazy::make_expression(construct_test, 5);
    CHECK(exp() == 5);
  }

  SECTION("invocable wrapped into a std::reference_wrapper") {
    std::function<int(int, int)> fn = [](int x, int y) { return x + y; };
    auto exp = lazy::make_expression(std::ref(fn), 4, 6);
    CHECK(exp() == 10);
  }

  SECTION("expression with lambda") {
    auto exp = lazy::make_expression([](int x, int y) { return x + y; }, 4, 6);
    CHECK(exp() == 10);
  }

  SECTION("expression with std::function") {
    std::function<int(int, int)> fn = [](int x, int y) { return x + y; };
    auto exp = lazy::make_expression(fn, 4, 6);
    CHECK(exp() == 10);
  }

  SECTION("expression with class member function") {
    struct_test s;
    auto exp = lazy::make_expression(&struct_test::fn, s, 2, 3);
    CHECK(exp() == 5);
  }

  SECTION("expression with std::mem_fn") {
    struct_test s;
    auto mem_fn = std::mem_fn(&struct_test::fn);
    auto exp = lazy::make_expression(mem_fn, s, 2, 3);
    CHECK(exp() == 5);
  }

  SECTION("copy constructor") {
    auto exp = lazy::make_expression(value_test, 5);
    CHECK(exp() == 5);

    auto copy_exp(exp);
    CHECK(copy_exp() == 5);
  }

  SECTION("move constructor") {
    auto exp = lazy::make_expression(value_test, 5);
    CHECK(exp() == 5);

    auto move_exp(std::move(exp));
    CHECK(move_exp() == 5);
  }

  SECTION("iterate multidimensional arrays") {
    std::array<std::array<std::array<int, 2>, 4>, 6> x;
    std::array<std::array<std::array<int, 2>, 4>, 6> y;

    for (unsigned int k = 0; k < 2; ++k) {
      for (unsigned int j = 0; j < 4; ++j) {
        for (unsigned int i = 0; i < 6; ++i) {
          x[i][j][k] = 0;
          y[i][j][k] = 0;
        }
      }
    }

    auto exp = lazy::make_expression(ref_test_two_params, std::ref(x), std::ref(y));

    CHECK(exp.size() == 6);
    CHECK(exp[0].size() == 4);
    CHECK(exp[0][0].size() == 2);

    exp.eval();

    for (unsigned int k = 0; k < 2; ++k) {
      for (unsigned int j = 0; j < 4; ++j) {
        for (unsigned int i = 0; i < 6; ++i) {
          CHECK(x[i][j][k] == 5);
          CHECK(y[i][j][k] == 7);
        }
      }
    }

    for (unsigned int k = 0; k < 2; ++k) {
      for (unsigned int j = 0; j < 4; ++j) {
        for (unsigned int i = 0; i < 6; ++i) {
          x[i][j][k] = 0;
          y[i][j][k] = 0;
          exp.eval_at(i, j, k);
          CHECK(x[i][j][k] == 5);
          CHECK(y[i][j][k] == 7);
        }
      }
    }

    for (unsigned int k = 0; k < 2; ++k) {
      for (unsigned int j = 0; j < 4; ++j) {
        for (unsigned int i = 0; i < 6; ++i) {
          x[i][j][k] = 0;
          y[i][j][k] = 0;
          exp[i][j][k];
          CHECK(x[i][j][k] == 5);
          CHECK(y[i][j][k] == 7);
        }
      }
    }

    for (unsigned int k = 0; k < 2; ++k) {
      for (unsigned int j = 0; j < 4; ++j) {
        for (unsigned int i = 0; i < 6; ++i) {
          x[i][j][k] = 0;
          y[i][j][k] = 0;
        }
      }
    }

    exp.eval<2u>();

    for (unsigned int k = 0; k < 2; ++k) {
      for (unsigned int j = 0; j < 4; ++j) {
        for (unsigned int i = 0; i < 6; ++i) {
          if (k == 0) {
            CHECK(x[i][j][k] == 5);
            CHECK(y[i][j][k] == 7);
          } else {
            CHECK(x[i][j][k] == 0);
            CHECK(y[i][j][k] == 0);
          }
        }
      }
    }
  }

  SECTION("iterate multidimensional arrays with broadcasting") {
    std::array<std::array<std::array<std::array<int, 2>, 4>, 6>, 8> x;
    std::array<std::array<std::array<int, 2>, 1>, 6> y;

    for (int j = 0; j < 2; ++j) {
      for (int i = 0; i < 6; ++i) {
        y[i][0][j] = i + j * 6;
      }
    }

    for (unsigned int l = 0; l < 2; ++l) {
      for (unsigned int k = 0; k < 4; ++k) {
        for (unsigned int j = 0; j < 6; ++j) {
          for (unsigned int i = 0; i < 8; ++i) {
            x[i][j][k][l] = 1;
          }
        }
      }
    }

    auto exp = lazy::make_expression(add_y_to_x, std::ref(x), std::cref(y));

    CHECK(exp.size() == 8);
    CHECK(exp[0].size() == 6);
    CHECK(exp[0][0].size() == 4);
    CHECK(exp[0][0][0].size() == 2);

    exp.eval();

    for (unsigned int l = 0; l < 2; ++l) {
      for (unsigned int k = 0; k < 4; ++k) {
        for (unsigned int j = 0; j < 6; ++j) {
          for (unsigned int i = 0; i < 8; ++i) {
            CHECK(x[i][j][k][l] == (y[j][0][l] + 1));
          }
        }
      }
    }

    for (unsigned int l = 0; l < 2; ++l) {
      for (unsigned int k = 0; k < 4; ++k) {
        for (unsigned int j = 0; j < 6; ++j) {
          for (unsigned int i = 0; i < 8; ++i) {
            x[i][j][k][l] = 1;
            exp.eval_at(i, j, k, l);
            CHECK(x[i][j][k][l] == (y[j][0][l] + 1));
          }
        }
      }
    }

    for (unsigned int l = 0; l < 2; ++l) {
      for (unsigned int k = 0; k < 4; ++k) {
        for (unsigned int j = 0; j < 6; ++j) {
          for (unsigned int i = 0; i < 8; ++i) {
            x[i][j][k][l] = 1;
            exp[i][j][k][l];
            CHECK(x[i][j][k][l] == (y[j][0][l] + 1));
          }
        }
      }
    }

    for (unsigned int l = 0; l < 2; ++l) {
      for (unsigned int k = 0; k < 4; ++k) {
        for (unsigned int j = 0; j < 6; ++j) {
          for (unsigned int i = 0; i < 8; ++i) {
            x[i][j][k][l] = 1;
          }
        }
      }
    }

    exp.eval<2u>();

    for (unsigned int l = 0; l < 2; ++l) {
      for (unsigned int k = 0; k < 4; ++k) {
        for (unsigned int j = 0; j < 6; ++j) {
          for (unsigned int i = 0; i < 8; ++i) {
            if (l == 0) {
              CHECK(x[i][j][k][l] == (y[j][0][l] + 1));
            } else {
              CHECK(x[i][j][k][l] == 1);
            }
          }
        }
      }
    }
  }

  SECTION("store result of broadcasted arrays") {
    std::array<std::array<int, 3>, 4> x{{{{0, 0, 0}}, {{10, 10, 10}}, {{20, 20, 20}}, {{30, 30, 30}}}};
    std::array<int, 3> y{{1, 2, 3}};
    std::array<std::array<int, 3>, 4> z;
    auto exp = lazy::make_expression(store_result, std::cref(x), std::cref(y), std::ref(z));

    CHECK(exp.size() == 4);
    CHECK(exp[0].size() == 3);

    exp.eval();

    for (unsigned int j = 0; j < 3; ++j) {
      for (unsigned int i = 0; i < 4; ++i) {
        CHECK(z[i][j] == (x[i][j] + y[j]));
      }
    }

    for (unsigned int j = 0; j < 3; ++j) {
      for (unsigned int i = 0; i < 4; ++i) {
        z[i][j] = 0;
        exp.eval_at(i, j);
        CHECK(z[i][j] == (x[i][j] + y[j]));
      }
    }

    for (unsigned int j = 0; j < 3; ++j) {
      for (unsigned int i = 0; i < 4; ++i) {
        z[i][j] = 0;
        exp[i][j];
        CHECK(z[i][j] == (x[i][j] + y[j]));
      }
    }

    CHECK_THROWS(exp.eval<2u>());
  }

  SECTION("broadcasting test with input parameters of different ranks 1") {
    std::vector<std::vector<std::vector<int>>> x;
    x = std::vector<std::vector<std::vector<int>>>(4, std::vector<std::vector<int>>(4, std::vector<int>(4)));
    std::vector<std::vector<std::vector<int>>> y;
    y = std::vector<std::vector<std::vector<int>>>(4, std::vector<std::vector<int>>(4, std::vector<int>(4)));
    std::vector<std::vector<std::vector<int>>> z;
    z = std::vector<std::vector<std::vector<int>>>(4, std::vector<std::vector<int>>(4, std::vector<int>(4)));

    for (int k = 0; k < 4; ++k) {
      for (int j = 0; j < 4; ++j) {
        for (int i = 0; i < 4; ++i) {
          x[i][j][k] = i + 4 * j + 16 * k;
          y[i][j][k] = 2;
        }
      }
    }

    auto exp = lazy::make_expression(different_input_ranks_1, x, y, std::ref(z));

    CHECK(exp.size() == 4);
    CHECK(exp[0].size() == 4);
    CHECK(exp[0][0].size() == 4);

    exp.eval();

    for (unsigned int k = 0; k < 4; ++k) {
      for (unsigned int j = 0; j < 4; ++j) {
        for (unsigned int i = 0; i < 4; ++i) {
          CHECK(z[i][j][k] == (x[i][j][0] + y[i][j][k]));
        }
      }
    }

    for (unsigned int k = 0; k < 4; ++k) {
      for (unsigned int j = 0; j < 4; ++j) {
        for (unsigned int i = 0; i < 4; ++i) {
          z[i][j][k] = 0;
          exp.eval_at(i, j, k);
          CHECK(z[i][j][k] == (x[i][j][0] + 2));
        }
      }
    }

    for (unsigned int k = 0; k < 4; ++k) {
      for (unsigned int j = 0; j < 4; ++j) {
        for (unsigned int i = 0; i < 4; ++i) {
          z[i][j][k] = 0;
          exp[i][j][k];
          CHECK(z[i][j][k] == (x[i][j][0] + 2));
        }
      }
    }

    for (unsigned int k = 0; k < 4; ++k) {
      for (unsigned int j = 0; j < 4; ++j) {
        for (unsigned int i = 0; i < 4; ++i) {
          z[i][j][k] = 0;
        }
      }
    }

    exp.eval<2>();

    for (unsigned int k = 0; k < 4; ++k) {
      for (unsigned int j = 0; j < 4; ++j) {
        for (unsigned int i = 0; i < 4; ++i) {
          if (k % 2 == 0) {
            CHECK(z[i][j][k] == (x[i][j][0] + 2));
          } else {
            CHECK(z[i][j][k] == 0);
          }
        }
      }
    }
  }

  SECTION("broadcasting test with input parameters of different ranks 2") {
    std::vector<std::vector<std::vector<int>>> x;
    x = std::vector<std::vector<std::vector<int>>>(4, std::vector<std::vector<int>>(4, std::vector<int>(4)));
    std::vector<std::vector<std::vector<int>>> y;
    y = std::vector<std::vector<std::vector<int>>>(4, std::vector<std::vector<int>>(4, std::vector<int>(4)));
    std::vector<std::vector<std::vector<int>>> z;
    z = std::vector<std::vector<std::vector<int>>>(4, std::vector<std::vector<int>>(4, std::vector<int>(4)));

    for (int k = 0; k < 4; ++k) {
      for (int j = 0; j < 4; ++j) {
        for (int i = 0; i < 4; ++i) {
          x[i][j][k] = 2;
          y[i][j][k] = i + 4 * j + 16 * k;
        }
      }
    }

    auto exp = lazy::make_expression(different_input_ranks_2, x, y, std::ref(z));

    CHECK(exp.size() == 4);
    CHECK(exp[0].size() == 4);
    CHECK(exp[0][0].size() == 4);

    exp.eval();

    for (unsigned int k = 0; k < 4; ++k) {
      for (unsigned int j = 0; j < 4; ++j) {
        for (unsigned int i = 0; i < 4; ++i) {
          CHECK(z[i][j][k] == (2 + y[i][j][0]));
        }
      }
    }

    for (unsigned int k = 0; k < 4; ++k) {
      for (unsigned int j = 0; j < 4; ++j) {
        for (unsigned int i = 0; i < 4; ++i) {
          z[i][j][k] = 0;
          exp.eval_at(i, j, k);
          CHECK(z[i][j][k] == (2 + y[i][j][0]));
        }
      }
    }

    for (unsigned int k = 0; k < 4; ++k) {
      for (unsigned int j = 0; j < 4; ++j) {
        for (unsigned int i = 0; i < 4; ++i) {
          z[i][j][k] = 0;
          exp[i][j][k];
          CHECK(z[i][j][k] == (2 + y[i][j][0]));
        }
      }
    }

    for (unsigned int k = 0; k < 4; ++k) {
      for (unsigned int j = 0; j < 4; ++j) {
        for (unsigned int i = 0; i < 4; ++i) {
          z[i][j][k] = 0;
        }
      }
    }

    exp.eval<2>();

    for (unsigned int k = 0; k < 4; ++k) {
      for (unsigned int j = 0; j < 4; ++j) {
        for (unsigned int i = 0; i < 4; ++i) {
          if (k % 2 == 0) {
            CHECK(z[i][j][k] == (2 + y[i][j][0]));
          } else {
            CHECK(z[i][j][k] == 0);
          }
        }
      }
    }
  }

  SECTION("expression with invocable whose inputs are partially unwrapped arrays") {
    std::vector<std::vector<std::vector<int>>> x;
    x = std::vector<std::vector<std::vector<int>>>(4, std::vector<std::vector<int>>(4, std::vector<int>(4)));
    std::vector<std::vector<std::vector<int>>> y;
    y = std::vector<std::vector<std::vector<int>>>(4, std::vector<std::vector<int>>(4, std::vector<int>(4)));
    std::vector<std::vector<std::vector<int>>> z;
    z = std::vector<std::vector<std::vector<int>>>(4, std::vector<std::vector<int>>(4, std::vector<int>(4)));

    for (unsigned int k = 0; k < 4; ++k) {
      for (unsigned int j = 0; j < 4; ++j) {
        for (unsigned int i = 0; i < 4; ++i) {
          x[i][j][k] = 5;
          y[i][j][k] = 4;
        }
      }
    }

    auto exp = lazy::make_expression(arrays_as_inputs, std::ref(x), std::ref(y), std::ref(z));

    CHECK(exp.size() == 4);
    CHECK(exp[0].size() == 4);

    exp.eval();

    for (unsigned int k = 0; k < 4; ++k) {
      for (unsigned int j = 0; j < 4; ++j) {
        for (unsigned int i = 0; i < 4; ++i) {
          if (k == 0) {
            CHECK(z[i][j][k] == (x[i][j][k] + y[i][j][k]));
          } else {
            CHECK(z[i][j][k] == 0);
          }
        }
      }
    }

    for (unsigned int j = 0; j < 4; ++j) {
      for (unsigned int i = 0; i < 4; ++i) {
        z[i][j][0] = 0;
        exp.eval_at(i, j);
      }
    }

    for (unsigned int j = 0; j < 4; ++j) {
      for (unsigned int i = 0; i < 4; ++i) {
        z[i][j][0] = 0;
        exp[i][j];
      }
    }

    for (unsigned int k = 0; k < 4; ++k) {
      for (unsigned int j = 0; j < 4; ++j) {
        for (unsigned int i = 0; i < 4; ++i) {
          if (k == 0) {
            CHECK(z[i][j][k] == (x[i][j][k] + y[i][j][k]));
          } else {
            CHECK(z[i][j][k] == 0);
          }
        }
      }
    }
  }

  SECTION("nested expression") {
    std::array<std::array<int, 6>, 4> x{
          {{{0, 0, 0, 0, 0, 0}}, {{10, 10, 10, 10, 10, 10}}, {{20, 20, 20, 20, 20, 20}}, {{30, 30, 30, 30, 30, 30}}}};
    std::array<int, 6> y{{1, 2, 3, 4, 5, 6}};
    std::array<std::array<int, 6>, 4> z;
    auto addition_exp = lazy::make_expression(addition, x, y);

    auto multiplication_exp = lazy::make_expression(multiplication, addition_exp, 2, std::ref(z));

    CHECK(multiplication_exp.size() == 4);
    CHECK(multiplication_exp[0].size() == 6);

    multiplication_exp.eval();

    for (unsigned int j = 0; j < 6; ++j) {
      for (unsigned int i = 0; i < 4; ++i) {
        CHECK(z[i][j] == ((x[i][j] + y[j]) * 2));
      }
    }

    for (unsigned int j = 0; j < 6; ++j) {
      for (unsigned int i = 0; i < 4; ++i) {
        z[i][j] = 0;
        multiplication_exp.eval_at(i, j);
        CHECK(z[i][j] == ((x[i][j] + y[j]) * 2));
      }
    }

    for (unsigned int j = 0; j < 6; ++j) {
      for (unsigned int i = 0; i < 4; ++i) {
        z[i][j] = 0;
        multiplication_exp[i][j];
        CHECK(z[i][j] == ((x[i][j] + y[j]) * 2));
      }
    }

    for (unsigned int j = 0; j < 6; ++j) {
      for (unsigned int i = 0; i < 4; ++i) {
        z[i][j] = 0;
      }
    }

    multiplication_exp.eval<2u>();

    for (unsigned int j = 0; j < 6; ++j) {
      for (unsigned int i = 0; i < 4; ++i) {
        if (j % 2 == 0) {
          CHECK(z[i][j] == ((x[i][j] + y[j]) * 2));
        } else {
          CHECK(z[i][j] == 0);
        }
      }
    }
  }

  SECTION("nested expression as ref wrapper") {
    std::array<std::array<int, 6>, 4> x{
          {{{0, 0, 0, 0, 0, 0}}, {{10, 10, 10, 10, 10, 10}}, {{20, 20, 20, 20, 20, 20}}, {{30, 30, 30, 30, 30, 30}}}};
    std::array<int, 6> y{{1, 2, 3, 4, 5, 6}};
    std::array<std::array<int, 6>, 4> z;
    auto addition_exp = lazy::make_expression(addition, x, y);

    auto multiplication_exp = lazy::make_expression(multiplication, std::ref(addition_exp), 2, std::ref(z));

    CHECK(multiplication_exp.size() == 4);
    CHECK(multiplication_exp[0].size() == 6);

    multiplication_exp.eval();

    for (unsigned int j = 0; j < 6; ++j) {
      for (unsigned int i = 0; i < 4; ++i) {
        CHECK(z[i][j] == ((x[i][j] + y[j]) * 2));
      }
    }

    for (unsigned int j = 0; j < 6; ++j) {
      for (unsigned int i = 0; i < 4; ++i) {
        z[i][j] = 0;
        multiplication_exp.eval_at(i, j);
        CHECK(z[i][j] == ((x[i][j] + y[j]) * 2));
      }
    }

    for (unsigned int j = 0; j < 6; ++j) {
      for (unsigned int i = 0; i < 4; ++i) {
        z[i][j] = 0;
        multiplication_exp[i][j];
        CHECK(z[i][j] == ((x[i][j] + y[j]) * 2));
      }
    }

    for (unsigned int j = 0; j < 6; ++j) {
      for (unsigned int i = 0; i < 4; ++i) {
        z[i][j] = 0;
      }
    }

    multiplication_exp.eval<2u>();

    for (unsigned int j = 0; j < 6; ++j) {
      for (unsigned int i = 0; i < 4; ++i) {
        if (j % 2 == 0) {
          CHECK(z[i][j] == ((x[i][j] + y[j]) * 2));
        } else {
          CHECK(z[i][j] == 0);
        }
      }
    }
  }

  SECTION("nested expression with the same variable used for both expressions") {
    std::array<std::array<int, 6>, 4> x{
          {{{0, 0, 0, 0, 0, 0}}, {{10, 10, 10, 10, 10, 10}}, {{20, 20, 20, 20, 20, 20}}, {{30, 30, 30, 30, 30, 30}}}};
    std::array<int, 6> y{{1, 2, 3, 4, 5, 6}};
    std::array<std::array<int, 6>, 4> z{
          {{{0, 0, 0, 0, 0, 0}}, {{10, 10, 10, 10, 10, 10}}, {{20, 20, 20, 20, 20, 20}}, {{30, 30, 30, 30, 30, 30}}}};
    auto addition_exp = lazy::make_expression(addition, std::ref(x), std::ref(y));

    auto multiplication_exp = lazy::make_expression(multiplication, addition_exp, 2, std::ref(x));

    CHECK(multiplication_exp.size() == 4);
    CHECK(multiplication_exp[0].size() == 6);

    multiplication_exp.eval();

    for (unsigned int j = 0; j < 6; ++j) {
      for (unsigned int i = 0; i < 4; ++i) {
        CHECK(x[i][j] == ((z[i][j] + y[j]) * 2));
      }
    }

    for (unsigned int j = 0; j < 6; ++j) {
      for (unsigned int i = 0; i < 4; ++i) {
        x[i][j] = z[i][j];
        multiplication_exp.eval_at(i, j);
        CHECK(x[i][j] == ((z[i][j] + y[j]) * 2));
      }
    }

    for (unsigned int j = 0; j < 6; ++j) {
      for (unsigned int i = 0; i < 4; ++i) {
        x[i][j] = z[i][j];
        multiplication_exp[i][j];
        CHECK(x[i][j] == ((z[i][j] + y[j]) * 2));
      }
    }

    for (unsigned int j = 0; j < 6; ++j) {
      for (unsigned int i = 0; i < 4; ++i) {
        x[i][j] = z[i][j];
      }
    }

    multiplication_exp.eval<2u>();

    for (unsigned int j = 0; j < 6; ++j) {
      for (unsigned int i = 0; i < 4; ++i) {
        if (j % 2 == 0) {
          CHECK(x[i][j] == ((z[i][j] + y[j]) * 2));
        } else {
          CHECK(x[i][j] == z[i][j]);
        }
      }
    }
  }

  SECTION("copy constructor with nested expression") {
    std::array<std::array<int, 6>, 4> x{
          {{{0, 0, 0, 0, 0, 0}}, {{10, 10, 10, 10, 10, 10}}, {{20, 20, 20, 20, 20, 20}}, {{30, 30, 30, 30, 30, 30}}}};
    std::array<int, 6> y{{1, 2, 3, 4, 5, 6}};
    std::array<std::array<int, 6>, 4> z;
    auto addition_exp = lazy::make_expression(addition, x, y);

    auto multiplication_exp = lazy::make_expression(multiplication, addition_exp, 2, std::ref(z));

    auto copy_exp = multiplication_exp;

    CHECK(multiplication_exp.size() == 4);
    CHECK(multiplication_exp[0].size() == 6);

    CHECK(copy_exp.size() == 4);
    CHECK(copy_exp[0].size() == 6);

    multiplication_exp.eval();

    for (unsigned int j = 0; j < 6; ++j) {
      for (unsigned int i = 0; i < 4; ++i) {
        CHECK(z[i][j] == ((x[i][j] + y[j]) * 2));
      }
    }

    for (unsigned int j = 0; j < 6; ++j) {
      for (unsigned int i = 0; i < 4; ++i) {
        z[i][j] = 0;
      }
    }

    copy_exp.eval();

    for (unsigned int j = 0; j < 6; ++j) {
      for (unsigned int i = 0; i < 4; ++i) {
        CHECK(z[i][j] == ((x[i][j] + y[j]) * 2));
      }
    }

    for (unsigned int j = 0; j < 6; ++j) {
      for (unsigned int i = 0; i < 4; ++i) {
        z[i][j] = 0;
        multiplication_exp.eval_at(i, j);
        CHECK(z[i][j] == ((x[i][j] + y[j]) * 2));

        z[i][j] = 0;
        copy_exp.eval_at(i, j);
        CHECK(z[i][j] == ((x[i][j] + y[j]) * 2));
      }
    }

    for (unsigned int j = 0; j < 6; ++j) {
      for (unsigned int i = 0; i < 4; ++i) {
        z[i][j] = 0;
        multiplication_exp[i][j];
        CHECK(z[i][j] == ((x[i][j] + y[j]) * 2));

        z[i][j] = 0;
        copy_exp[i][j];
        CHECK(z[i][j] == ((x[i][j] + y[j]) * 2));
      }
    }

    for (unsigned int j = 0; j < 6; ++j) {
      for (unsigned int i = 0; i < 4; ++i) {
        z[i][j] = 0;
      }
    }

    multiplication_exp.eval<2u>();

    for (unsigned int j = 0; j < 6; ++j) {
      for (unsigned int i = 0; i < 4; ++i) {
        if (j % 2 == 0) {
          CHECK(z[i][j] == ((x[i][j] + y[j]) * 2));
        } else {
          CHECK(z[i][j] == 0);
        }
      }
    }

    for (unsigned int j = 0; j < 6; ++j) {
      for (unsigned int i = 0; i < 4; ++i) {
        z[i][j] = 0;
      }
    }

    copy_exp.eval<2u>();

    for (unsigned int j = 0; j < 6; ++j) {
      for (unsigned int i = 0; i < 4; ++i) {
        if (j % 2 == 0) {
          CHECK(z[i][j] == ((x[i][j] + y[j]) * 2));
        } else {
          CHECK(z[i][j] == 0);
        }
      }
    }
  }

  SECTION("move constructor with nested expression") {
    std::array<std::array<int, 6>, 4> x{
          {{{0, 0, 0, 0, 0, 0}}, {{10, 10, 10, 10, 10, 10}}, {{20, 20, 20, 20, 20, 20}}, {{30, 30, 30, 30, 30, 30}}}};
    std::array<int, 6> y{{1, 2, 3, 4, 5, 6}};
    std::array<std::array<int, 6>, 4> z;
    auto addition_exp = lazy::make_expression(addition, x, y);

    auto multiplication_exp = lazy::make_expression(multiplication, addition_exp, 2, std::ref(z));

    auto move_exp = std::move(multiplication_exp);

    CHECK(move_exp.size() == 4);
    CHECK(move_exp[0].size() == 6);

    move_exp.eval();

    for (unsigned int j = 0; j < 6; ++j) {
      for (unsigned int i = 0; i < 4; ++i) {
        CHECK(z[i][j] == ((x[i][j] + y[j]) * 2));
      }
    }

    for (unsigned int j = 0; j < 6; ++j) {
      for (unsigned int i = 0; i < 4; ++i) {
        z[i][j] = 0;
        move_exp.eval_at(i, j);
        CHECK(z[i][j] == ((x[i][j] + y[j]) * 2));
      }
    }

    for (unsigned int j = 0; j < 6; ++j) {
      for (unsigned int i = 0; i < 4; ++i) {
        z[i][j] = 0;
        move_exp[i][j];
        CHECK(z[i][j] == ((x[i][j] + y[j]) * 2));
      }
    }

    for (unsigned int j = 0; j < 6; ++j) {
      for (unsigned int i = 0; i < 4; ++i) {
        z[i][j] = 0;
      }
    }

    move_exp.eval<2u>();

    for (unsigned int j = 0; j < 6; ++j) {
      for (unsigned int i = 0; i < 4; ++i) {
        if (j % 2 == 0) {
          CHECK(z[i][j] == ((x[i][j] + y[j]) * 2));
        } else {
          CHECK(z[i][j] == 0);
        }
      }
    }
  }

  SECTION("unbroadcastable arrays") {
    std::array<std::array<int, 4>, 2> x{{{{0, 0, 0, 0}}, {{10, 10, 10, 10}}}};
    std::array<int, 3> y{{1, 2, 3}};

    CHECK_THROWS(lazy::make_expression(add_y_to_x, std::ref(x), std::ref(y)));
  }
}
