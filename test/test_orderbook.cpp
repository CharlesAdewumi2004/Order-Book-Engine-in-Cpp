#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <vector>
#include <utility>
#include "OrderBook.hpp"
#include "OrderFactory.hpp"
#include "Interfaces/IEvent.hpp"
#include "Events/AddOrderEvent.hpp"
#include "Events/RemoveOrderEvent.hpp"

//––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––
// Dummy observer that records (event type, order ID) pairs
//––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––
struct DummyObserver : IOrderObserver {
    std::vector<std::pair<OrderEventType, std::string>> events;

    void onOrderEvent(std::shared_ptr<IEvent> event) override {
        if (!event) return;

        // Check dynamic type and extract the order and event type
        if (auto add = std::dynamic_pointer_cast<AddOrderEvent>(event)) {
            events.emplace_back(OrderEventType::ADD, add->getOrder()->getId());
        }
        else if (auto rem = std::dynamic_pointer_cast<RemoveOrderEvent>(event)) {
            events.emplace_back(OrderEventType::REMOVE, rem->getOrder()->getId());
        }
        else if (event->getType() == OrderEventType::MATCH) {
            auto order = event->getOrder();
            events.emplace_back(OrderEventType::MATCH, order->getId());
        }
    }
};

//––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––
// Helper to count specific (event type, ID) pairs
//––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––
static int countEvent(
    const std::vector<std::pair<OrderEventType, std::string>>& ev,
    OrderEventType want,
    const std::string& id
) {
    int c = 0;
    for (const auto& p : ev)
        if (p.first == want && p.second == id) ++c;
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
    REQUIRE(countEvent(obs->events, OrderEventType::ADD, o->getId()) == 1);
    REQUIRE(countEvent(obs->events, OrderEventType::REMOVE, o->getId()) == 0);
    REQUIRE(countEvent(obs->events, OrderEventType::MATCH, o->getId()) == 0);
}

TEST_CASE("Removing an order triggers REMOVE event only", "[OrderBook]") {
    OrderBook book;
    auto obs = std::make_shared<DummyObserver>();
    book.addObserver(obs);

    auto o = OrderFactory::createLimitOrder(3, 50, OrderType::SELL);
    book.addOrder(o);
    obs->events.clear();

    book.removeOrder(o);

    REQUIRE(obs->events.size() == 1);
    REQUIRE(countEvent(obs->events, OrderEventType::REMOVE, o->getId()) == 1);
    REQUIRE(countEvent(obs->events, OrderEventType::ADD, o->getId()) == 0);
    REQUIRE(countEvent(obs->events, OrderEventType::MATCH, o->getId()) == 0);
}

TEST_CASE("Matching BUY against existing SELL fires MATCH events", "[OrderBook]") {
    OrderBook book;
    auto obs = std::make_shared<DummyObserver>();
    book.addObserver(obs);

    auto sell = OrderFactory::createLimitOrder(3, 100, OrderType::SELL);
    book.addOrder(sell);
    obs->events.clear();

    auto buy = OrderFactory::createLimitOrder(5, 100, OrderType::BUY);
    book.addOrder(buy);

    REQUIRE(obs->events.size() == 3);
    REQUIRE(countEvent(obs->events, OrderEventType::ADD, buy->getId()) == 1);
    REQUIRE(countEvent(obs->events, OrderEventType::MATCH, buy->getId()) == 1);
    REQUIRE(countEvent(obs->events, OrderEventType::MATCH, sell->getId()) == 1);
}

TEST_CASE("Multiple partial matches across price levels", "[OrderBook]") {
    OrderBook book;
    auto obs = std::make_shared<DummyObserver>();
    book.addObserver(obs);

    auto A = OrderFactory::createLimitOrder(2, 99, OrderType::SELL);
    auto B = OrderFactory::createLimitOrder(3, 100, OrderType::SELL);
    book.addOrder(A);
    book.addOrder(B);
    obs->events.clear();

    auto buy = OrderFactory::createLimitOrder(4, 100, OrderType::BUY);
    book.addOrder(buy);

    int mBuy = countEvent(obs->events, OrderEventType::MATCH, buy->getId());
    int mA = countEvent(obs->events, OrderEventType::MATCH, A->getId());
    int mB = countEvent(obs->events, OrderEventType::MATCH, B->getId());

    REQUIRE(obs->events[0].first == OrderEventType::ADD);
    REQUIRE(mBuy == 2);
    REQUIRE(mA == 1);
    REQUIRE(mB == 1);
}
