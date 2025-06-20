#pragma once

#include <string>
#include <chrono>

enum class OrderType { BUY, SELL };

class IOrder {
public:
    virtual ~IOrder() = default;

    virtual std::string getId() const = 0;
    virtual OrderType getType() const = 0;
    virtual double getPrice() const = 0;
    virtual int getQuantity() const = 0;
    virtual std::chrono::system_clock::time_point getTimestamp() const = 0;

    virtual void reduceQuantity(int amount) = 0;
};
