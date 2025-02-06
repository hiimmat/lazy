

#include <catch2/catch_test_macros.hpp>
#include <vector>

import Lazy.Einsum;

// TODO: Unfinished

TEST_CASE("einsum parsing") {
  SECTION("ellipses") {
    static constexpr auto einsum_out = lazy::parse_einsum_input<"...a,...a->...", std::vector<std::vector<int>>,
                                                                std::vector<std::vector<std::vector<int>>>>();
    CHECK(einsum_out == lazy::string_literal("da,cda->cd"));
  }
}