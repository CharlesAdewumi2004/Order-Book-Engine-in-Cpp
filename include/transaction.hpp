#pragma once
#include "IOrder.hpp"
#include <chrono>
#include <memory>

class Transaction {
private:
    inline static int transactionID;
    std::shared_ptr<IOrder> buyOrder;
    std::shared_ptr<IOrder> sellOrder;
    std::chrono::system_clock::time_point executionTime;

public:
    Transaction(std::shared_ptr<IOrder> buy, std::shared_ptr<IOrder> sell);

    int getTransactionId() const;
    std::shared_ptr<IOrder> getBuyOrder() const;
    std::shared_ptr<IOrder> getSellOrder() const;
    std::chrono::system_clock::time_point getExecutionTime() const;
};
