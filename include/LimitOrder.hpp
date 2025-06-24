#pragma once
#include "IOrder.hpp"

class LimitOrder : public IOrder {
private:
    std::string id;
    OrderType type;
    double price;
    int quantity;
    std::chrono::system_clock::time_point timestamp;

public:
    LimitOrder(const std::string& id,
               OrderType type,
               double price,
               int quantity,
               std::chrono::system_clock::time_point timestamp);

    std::string getId() const override;
    OrderType getType() const override;
    double getPrice() const override;
    int getQuantity() const override;
    OrderType getOrderType() const override;
    std::chrono::system_clock::time_point getTimestamp() const override;

    void reduceQuantity(int amount) override;
};
