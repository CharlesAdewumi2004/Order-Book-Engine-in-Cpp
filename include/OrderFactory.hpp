#pragma once

#include "LimitOrder.hpp"

class OrderFactory {
    private:
        inline static int id = 0;
    public:
        OrderFactory() = delete;
        static std::shared_ptr<LimitOrder> createLimitOrder(int quantity, int price, OrderType orderType);
};
