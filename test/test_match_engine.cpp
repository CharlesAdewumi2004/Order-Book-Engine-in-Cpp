#include <catch2/catch_test_macros.hpp>
#include "OrderFactory.hpp"
#include "MatchingEngine.hpp"

#include <map>
#include <deque>
#include <memory>

// ——————————————————————————————————————————
// Map aliases matching your engine signature
// ——————————————————————————————————————————
using BuyBook  = std::map<double, std::deque<std::shared_ptr<IOrder>>, std::greater<>>;
using SellBook = std::map<double, std::deque<std::shared_ptr<IOrder>>>;

// Helper to sum quantities (if needed)
static int totalQuantity(const SellBook& b) {
    int sum = 0;
    for (auto const& [price, dq] : b)
        for (auto const& o : dq)
            sum += o->getQuantity();
    return sum;
}

// ——————————————————————————————————————————
// TESTS
// ——————————————————————————————————————————

TEST_CASE("No match when incoming price too low", "[match][nomatch]") {
    BuyBook  buyBook;
    SellBook sellBook;

    auto sell   = OrderFactory::createLimitOrder(5, 100, OrderType::SELL);
    sellBook[100].push_back(sell);

    auto buy     = OrderFactory::createLimitOrder(5,  99, OrderType::BUY);
    auto trades  = MatchingEngine::match(buy, buyBook, sellBook);

    REQUIRE(trades.empty());
    REQUIRE(sellBook.size() == 1);
    REQUIRE(sellBook.at(100).size() == 1);
}

TEST_CASE("Complete fill removes price level", "[match][cleanup]") {
    BuyBook  buyBook;
    SellBook sellBook;

    auto sell   = OrderFactory::createLimitOrder(5, 100, OrderType::SELL);
    sellBook[100].push_back(sell);

    auto buy     = OrderFactory::createLimitOrder(5, 100, OrderType::BUY);
    auto trades  = MatchingEngine::match(buy, buyBook, sellBook);

    REQUIRE(trades.size() == 1);
    REQUIRE(sellBook.empty());
}

TEST_CASE("Partial fill leaves leftover resting order", "[match][partial]") {
    BuyBook  buyBook;
    SellBook sellBook;

    auto sell   = OrderFactory::createLimitOrder(10, 100, OrderType::SELL);
    sellBook[100].push_back(sell);

    auto buy     = OrderFactory::createLimitOrder(4, 100, OrderType::BUY);
    auto trades  = MatchingEngine::match(buy, buyBook, sellBook);

    REQUIRE(trades.size() == 1);
    REQUIRE(sellBook.size() == 1);
    REQUIRE(sellBook.at(100).front()->getQuantity() == 6);
}

TEST_CASE("Incoming order larger than book total", "[match][overflow]") {
    BuyBook  buyBook;
    SellBook sellBook;

    sellBook[100].push_back(
      OrderFactory::createLimitOrder(3, 100, OrderType::SELL));
    sellBook[100].push_back(
      OrderFactory::createLimitOrder(5, 100, OrderType::SELL));

    auto buy     = OrderFactory::createLimitOrder(10, 100, OrderType::BUY);
    auto trades  = MatchingEngine::match(buy, buyBook, sellBook);

    REQUIRE(trades.size() == 2);
    REQUIRE(sellBook.empty());
}

TEST_CASE("Time-priority at same price", "[match][time]") {
    BuyBook  buyBook;
    SellBook sellBook;

    auto first  = OrderFactory::createLimitOrder(2, 100, OrderType::SELL);
    auto second = OrderFactory::createLimitOrder(3, 100, OrderType::SELL);
    sellBook[100].push_back(first);
    sellBook[100].push_back(second);

    auto buy     = OrderFactory::createLimitOrder(4, 100, OrderType::BUY);
    auto trades  = MatchingEngine::match(buy, buyBook, sellBook);

    REQUIRE(trades.size() == 2);
    REQUIRE(trades[0].getSellOrder()->getId() == first->getId());
    REQUIRE(trades[1].getSellOrder()->getId() == second->getId());
}

TEST_CASE("Price-priority across levels", "[match][price]") {
    BuyBook  buyBook;
    SellBook sellBook;

    auto cheap  = OrderFactory::createLimitOrder(1,  99, OrderType::SELL);
    auto expen  = OrderFactory::createLimitOrder(1, 100, OrderType::SELL);
    sellBook[100].push_back(expen);
    sellBook[ 99].push_back(cheap);

    auto buy     = OrderFactory::createLimitOrder(2, 100, OrderType::BUY);
    auto trades  = MatchingEngine::match(buy, buyBook, sellBook);

    REQUIRE(trades.size() == 2);
    REQUIRE(trades[0].getSellOrder()->getId() == cheap->getId());
    REQUIRE(trades[1].getSellOrder()->getId() == expen->getId());
}

TEST_CASE("Matching a SELL order against BUY book", "[match][sell]") {
    BuyBook  buyBook;
    SellBook sellBook;

    auto b1 = OrderFactory::createLimitOrder(4,  50, OrderType::BUY);
    auto b2 = OrderFactory::createLimitOrder(4,  49, OrderType::BUY);
    buyBook[50].push_back(b1);
    buyBook[49].push_back(b2);

    auto sell   = OrderFactory::createLimitOrder(6, 49, OrderType::SELL);
    auto trades = MatchingEngine::match(sell, buyBook, sellBook);

    REQUIRE(trades.size() == 2);
    REQUIRE(trades[0].getBuyOrder()->getId() == b1->getId());
    REQUIRE(trades[1].getBuyOrder()->getId() == b2->getId());
}

TEST_CASE("Empty opposing book yields no trades", "[match][empty]") {
    BuyBook  buyBook;
    SellBook sellBook;

    auto buy     = OrderFactory::createLimitOrder(5, 100, OrderType::BUY);
    auto trades  = MatchingEngine::match(buy, buyBook, sellBook);

    REQUIRE(trades.empty());
    REQUIRE(sellBook.empty());
}
