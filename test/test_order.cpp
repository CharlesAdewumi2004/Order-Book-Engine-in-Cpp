#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>
#include "LimitOrder.hpp"
#include <chrono>

TEST_CASE("LimitOrder correctly stores and returns its values", "[order]") {
     std::string id = "101";
    OrderType type = OrderType::BUY;
    double price = 99.5;
    int quantity = 10;
    std::chrono::system_clock::time_point timestamp = std::chrono::system_clock::now();

    LimitOrder order(id, type, price, quantity, timestamp);

    REQUIRE(order.getId() == id);
    REQUIRE(order.getType() == OrderType::BUY);
    REQUIRE(order.getPrice() == Catch::Approx(99.5));
    REQUIRE(order.getQuantity() == 10);
    REQUIRE(order.getTimestamp() == timestamp);
}

TEST_CASE("LimitOrder handles SELL type correctly", "[order]") {
    LimitOrder order("202", OrderType::SELL, 120.25, 5, std::chrono::system_clock::now());

    REQUIRE(order.getType() == OrderType::SELL);
    REQUIRE(order.getPrice() > 0);
    REQUIRE(order.getQuantity() > 0);
}
