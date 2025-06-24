#include "Trade.hpp"

Trade::Trade(std::shared_ptr<IOrder> buy, std::shared_ptr<IOrder> sell, int Qty)
    : id(nextId++), buyOrder(std::move(buy)), sellOrder(std::move(sell)),
      executionTime(std::chrono::system_clock::now()) {matchQty = Qty;}

int Trade::getTradeId() const { return id; }
std::shared_ptr<IOrder> Trade::getBuyOrder() const { return buyOrder; }
std::shared_ptr<IOrder> Trade::getSellOrder() const { return sellOrder; }
std::chrono::system_clock::time_point Trade::getExecutionTime() const { return executionTime; }
