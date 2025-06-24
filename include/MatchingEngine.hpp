#pragma once
#include "IOrder.hpp"
#include "Transaction.hpp"
#include <map>
#include <deque>
#include <memory>
#include <vector>

class MatchingEngine {
public:
    MatchingEngine() = default;

    std::vector<Transaction> match(
        std::shared_ptr<IOrder> incomingOrder,
        std::map<double, std::deque<std::shared_ptr<IOrder>>>& opposingOrders
    );
};
