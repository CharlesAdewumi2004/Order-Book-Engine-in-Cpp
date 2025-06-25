#pragma once
#include "Interfaces/IOrder.hpp"
#include <chrono>
#include <memory>

class TradeEvent {
private:
    inline static int nextId;
    int id;
    int matchQty;
    std::shared_ptr<IOrder> buyOrder;
    std::shared_ptr<IOrder> sellOrder;
    std::chrono::system_clock::time_point executionTime;


public:
    TradeEvent(std::shared_ptr<IOrder> buy, std::shared_ptr<IOrder> sell, int matchQty);

    int getTradeId() const;
    std::shared_ptr<IOrder> getBuyOrder() const;
    std::shared_ptr<IOrder> getSellOrder() const;
    std::chrono::system_clock::time_point getExecutionTime() const;
};
