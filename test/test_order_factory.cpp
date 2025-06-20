#include <catch2/catch_test_macros.hpp>
#include <memory>
#include "OrderFactory.hpp"
#include "LimitOrder.hpp"
#include <chrono>
#include <thread>

TEST_CASE("OrderFactory creates orders with correct attributes", "[OrderFactory]") {
    auto order1 = OrderFactory::createLimitOrder(100, 50, OrderType::BUY);
    auto order2 = OrderFactory::createLimitOrder(200, 75, OrderType::SELL);

    REQUIRE(order1->getQuantity() == 100);
    REQUIRE(order1->getPrice() == 50);
    REQUIRE(order1->getType() == OrderType::BUY);

    REQUIRE(order2->getQuantity() == 200);
    REQUIRE(order2->getPrice() == 75);
    REQUIRE(order2->getType() == OrderType::SELL);
}

TEST_CASE("OrderFactory generates unique incremental IDs", "[OrderFactory]") {
    auto orderA = OrderFactory::createLimitOrder(10, 20, OrderType::BUY);
    auto orderB = OrderFactory::createLimitOrder(15, 25, OrderType::SELL);

    REQUIRE(orderA->getId() != orderB->getId());
    REQUIRE(std::stoi(orderB->getId()) > std::stoi(orderA->getId()));
}

TEST_CASE("OrderFactory timestamps are correctly set", "[OrderFactory]") {
    auto before = std::chrono::system_clock::now();
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    auto order = OrderFactory::createLimitOrder(50, 100, OrderType::BUY);
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    auto after = std::chrono::system_clock::now();

    auto timestamp = order->getTimestamp();
    REQUIRE(timestamp >= before);
    REQUIRE(timestamp <= after);
}
