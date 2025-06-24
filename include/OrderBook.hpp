#pragma once
#include "OrderFactory.hpp"
#include "IOrderObserver.hpp"
#include <deque>
#include <map>
#include "MatchingEngine.hpp"

class OrderBook{
    private:

        std::map<double, std::deque<std::shared_ptr<IOrder>>> sellOrders;
        std::map<double, std::deque<std::shared_ptr<IOrder>>, std::greater<>> buyOrders;
        std::vector<std::shared_ptr<IOrderObserver>> observers;
    public:
        OrderBook() = default;
        ~OrderBook() = default;
        void addObserver(const std::shared_ptr<IOrderObserver>& observer);
        bool addOrder(const std::shared_ptr<IOrder>& order);
        void notifyObservers();
        bool addOrder(std::shared_ptr<IOrder> order);
        bool removeOrder(std::shared_ptr<IOrder> order);
        void matchingEngine();
};
