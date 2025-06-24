#pragma once
#include "OrderFactory.hpp"
#include "IOrderObserver.hpp"
#include <deque>
#include <map>

class OrderBook{
    private:
        //matchingengine class
        std::vector<IOrderObserver> observers;
        std::map<double, std::deque<IOrder>> sellOrders;
        std::map<double, std::deque<IOrder>, std::greater<>> buyOrders;
    public:
        OrderBook() = default;
        ~OrderBook() = default;
        void addObserver(IOrderObserver);
        void removeObserver(IOrderObserver);
        void notifyObservers();
        bool addOrder(IOrder);
        bool removeOrder(IOrder);
        void matchingEngine();
}
