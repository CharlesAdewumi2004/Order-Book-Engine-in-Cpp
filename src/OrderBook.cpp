#include "OrderBook.hpp"
#include "Events/AddOrderEvent.hpp"
#include "Events/TradeEvent.hpp"
#include "Events/RemoveOrderEvent.hpp"


void OrderBook::addObserver(const std::shared_ptr<IOrderObserver>& observer) {
    observers.push_back(observer);
}

void OrderBook::removeObserver(const std::shared_ptr<IOrderObserver>& observer) {
    observers.erase(
        std::ranges::remove(observers, observer).begin(),
        observers.end()
    );
}

void OrderBook::notifyObservers(const std::shared_ptr<IEvent>& event) {
    for (const auto& obs : observers) {
        obs->onOrderEvent(event);
    }
}



void OrderBook::addOrder(const std::shared_ptr<IOrder>& order) {
    if (order->getOrderType() == OrderType::BUY) {
        buyOrders[order->getPrice()].push_back(order);
    } else {
        sellOrders[order->getPrice()].push_back(order);
    }
    const std::shared_ptr<IEvent> addOrderEvent = std::make_shared<AddOrderEvent>(order);
    notifyObservers(addOrderEvent);

    matchingEngine(order);
}

void OrderBook::removeOrder(const std::shared_ptr<IOrder>& order) {
    if (order->getOrderType() == OrderType::BUY) {
        auto it = buyOrders.find(order->getPrice());
        if (it != buyOrders.end()) {
            auto& dq = it->second;
            dq.erase(std::ranges::remove(dq, order).begin(), dq.end());
            if (dq.empty()) buyOrders.erase(it);
        }
    } else {
        auto it = sellOrders.find(order->getPrice());
        if (it != sellOrders.end()) {
            auto& dq = it->second;
            dq.erase(std::ranges::remove(dq, order).begin(), dq.end());
            if (dq.empty()) sellOrders.erase(it);
        }
    }

    std::shared_ptr<IEvent> const removeOrderEvent = std::make_shared<RemoveOrderEvent>(order);
    notifyObservers(removeOrderEvent);
}



void OrderBook::matchingEngine(const std::shared_ptr<IOrder>& incomingOrder) {
    if (incomingOrder->getOrderType() == OrderType::BUY) {
        auto trades = MatchingEngine::match(incomingOrder, sellOrders);
        for (auto& t : trades) {
            std::shared_ptr<IEvent> event = std::make_shared<TradeEvent>(t);
            notifyObservers(event);
        }
    }
    else {
        auto trades = MatchingEngine::match(incomingOrder, buyOrders);
        for (auto& t : trades) {
            std::shared_ptr<IEvent> event = std::make_shared<TradeEvent>(t);
            notifyObservers(event);
        }
    }
}
std::map<double, std::deque<std::shared_ptr<IOrder>>> OrderBook::getSellOrders() const{return sellOrders;}
std::map<double, std::deque<std::shared_ptr<IOrder>>, std::greater<>> OrderBook::getBuyOrders() const{return buyOrders;}

