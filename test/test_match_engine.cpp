#include <iostream>
#include <catch2/catch_test_macros.hpp>

#include "OrderFactory.hpp"
#include "MatchingEngine.hpp"

using Book = std::map<double, std::deque<std::shared_ptr<IOrder>>>;

//
// Helpers
//
static int totalQuantity(const Book& b) {
    int sum = 0;
    for (auto const& [price, dq] : b)
        for (auto const& o : dq)
            sum += o->getQuantity();
    return sum;
}

TEST_CASE("No match when incoming price too low", "[match][nomatch]") {
    Book sellBook;
    auto sell = OrderFactory::createLimitOrder(5, 100, OrderType::SELL);
    sellBook[100].push_back(sell);

    auto buy = OrderFactory::createLimitOrder(5,  99, OrderType::BUY);
    auto trades = MatchingEngine::match(buy, sellBook);

    REQUIRE(trades.empty());
    // book unchanged
    REQUIRE(sellBook.size() == 1);
    REQUIRE(sellBook.at(100).size() == 1);
}

TEST_CASE("Complete fill removes price level", "[match][cleanup]") {
    Book sellBook;
    auto sell = OrderFactory::createLimitOrder(5, 100, OrderType::SELL);
    sellBook[100].push_back(sell);

    auto buy = OrderFactory::createLimitOrder(5, 100, OrderType::BUY);
    auto trades = MatchingEngine::match(buy, sellBook);

    REQUIRE(trades.size() == 1);
    // price level removed
    REQUIRE(sellBook.empty());
}

TEST_CASE("Partial fill leaves leftover resting order", "[match][partial]") {
    Book sellBook;
    auto sell = OrderFactory::createLimitOrder(10, 100, OrderType::SELL);
    sellBook[100].push_back(sell);

    auto buy = OrderFactory::createLimitOrder(4, 100, OrderType::BUY);
    auto trades = MatchingEngine::match(buy, sellBook);

    REQUIRE(trades.size() == 1);
    // remaining 6 on the resting
    REQUIRE(sellBook.size() == 1);
    REQUIRE(sellBook.at(100).front()->getQuantity() == 6);
}

TEST_CASE("Incoming order larger than book total", "[match][overflow]") {
    Book sellBook;
    // two orders: total 8
    sellBook[100].push_back(OrderFactory::createLimitOrder(3, 100, OrderType::SELL));
    sellBook[100].push_back(OrderFactory::createLimitOrder(5, 100, OrderType::SELL));

    auto buy = OrderFactory::createLimitOrder(10, 100, OrderType::BUY);
    auto trades = MatchingEngine::match(buy, sellBook);

    // should produce 2 trades
    REQUIRE(trades.size() == 2);
    // book now empty
    REQUIRE(sellBook.empty());
}

TEST_CASE("Time-priority at same price", "[match][time]") {
    Book sellBook;
    auto first  = OrderFactory::createLimitOrder(2, 100, OrderType::SELL);
    auto second = OrderFactory::createLimitOrder(3, 100, OrderType::SELL);
    sellBook[100].push_back(first);
    sellBook[100].push_back(second);

    auto buy = OrderFactory::createLimitOrder(4, 100, OrderType::BUY);
    auto trades = MatchingEngine::match(buy, sellBook);

    REQUIRE(trades.size() == 2);
    // first match must be against `first`
    REQUIRE(trades[0].getSellOrder()->getId() == first->getId());
    // second against `second`
    REQUIRE(trades[1].getSellOrder()->getId() == second->getId());
}

TEST_CASE("Price-priority across levels", "[match][price]") {
    Book sellBook;
    // cheaper first
    auto cheap = OrderFactory::createLimitOrder(1,  99, OrderType::SELL);
    auto expen = OrderFactory::createLimitOrder(1, 100, OrderType::SELL);
    sellBook[100].push_back(expen);
    sellBook[ 99].push_back(cheap);

    auto buy = OrderFactory::createLimitOrder(2, 100, OrderType::BUY);
    auto trades = MatchingEngine::match(buy, sellBook);

    REQUIRE(trades.size() == 2);
    // first should match cheap @99
    REQUIRE(trades[0].getSellOrder()->getId() == cheap->getId());
    // then expensive @100
    REQUIRE(trades[1].getSellOrder()->getId() == expen->getId());
}

TEST_CASE("Matching a SELL order against BUY book", "[match][sell]") {
    Book buyBook;
    auto b1 = OrderFactory::createLimitOrder(4,  50, OrderType::BUY);
    auto b2 = OrderFactory::createLimitOrder(4,  49, OrderType::BUY);

    buyBook[49].push_back(b1);
    buyBook[50].push_back(b2);

    auto sell = OrderFactory::createLimitOrder(6, 49, OrderType::SELL);
    auto trades = MatchingEngine::match(sell, buyBook);

    REQUIRE(trades.size() == 2);
    INFO("order 1");
    INFO("Trade[0] ID:    " << trades[0].getBuyOrder()->getId());
    INFO("Expected b1 ID: " << b1->getId());
    REQUIRE(trades[0].getBuyOrder()->getId() == b1->getId());

    INFO("Order 2");
    INFO("Trade[1] ID:    " << trades[1].getBuyOrder()->getId());
    INFO("Expected b2 ID: " << b2->getId());
    REQUIRE(trades[1].getBuyOrder()->getId() == b2->getId());

}

TEST_CASE("Empty opposing book yields no trades", "[match][empty]") {
    Book empty;
    auto buy = OrderFactory::createLimitOrder(5, 100, OrderType::BUY);
    auto trades = MatchingEngine::match(buy, empty);

    REQUIRE(trades.empty());
    REQUIRE(empty.empty());
}
