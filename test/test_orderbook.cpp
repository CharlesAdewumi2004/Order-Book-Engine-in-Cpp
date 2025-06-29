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

struct RecordingObserver : IOrderObserver {
    std::vector<std::shared_ptr<IEvent>> receivedEvents;
    void onOrderEvent(std::shared_ptr<IEvent> ev) override {
        receivedEvents.push_back(std::move(ev));
    }
};

static int countType(const std::vector<std::shared_ptr<IEvent>>& evs, OrderEventType type) {
    return std::count_if(evs.begin(), evs.end(),
        [&](auto& e){ return e->getEventType()==type; });
}

TEST_CASE("AddOrder emits one AddOrderEvent", "[OrderBook]") {
    OrderBook book;
    auto obs = std::make_shared<RecordingObserver>();
    book.addObserver(obs);

    auto o = OrderFactory::createLimitOrder(7,  42, OrderType::BUY);
    book.addOrder(o);

    REQUIRE(obs->receivedEvents.size() == 1);
    REQUIRE(countType(obs->receivedEvents, OrderEventType::ADD) == 1);
    auto ev = std::dynamic_pointer_cast<AddOrderEvent>(obs->receivedEvents[0]);
    REQUIRE(ev);
    REQUIRE(ev->getOrder()->getId() == o->getId());
}

TEST_CASE("RemoveOrder emits one RemoveOrderEvent", "[OrderBook]") {
    OrderBook book;
    auto obs = std::make_shared<RecordingObserver>();
    book.addObserver(obs);

    auto o = OrderFactory::createLimitOrder(3, 99, OrderType::SELL);
    book.addOrder(o);
    obs->receivedEvents.clear();

    book.removeOrder(o);

    REQUIRE(obs->receivedEvents.size() == 1);
    REQUIRE(countType(obs->receivedEvents, OrderEventType::REMOVE) == 1);
    auto ev = std::dynamic_pointer_cast<RemoveOrderEvent>(obs->receivedEvents[0]);
    REQUIRE(ev);
    REQUIRE(ev->getOrder()->getId() == o->getId());
}

TEST_CASE("Single fill produces exactly one TradeEvent", "[OrderBook][match]") {
    OrderBook book;
    auto obs = std::make_shared<RecordingObserver>();
    book.addObserver(obs);

    // seed a resting SELL
    auto sell = OrderFactory::createLimitOrder(5, 50, OrderType::SELL);
    book.addOrder(sell);
    obs->receivedEvents.clear();

    // incoming BUY at same price/size → full match
    auto buy = OrderFactory::createLimitOrder(5, 50, OrderType::BUY);
    book.addOrder(buy);

    // Expect 1 ADD + 1 MATCH
    REQUIRE(countType(obs->receivedEvents, OrderEventType::ADD)   == 1);
    REQUIRE(countType(obs->receivedEvents, OrderEventType::MATCH) == 1);

    auto te = std::dynamic_pointer_cast<TradeEvent>(
        *std::find_if(obs->receivedEvents.begin(), obs->receivedEvents.end(),
            [](auto& e){ return e->getEventType()==OrderEventType::MATCH; })
    );
    REQUIRE(te);
    CHECK(te->getQty() == 5);
    CHECK(te->getPrice() == 50.0);
    CHECK(te->getBuyOrder()->getOrderType()  == OrderType::BUY);
    CHECK(te->getSellOrder()->getOrderType() == OrderType::SELL);
}

TEST_CASE("Partial fill leaves remainder and emits one TradeEvent", "[OrderBook][match]") {
    OrderBook book;
    auto obs = std::make_shared<RecordingObserver>();
    book.addObserver(obs);

    // resting SELL qty=10
    auto sell = OrderFactory::createLimitOrder(10, 100, OrderType::SELL);
    book.addOrder(sell);
    obs->receivedEvents.clear();

    // incoming BUY qty=4 → partial fill
    auto buy = OrderFactory::createLimitOrder(4, 100, OrderType::BUY);
    book.addOrder(buy);

    REQUIRE(countType(obs->receivedEvents, OrderEventType::MATCH) == 1);
    auto te = std::dynamic_pointer_cast<TradeEvent>(obs->receivedEvents.back());
    REQUIRE(te);
    CHECK(te->getQty() == 4);
    CHECK(sell->getQuantity() == 6);  // 10−4 left
}

TEST_CASE("Multi‐level fill emits one TradeEvent per fill", "[OrderBook][match]") {
    OrderBook book;
    auto obs = std::make_shared<RecordingObserver>();
    book.addObserver(obs);

    // two SELL orders: (2@99) and (3@100)
    auto A = OrderFactory::createLimitOrder(2,  99, OrderType::SELL);
    auto B = OrderFactory::createLimitOrder(3, 100, OrderType::SELL);
    book.addOrder(A);
    book.addOrder(B);
    obs->receivedEvents.clear();

    // BUY qty=4@100 → matches A(2) then B(2)
    auto buy = OrderFactory::createLimitOrder(4, 100, OrderType::BUY);
    book.addOrder(buy);

    REQUIRE(countType(obs->receivedEvents, OrderEventType::MATCH) == 2);

    // verify the two TradeEvents in order:
    std::vector<std::shared_ptr<TradeEvent>> trades;
    for(auto& e: obs->receivedEvents){
      if(e->getEventType()==OrderEventType::MATCH)
        trades.push_back(std::dynamic_pointer_cast<TradeEvent>(e));
    }
    REQUIRE(trades.size()==2);
    CHECK(trades[0]->getSellOrder()->getId() == A->getId());
    CHECK(trades[0]->getQty() == 2);
    CHECK(trades[1]->getSellOrder()->getId() == B->getId());
    CHECK(trades[1]->getQty() == 2);
}
