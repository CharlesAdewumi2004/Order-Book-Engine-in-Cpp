#pragma once
#include "Interfaces/IOrder.hpp"
#include "Interfaces/IEvent.hpp"
#include <chrono>
#include <memory>

class TradeEvent final : public IEvent{
private:
    inline static int nextId;
    int id;
    int matchQty;
    OrderEventType eventType;
    std::shared_ptr<IOrder> buyOrder;
    std::shared_ptr<IOrder> sellOrder;
    std::chrono::system_clock::time_point executionTime;


public:
    explicit  TradeEvent(std::shared_ptr<IOrder> buy, std::shared_ptr<IOrder> sell, int matchQty);

    OrderEventType getEventType() const override;
    int getId() const override;
    int getQty() const;
    double getPrice() const;
    std::shared_ptr<IOrder> getBuyOrder() const;
    std::shared_ptr<IOrder> getSellOrder() const;
    std::chrono::system_clock::time_point getExecutionTime() const override;
    std::shared_ptr<IOrder> getOrder() const override;
};
