# Lazy: A C++ Library for Lazy Evaluation
Lazy is a C++20 library that offers a generalized approach to expression templates through the `expression` class. Expression objects are created by specifying an invocable along with its required arguments or arguments that can be converted to match the parameters expected by the invocable. This enables passing references or pointers instead of exact invocable arguments, reducing potential memory overhead or premature deallocation before evaluation.

Additionally, `expression` objects support composition, enabling them to be nested within other `expressions` so that the output of one can serve as the input for another. A key requirement for this is that the output rank of one expression matches the input rank of the next.

For expressions involving multiple multidimensional arrays, their dimensions must either be identical or broadcast-compatible.

## Einsum Notation (Planned Feature)
Future development will introduce einsum notation, addressing the current limitation where multidimensional arrays are processed strictly from the outermost to the innermost dimension. With einsum notation, users will have greater control over iteration order and output structure, enabling operations such as matrix multiplication, Hadamard product, transposition, and more.

At present, only the parsing and validation of einsum strings has been implemented. The final implementation will integrate einsum notation as a template parameter within the `expression` class, ensuring zero runtime overhead while maintaining compatibility with existing features like nested expressions, broadcasting, and compile-time shape validation.


# Examples

## Expressions with scalars as inputs

```
auto fn = [](int x, int y) { return x + y; };
auto exp = lazy::make_expression(fn, 7, 5);
exp(); // Returns 12
```

## Passing elements to expression as pointers
```
auto inv = [](int x) { return x; };

auto exp = [&inv]() {
  auto x = std::make_unique<int>(5);
  auto exp = lazy::make_expression(inv, std::move(x));
  return exp;
}();

// The ownership of the unique ptr was transferred to the expression. And expression unwraps the pointer before
// passing it to the "inv" function.
std::cout << exp() << std::endl;
```

## Expression with a one dimensional vector as an input
```
std::vector<double> vec = {  1, 2, 3, 4, 5, 6, 7, 8 };
int e = 2;
auto fn = [](double x, int e) { std::cout << ldexp(x, e) << std::endl; };
auto exp = lazy::make_expression(fn, vec, e);

std::cout << "eval:\n";
exp.eval(); // Prints each vector element multiplied with 2 raised to the power of e.

std::cout << "\neval at:\n";
exp.eval_at(3u); // Prints the 4th vector element multiplied with 2 raised to the power of e (expression is zero-indexed).

std::cout << "\nsubscript operator:\n";
exp[4u]; // Prints the 5th vector element multiplied with 2 raised to the power of e (expression is zero-indexed).

std::cout << "\neval every second element:\n";
exp.eval<2u>(); // Prints every second vector element multiplied with 2 raised to the power of e.
```

## Expression with containers of different ranks (broadcasting)
```
std::array<std::array<std::array<std::array<int, 2>, 4>, 6>, 8> x;
std::vector<std::vector<std::vector<int>>> y;
std::array<std::array<std::array<std::array<int, 2>, 4>, 6>, 8> z;

y = std::vector<std::vector<std::vector<int>>>(6, std::vector<std::vector<int>>(1, std::vector<int>(2)));

// Assign elements to x
for (unsigned int l = 0; l < 2; ++l) {
  for (unsigned int k = 0; k < 4; ++k) {
    for (unsigned int j = 0; j < 6; ++j) {
      for (unsigned int i = 0; i < 8; ++i) {
        x[i][j][k][l] = 1;
      }
    }
  }
}

// Assign elements to y
for (int j = 0; j < 2; ++j) {
  for (int i = 0; i < 6; ++i) {
    y[i][0][j] = i + j * 6;
  }
}

auto fn = [](int x, int y, int& z) { z = x + y; };
auto exp = lazy::make_expression(fn, std::cref(x), std::cref(y), std::ref(z));  // pass elements as references
exp.eval(); // the array z now contains the result of broadcasted addition x + y
```

## Expression which needs to partially unwrap input arrays
```
std::array<std::array<int, 3>, 4> x;
std::array<std::array<int, 3>, 4> y;
std::array<int, 4> z;

for (unsigned int j = 0; j < 3; ++j) {
  for (unsigned int i = 0; i < 4; ++i) {
    x[i][j] = i + 3 * j;
    y[i][j] = i + 3 * j;
  }
}

auto fn = [](const std::array<int, 3>& x, const std::array<int, 3>& y, int& z) { z = x[0] + y[0]; };
auto exp = lazy::make_expression(fn, std::cref(x), std::cref(y), std::ref(z));

// x and y are just partially unwrapped here
exp.eval();
```

## Nested expressions
```
std::array<std::array<int, 3>, 4> x{{{{0, 0, 0}}, {{10, 10, 10}}, {{20, 20, 20}}, {{30, 30, 30}}}};
std::array<int, 3> y{{1, 2, 3}};
std::array<std::array<int, 3>, 4> z;

auto addition = [](int x, int y) { return x + y; };
auto addition_exp = lazy::make_expression(addition, x, y);

auto multiplication = [](int x, int y) { std::cout << x * y << std::endl; };
auto multiplication_exp = lazy::make_expression(multiplication, addition_exp, 2);

multiplication_exp.eval();
```

# Building and Installation

## With CMake
`lazy` can be included using `add_subdirectory`:

```
  add_subdirectory(${lazy_SOURCE_DIR} ${lazy_BINARY_DIR}
                   EXCLUDE_FROM_ALL)
```


Another option is to use `FetchContent`:

```
include(FetchContent)

FetchContent_Declare(
    lazy
    GIT_REPOSITORY https://github.com/hiimmat/lazy
    GIT_TAG 0.1.0
)

FetchContent_MakeAvailable(lazy)
```

### Notes regarding unit tests
- If `lazy` is included and the `EXCLUDE_FROM_ALL` flag is set, `lazy` tests can be disabled by setting the `LAZY_BUILD_TESTS` option to OFF.
- To build the unit tests, the Catch2 unit testing framework is required. If it's not already available on the system, CMake will attempt to download it.


## Without CMake
Apart from Catch2, the library has no other external dependencies. So in case if the tests aren't needed, the files can be included into another project as is.


# Requirements
The code has been tested using GCC 14.1.0 and CMake 3.30.2, but any standard compiler supporting C++20 modules should work.

- C++ compiler (such as GCC 14 and newer, or LLVM/Clang 16.0 and newer)
- CMake 3.28.2 or higher (optional)

# License
`lazy` is licensed under the MIT License. See `license.txt` for more details.
