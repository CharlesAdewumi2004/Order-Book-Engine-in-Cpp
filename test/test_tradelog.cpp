// test/test_orderbook_events.cpp
#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <vector>

#include "OrderBook.hpp"
#include "OrderFactory.hpp"
#include "Interfaces/IEvent.hpp"
#include "Events/AddOrderEvent.hpp"
#include "Events/RemoveOrderEvent.hpp"
#include "Events/TradeEvent.hpp"

struct DummyObserver : IOrderObserver {
    std::vector<std::shared_ptr<IEvent>> events;
    void onOrderEvent(std::shared_ptr<IEvent> ev) override {
        events.push_back(std::move(ev));
    }
};

static int countType(const std::vector<std::shared_ptr<IEvent>>& evs, OrderEventType want) {
    return std::count_if(evs.begin(), evs.end(),
        [&](auto& e){ return e->getEventType()==want; });
}

TEST_CASE("AddOrder emits exactly one AddOrderEvent", "[OrderBook]") {
    OrderBook book;
    auto obs = std::make_shared<DummyObserver>();
    book.addObserver(obs);

    auto o = OrderFactory::createLimitOrder(7,  42, OrderType::BUY);
    book.addOrder(o);

    REQUIRE(obs->events.size() == 1);
    auto ev = obs->events[0];
    REQUIRE(ev->getEventType() == OrderEventType::ADD);

    // downcast and verify payload
    auto add = std::dynamic_pointer_cast<AddOrderEvent>(ev);
    REQUIRE(add);
    REQUIRE(add->getOrder()->getId() == o->getId());
}

TEST_CASE("RemoveOrder emits exactly one RemoveOrderEvent", "[OrderBook]") {
    OrderBook book;
    auto obs = std::make_shared<DummyObserver>();
    book.addObserver(obs);

    auto o = OrderFactory::createLimitOrder(3, 99, OrderType::SELL);
    book.addOrder(o);
    obs->events.clear();

    book.removeOrder(o);
    REQUIRE(obs->events.size() == 1);
    auto ev = obs->events.front();
    REQUIRE(ev->getEventType() == OrderEventType::REMOVE);

    auto rem = std::dynamic_pointer_cast<RemoveOrderEvent>(ev);
    REQUIRE(rem);
    REQUIRE(rem->getOrder()->getId() == o->getId());
}

TEST_CASE("Single full match emits one TradeEvent", "[OrderBook][match]") {
    OrderBook book;
    auto obs = std::make_shared<DummyObserver>();
    book.addObserver(obs);

    // seed a resting sell
    auto sell = OrderFactory::createLimitOrder(5, 50, OrderType::SELL);
    book.addOrder(sell);
    obs->events.clear();

    // incoming buy that fully matches
    auto buy = OrderFactory::createLimitOrder(5, 50, OrderType::BUY);
    book.addOrder(buy);

    // should see: 1 ADD(buy) + 1 MATCH
    REQUIRE(obs->events.size() == 2);
    REQUIRE(countType(obs->events, OrderEventType::ADD)   == 1);
    REQUIRE(countType(obs->events, OrderEventType::MATCH) == 1);

    auto tev = std::dynamic_pointer_cast<TradeEvent>(obs->events.back());
    REQUIRE(tev);
    REQUIRE(tev->getBuyOrder()->getId()  == buy->getId());
    REQUIRE(tev->getSellOrder()->getId() == sell->getId());
    REQUIRE(tev->getQty() == 5);
    REQUIRE(tev->getPrice() == 50.0);
}

TEST_CASE("Partial match leaves leftover and emits two TradeEvents", "[OrderBook][match]") {
    OrderBook book;
    auto obs = std::make_shared<DummyObserver>();
    book.addObserver(obs);

    // resting sell qty=10@100
    auto sell = OrderFactory::createLimitOrder(10, 100, OrderType::SELL);
    book.addOrder(sell);
    obs->events.clear();

    // incoming buy qty=4@100 => one partial trade
    auto buy = OrderFactory::createLimitOrder(4, 100, OrderType::BUY);
    book.addOrder(buy);

    // events: ADD(buy), MATCH
    REQUIRE(countType(obs->events, OrderEventType::ADD)   == 1);
    REQUIRE(countType(obs->events, OrderEventType::MATCH) == 1);

    // check the resting sell has been reduced
    REQUIRE(sell->getQuantity() == 6);

    // inspect TradeEvent
    auto tev = std::dynamic_pointer_cast<TradeEvent>(obs->events.back());
    REQUIRE(tev->getQty() == 4);
}

TEST_CASE("Multiple price‐level matches emit one TradeEvent per fill", "[OrderBook][match]") {
    OrderBook book;
    auto obs = std::make_shared<DummyObserver>();
    book.addObserver(obs);

    // two resting sells at 99(qty=2) and 100(qty=3)
    auto A = OrderFactory::createLimitOrder(2, 99,  OrderType::SELL);
    auto B = OrderFactory::createLimitOrder(3, 100, OrderType::SELL);
    book.addOrder(A);
    book.addOrder(B);
    obs->events.clear();

    // buy qty=4@100 => should match A(2) then B(2)
    auto buy = OrderFactory::createLimitOrder(4, 100, OrderType::BUY);
    book.addOrder(buy);

    // expect 1 ADD + 2 MATCH events
    REQUIRE(countType(obs->events, OrderEventType::ADD)   == 1);
    REQUIRE(countType(obs->events, OrderEventType::MATCH) == 2);

    // first trade against A, second against B
    auto tev1 = std::dynamic_pointer_cast<TradeEvent>(obs->events[1]);
    auto tev2 = std::dynamic_pointer_cast<TradeEvent>(obs->events[2]);
    REQUIRE(tev1);
    REQUIRE(tev2);
    REQUIRE(tev1->getSellOrder()->getId() == A->getId());
    REQUIRE(tev2->getSellOrder()->getId() == B->getId());
}
