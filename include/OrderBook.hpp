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

    void addObserver   (const std::shared_ptr<IOrderObserver>& observer);
    void removeObserver(const std::shared_ptr<IOrderObserver>& observer);

    void addOrder   (const std::shared_ptr<IOrder>& order);
    void removeOrder(const std::shared_ptr<IOrder>& order);

    void matchingEngine(const std::shared_ptr<IOrder>& incomingOrder);

    std::map<double, std::deque<std::shared_ptr<IOrder>>> getSellOrders() const;
    std::map<double, std::deque<std::shared_ptr<IOrder>>, std::greater<>> getBuyOrders() const;
};
