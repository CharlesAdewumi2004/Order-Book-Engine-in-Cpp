#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>

#include <memory>
#include <vector>
#include <utility>

#include "OrderBook.hpp"
#include "OrderFactory.hpp"
#include "OrderEventType.hpp"

//––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––
// Dummy observer that records (event, orderID) pairs
//––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––
struct DummyObserver : IOrderObserver {
    std::vector<std::pair<OrderEventType,std::string>> events;

    void onOrderEvent(OrderEventType event,
                      const std::shared_ptr<IOrder>& order) override {
        events.emplace_back(event, order->getId());
    }
};

//––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––
// Helpers
//––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––
// Find how many times a given (event,orderID) appears
static int countEvent(
    const std::vector<std::pair<OrderEventType,std::string>>& ev,
    OrderEventType want,
    const std::string& id
) {
    int c = 0;
    for (auto &p : ev)
        if (p.first==want && p.second==id) ++c;
    return c;
}

//––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––
// TESTS
//––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––

TEST_CASE("Adding an order triggers ADD event only", "[OrderBook]") {
    OrderBook book;
    auto obs = std::make_shared<DummyObserver>();
    book.addObserver(obs);

    auto o = OrderFactory::createLimitOrder(5, 100, OrderType::BUY);
    book.addOrder(o);

    REQUIRE(obs->events.size() == 1);
    REQUIRE(countEvent(obs->events, OrderEventType::ADD,   o->getId()) == 1);
    REQUIRE(countEvent(obs->events, OrderEventType::REMOVE,o->getId()) == 0);
    REQUIRE(countEvent(obs->events, OrderEventType::MATCH, o->getId()) == 0);
}

TEST_CASE("Removing an order triggers REMOVE event only", "[OrderBook]") {
    OrderBook book;
    auto obs = std::make_shared<DummyObserver>();
    book.addObserver(obs);

    auto o = OrderFactory::createLimitOrder(3,  50, OrderType::SELL);
    book.addOrder(o);
    obs->events.clear();

    book.removeOrder(o);

    REQUIRE(obs->events.size() == 1);
    REQUIRE(countEvent(obs->events, OrderEventType::REMOVE, o->getId()) == 1);
    REQUIRE(countEvent(obs->events, OrderEventType::ADD,    o->getId()) == 0);
    REQUIRE(countEvent(obs->events, OrderEventType::MATCH,  o->getId()) == 0);
}

TEST_CASE("Matching BUY against existing SELL fires MATCH events", "[OrderBook]") {
    OrderBook book;
    auto obs = std::make_shared<DummyObserver>();
    book.addObserver(obs);

    // Step 1: add a sell order at price=100, qty=3
    auto sell = OrderFactory::createLimitOrder(3, 100, OrderType::SELL);
    book.addOrder(sell);

    // Expect one ADD(sell)
    REQUIRE(obs->events.size() == 1);
    obs->events.clear();

    // Step 2: add a buy order at same price, qty=5 → matches qty=3
    auto buy = OrderFactory::createLimitOrder(5, 100, OrderType::BUY);
    book.addOrder(buy);

    // Expect events: ADD(buy), MATCH(buy), MATCH(sell)
    REQUIRE(obs->events.size() == 3);
    REQUIRE(countEvent(obs->events, OrderEventType::ADD,   buy->getId()) == 1);
    REQUIRE(countEvent(obs->events, OrderEventType::MATCH, buy->getId()) == 1);
    REQUIRE(countEvent(obs->events, OrderEventType::MATCH, sell->getId())==1);
}

TEST_CASE("Multiple partial matches across price levels", "[OrderBook]") {
    OrderBook book;
    auto obs = std::make_shared<DummyObserver>();
    book.addObserver(obs);

    // Sell side:
    //  - order A: qty=2 @99
    //  - order B: qty=3 @100
    auto A = OrderFactory::createLimitOrder(2, 99,  OrderType::SELL);
    auto B = OrderFactory::createLimitOrder(3, 100, OrderType::SELL);
    book.addOrder(A);
    book.addOrder(B);
    obs->events.clear();

    // Buy side incoming: qty=4 @100 → should match A then B (2+2)
    auto buy = OrderFactory::createLimitOrder(4, 100, OrderType::BUY);
    book.addOrder(buy);

    // Expect events: ADD(buy), MATCH(buy)*2, MATCH(A), MATCH(B)*?
    // Actually MatchingEngine returns two trades:
    //  - trade1: qty=2 with A
    //  - trade2: qty=2 with B
    // So MATCH(buy) twice, MATCH(A) once, MATCH(B) once
    int mBuy = countEvent(obs->events, OrderEventType::MATCH, buy->getId());
    int mA   = countEvent(obs->events, OrderEventType::MATCH, A->getId());
    int mB   = countEvent(obs->events, OrderEventType::MATCH, B->getId());

    REQUIRE(obs->events[0].first == OrderEventType::ADD);
    REQUIRE(mBuy == 2);
    REQUIRE(mA   == 1);
    REQUIRE(mB   == 1);
}

