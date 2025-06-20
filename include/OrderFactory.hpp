#pragma once

#include "LimitOrder.hpp"

class OrderFactory {
    private:
        constexpr static int id = 0;
        std::string createOrderId();
        std::chrono::system_clock::time_point setOrderTimestamp();
    public:
        OrderFactory() = delete;
        static std::shared_ptr<LimitOrder> createLimitOrder(int quantity, int price, OrderType orderType);
}
