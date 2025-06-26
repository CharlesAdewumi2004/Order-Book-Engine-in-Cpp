#pragma once
#include <map>
#include <deque>
#include <vector>
#include <memory>
#include <algorithm>

#include "Interfaces/IOrder.hpp"
#include "Interfaces/IOrderObserver.hpp"
#include "Events/TradeEvent.hpp"
#include "MatchingEngine.hpp"
#include "OrderEventType.hpp"

class OrderBook {
private:
    // two different map types: ascending for sell, descending for buy
    std::map<double, std::deque<std::shared_ptr<IOrder>>>                   sellOrders;
    std::map<double, std::deque<std::shared_ptr<IOrder>>, std::greater<>>   buyOrders;
    std::vector<std::shared_ptr<IOrderObserver>>                             observers;

    // helper to broadcast a specific event and order to all observers
    void notifyObservers(const std::shared_ptr<IEvent>& event);
public:
    OrderBook() = default;
    ~OrderBook() = default;

    // Observer management
    void addObserver   (const std::shared_ptr<IOrderObserver>& observer);
    void removeObserver(const std::shared_ptr<IOrderObserver>& observer);

    // Core operations
    void addOrder   (const std::shared_ptr<IOrder>& order);
    void removeOrder(const std::shared_ptr<IOrder>& order);

    // Runs matching for a just‐added order against the opposite book
    void matchingEngine(const std::shared_ptr<IOrder>& incomingOrder);
};
