#include "LimitOrder.hpp"

LimitOrder::LimitOrder(const std::string& id,
                       OrderType type,
                       double price,
                       int quantity,
                       std::chrono::system_clock::time_point timestamp)
    : id(id), type(type), price(price), quantity(quantity), timestamp(timestamp) {}

std::string LimitOrder::getId() const { return id; }
OrderType LimitOrder::getType() const { return type; }
double LimitOrder::getPrice() const { return price; }
int LimitOrder::getQuantity() const { return quantity; }
std::chrono::system_clock::time_point LimitOrder::getTimestamp() const { return timestamp; }

void LimitOrder::reduceQuantity(int amount) {
    if (amount > 0 && amount <= quantity) {
        quantity -= amount;
    }
}
