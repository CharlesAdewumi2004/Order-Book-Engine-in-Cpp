#include <catch2/catch_test_macros.hpp>
#include "OrderBook.hpp"
#include "OrderFactory.hpp"
#include "Observer/TradeLog.hpp"
#include "Events/AddOrderEvent.hpp"
#include "Events/RemoveOrderEvent.hpp"
#include "Events/TradeEvent.hpp"

TEST_CASE("Comprehensive OrderBook + TradeLog Integration", "[integration]") {
    // 1) Setup
    OrderBook book;
    auto log = std::make_shared<TradeLog>("trades_test.jsonl");
    book.addObserver(log);

    // 2) Add two buy orders at different prices
    auto B1 = OrderFactory::createLimitOrder(5, 100, OrderType::BUY);
    book.addOrder(B1);

    auto B2 = OrderFactory::createLimitOrder(2, 101, OrderType::BUY);
    book.addOrder(B2);

    // Best bid should be 101
    {
        auto bids = book.getBuyOrders();
        REQUIRE(!bids.empty());
        REQUIRE(bids.begin()->first == 101);
    }

    // 3) Add a sell order that partially matches across both
    //    Sell 3 @100: matches B2 (2) then B1 (1)
    auto S1 = OrderFactory::createLimitOrder(3, 100, OrderType::SELL);
    book.addOrder(S1);

    // After matching: B2 gone, B1 has 4 left
    {
        auto bids = book.getBuyOrders();
        REQUIRE(bids.size() == 1);
        auto &dq = bids.begin()->second;
        REQUIRE(dq.size() == 1);
        REQUIRE(dq.front()->getId() == B1->getId());
        REQUIRE(dq.front()->getQuantity() == 4);
    }
    REQUIRE(book.getSellOrders().empty());

    // 4) Add a sell order that does NOT match (price too high)
    auto S2 = OrderFactory::createLimitOrder(1, 102, OrderType::SELL);
    book.addOrder(S2);
    {
        auto asks = book.getSellOrders();
        REQUIRE(asks.size() == 1);
        REQUIRE(asks.begin()->first == 102);
    }

    // 5) Cancel the remaining buy order B1
    book.removeOrder(B1);
    REQUIRE(book.getBuyOrders().empty());

    // 6) Cancel the unmatched sell order S2
    book.removeOrder(S2);
    REQUIRE(book.getSellOrders().empty());

    // 7) Verify the exact sequence of events in the TradeLog
    auto &events = log->getEvents();
    REQUIRE(events.size() == 8);

    // 0: AddOrderEvent(B1)
    REQUIRE(events[0]->getEventType() == OrderEventType::ADD);
    {
        auto *e = dynamic_cast<AddOrderEvent*>(events[0].get());
        REQUIRE(e->getOrder()->getId() == B1->getId());
    }

    // 1: AddOrderEvent(B2)
    REQUIRE(events[1]->getEventType() == OrderEventType::ADD);
    {
        auto *e = dynamic_cast<AddOrderEvent*>(events[1].get());
        REQUIRE(e->getOrder()->getId() == B2->getId());
    }

    // 2: AddOrderEvent(S1)
    REQUIRE(events[2]->getEventType() == OrderEventType::ADD);
    {
        auto *e = dynamic_cast<AddOrderEvent*>(events[2].get());
        REQUIRE(e->getOrder()->getId() == S1->getId());
    }

    // 3 & 4: TradeEvent(B2↔S1 qty=2, B1↔S1 qty=1)
    REQUIRE(events[3]->getEventType() == OrderEventType::MATCH);
    REQUIRE(events[4]->getEventType() == OrderEventType::MATCH);
    {
        auto *t1 = dynamic_cast<TradeEvent*>(events[3].get());
        REQUIRE(t1->getBuyOrder()->getId()  == B2->getId());
        REQUIRE(t1->getSellOrder()->getId() == S1->getId());
        REQUIRE(t1->getQty()               == 2);

        auto *t2 = dynamic_cast<TradeEvent*>(events[4].get());
        REQUIRE(t2->getBuyOrder()->getId()  == B1->getId());
        REQUIRE(t2->getSellOrder()->getId() == S1->getId());
        REQUIRE(t2->getQty()               == 1);
    }

    // 5: AddOrderEvent(S2)
    REQUIRE(events[5]->getEventType() == OrderEventType::ADD);
    {
        auto *e = dynamic_cast<AddOrderEvent*>(events[5].get());
        REQUIRE(e->getOrder()->getId() == S2->getId());
    }

    // 6: RemoveOrderEvent(B1)
    REQUIRE(events[6]->getEventType() == OrderEventType::REMOVE);
    {
        auto *e = dynamic_cast<RemoveOrderEvent*>(events[6].get());
        REQUIRE(e->getOrder()->getId() == B1->getId());
    }

    // 7: RemoveOrderEvent(S2)
    REQUIRE(events[7]->getEventType() == OrderEventType::REMOVE);
    {
        auto *e = dynamic_cast<RemoveOrderEvent*>(events[7].get());
        REQUIRE(e->getOrder()->getId() == S2->getId());
    }
}
