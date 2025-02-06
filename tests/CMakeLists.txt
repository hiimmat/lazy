cmake_minimum_required(VERSION 3.28)

Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.8.0
)

FetchContent_MakeAvailable(Catch2)

add_executable(lazy_tests)

target_sources(lazy_tests
        PRIVATE FILE_SET HEADERS
            BASE_DIRS ${PROJECT_SOURCE_DIR}/include
            FILES ${PROJECT_SOURCE_DIR}/include/assert.hpp)

target_sources(lazy_tests
        PRIVATE FILE_SET CXX_MODULES
            BASE_DIRS ${PROJECT_SOURCE_DIR}/include
                  FILES ${PROJECT_SOURCE_DIR}/include/array_traits.cppm
                  ${PROJECT_SOURCE_DIR}/include/einsum.cppm
                  ${PROJECT_SOURCE_DIR}/include/expression.cppm
                  ${PROJECT_SOURCE_DIR}/include/invocable_traits.cppm
                  ${PROJECT_SOURCE_DIR}/include/string_literal.cppm
                  ${PROJECT_SOURCE_DIR}/include/utilities.cppm
)

target_sources(lazy_tests
    PRIVATE
        src/main_tests.cpp
        src/test_array_traits.cpp
        src/test_einsum.cpp
        src/test_expression.cpp
        src/test_string_literal.cpp
)

target_link_libraries(lazy_tests PRIVATE Catch2::Catch2WithMain)

target_compile_features(lazy_tests PRIVATE cxx_std_20)

add_compile_definitions(LAZY_ENABLE_EXPECTS=TRUE)
add_compile_definitions(LAZY_ENABLE_ENSURES=TRUE)
add_compile_definitions(LAZY_THROW_ON_CONTRACT_VIOLATION=TRUE)

add_test(NAME lazy_tests COMMAND lazy_tests)
