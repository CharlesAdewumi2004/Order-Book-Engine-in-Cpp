#pragma once
#include "IOrder.hpp"
#include "Trade.hpp"
#include <map>
#include <deque>
#include <memory>
#include <vector>

class MatchingEngine {
public:
    static std::vector<Trade> match(
        std::shared_ptr<IOrder> incomingOrder,
        std::map<double, std::deque<std::shared_ptr<IOrder>>>& opposingOrders
    );
};
