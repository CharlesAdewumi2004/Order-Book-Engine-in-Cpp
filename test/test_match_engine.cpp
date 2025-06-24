#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>

#include "OrderFactory.hpp"
#include "MatchingEngine.hpp"

TEST_CASE("Basic match between buy and sell order", "[match]") {
    std::map<double, std::deque<std::shared_ptr<IOrder>>> sellOrders;

    // Create a sell order at price 100 for quantity 5
    auto sellOrder = OrderFactory::createLimitOrder(5, 100, OrderType::SELL);
    sellOrders[100].push_back(sellOrder);

    // Incoming buy order at price 100 for quantity 5
    auto buyOrder = OrderFactory::createLimitOrder(5, 100, OrderType::BUY);

    // Call matching engine
    auto trades = MatchingEngine::match(buyOrder, sellOrders);

    REQUIRE(trades.size() == 1);
    REQUIRE(trades[0].getBuyOrder()->getId() == buyOrder->getId());
    REQUIRE(trades[0].getSellOrder()->getId() == sellOrder->getId());
}

TEST_CASE("Partial match when incoming order has less quantity", "[match]") {
    std::map<double, std::deque<std::shared_ptr<IOrder>>> sellOrders;

    // Sell order has quantity 10
    auto sellOrder = OrderFactory::createLimitOrder(10, 100, OrderType::SELL);
    sellOrders[100].push_back(sellOrder);

    // Buy order has quantity 4
    auto buyOrder = OrderFactory::createLimitOrder(4, 100, OrderType::BUY);

    auto trades = MatchingEngine::match(buyOrder, sellOrders);

    REQUIRE(trades.size() == 1);
    REQUIRE(trades[0].getBuyOrder()->getId() == buyOrder->getId());
    REQUIRE(trades[0].getSellOrder()->getId() == sellOrder->getId());
}

TEST_CASE("Multiple matches with different price levels", "[match]") {
    std::map<double, std::deque<std::shared_ptr<IOrder>>> sellOrders;

    // Add 2 sell orders at different prices
    auto sell1 = OrderFactory::createLimitOrder(3, 99, OrderType::SELL);
    auto sell2 = OrderFactory::createLimitOrder(3, 100, OrderType::SELL);

    sellOrders[99].push_back(sell1);
    sellOrders[100].push_back(sell2);

    // Incoming buy order wants to buy 6 at price 100
    auto buyOrder = OrderFactory::createLimitOrder(6, 100, OrderType::BUY);

    auto trades = MatchingEngine::match(buyOrder, sellOrders);

    REQUIRE(trades.size() == 2);
    REQUIRE(trades[0].getSellOrder()->getId() == sell1->getId());
    REQUIRE(trades[1].getSellOrder()->getId() == sell2->getId());
}
