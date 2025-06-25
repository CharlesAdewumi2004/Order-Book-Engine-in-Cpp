#include "Events/TradeEvent.hpp"

TradeEvent::TradeEvent(std::shared_ptr<IOrder> buy, std::shared_ptr<IOrder> sell, int Qty)
    : id(nextId++), buyOrder(std::move(buy)), sellOrder(std::move(sell)),
      executionTime(std::chrono::system_clock::now()) {matchQty = Qty;}

int TradeEvent::getId() const { return id; }
std::shared_ptr<IOrder> TradeEvent::getBuyOrder() const { return buyOrder; }
std::shared_ptr<IOrder> TradeEvent::getSellOrder() const { return sellOrder; }
std::chrono::system_clock::time_point TradeEvent::getExecutionTime() const { return executionTime; }
