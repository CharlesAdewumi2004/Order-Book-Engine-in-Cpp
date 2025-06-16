#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>


// This replicates the function from main.cpp
int add(int a, int b) {
    return a + b;
}

TEST_CASE("Addition works", "[add]") {
    REQUIRE(add(1, 1) == 2);
    REQUIRE(add(2, 3) == 5);
    REQUIRE(add(-1, 1) == 0);
    REQUIRE(add(0, 0) == 0);
}
