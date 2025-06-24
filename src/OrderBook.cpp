#include "OrderBook.hpp"

// ——— Observer Hooks ——————————————————————————————————————————————

void OrderBook::addObserver(const std::shared_ptr<IOrderObserver>& observer) {
    observers.push_back(observer);
}

void OrderBook::removeObserver(const std::shared_ptr<IOrderObserver>& observer) {
    observers.erase(
        std::remove(observers.begin(), observers.end(), observer),
        observers.end()
    );
}

void OrderBook::notifyObservers(OrderEventType event, const std::shared_ptr<IOrder>& order) {
    for (auto& obs : observers) {
        obs->onOrderEvent(event, order);
    }
}

// ——— Adding and Removing Orders ——————————————————————————————————

void OrderBook::addOrder(const std::shared_ptr<IOrder>& order) {
    // 1) Insert into the correct side
    if (order->getOrderType() == OrderType::BUY) {
        buyOrders[order->getPrice()].push_back(order);
    } else {
        sellOrders[order->getPrice()].push_back(order);
    }

    notifyObservers(OrderEventType::ADD, order);

    // 2) Immediately try to match
    matchingEngine(order);
}

void OrderBook::removeOrder(const std::shared_ptr<IOrder>& order) {
    // Branch on side because map types differ
    if (order->getOrderType() == OrderType::BUY) {
        auto it = buyOrders.find(order->getPrice());
        if (it != buyOrders.end()) {
            auto& dq = it->second;
            dq.erase(std::remove(dq.begin(), dq.end(), order), dq.end());
            if (dq.empty()) buyOrders.erase(it);
        }
    } else {
        auto it = sellOrders.find(order->getPrice());
        if (it != sellOrders.end()) {
            auto& dq = it->second;
            dq.erase(std::remove(dq.begin(), dq.end(), order), dq.end());
            if (dq.empty()) sellOrders.erase(it);
        }
    }

    notifyObservers(OrderEventType::REMOVE, order);
}



void OrderBook::matchingEngine(const std::shared_ptr<IOrder>& incomingOrder) {
    if (incomingOrder->getOrderType() == OrderType::BUY) {
        auto trades = MatchingEngine::match(incomingOrder, sellOrders);
        for (auto& t : trades) {
            notifyObservers(OrderEventType::MATCH, t.getBuyOrder());
            notifyObservers(OrderEventType::MATCH, t.getSellOrder());
        }
    }
    else {
        auto trades = MatchingEngine::match(incomingOrder, buyOrders);
        for (auto& t : trades) {
            notifyObservers(OrderEventType::MATCH, t.getBuyOrder());
            notifyObservers(OrderEventType::MATCH, t.getSellOrder());
        }
    }
}

