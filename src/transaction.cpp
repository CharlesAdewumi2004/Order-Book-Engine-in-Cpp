#include "Transaction.hpp"

Transaction::Transaction(std::shared_ptr<IOrder> buy, std::shared_ptr<IOrder> sell)
    : id(nextId++), buyOrder(std::move(buy)), sellOrder(std::move(sell)),
      executionTime(std::chrono::system_clock::now()) {}

int Transaction::getTransactionId() const { return id; }
std::shared_ptr<IOrder> Transaction::getBuyOrder() const { return buyOrder; }
std::shared_ptr<IOrder> Transaction::getSellOrder() const { return sellOrder; }
std::chrono::system_clock::time_point Transaction::getExecutionTime() const { return executionTime; }
