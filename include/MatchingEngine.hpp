// MatchingEngine.hpp
#pragma once

#include "IOrder.hpp"
#include "Trade.hpp"
#include <map>
#include <deque>
#include <vector>
#include <memory>
#include <algorithm>

/// A stateless matching engine that works with *any* map<double, deque<…>>.
class MatchingEngine {
public:
    template<typename MapT>
    static std::vector<Trade> match(
        const std::shared_ptr<IOrder>& incomingOrder,
        MapT& opposingOrders
    ) {
        std::vector<Trade> trades;
        int remainingQty = incomingOrder->getQuantity();

        for (auto it = opposingOrders.begin();
             it != opposingOrders.end() && remainingQty > 0; )
        {
            double priceLevel = it->first;
            bool priceMatch = (incomingOrder->getOrderType() == OrderType::BUY)
                ? (incomingOrder->getPrice() >= priceLevel)
                : (incomingOrder->getPrice() <= priceLevel);

            if (!priceMatch) break;

            auto& queue = it->second;
            while (!queue.empty() && remainingQty > 0) {
                auto resting = queue.front();
                int matchQty = std::min(remainingQty, resting->getQuantity());

                // Record trade with correct buy/sell order positions
                if (incomingOrder->getOrderType() == OrderType::BUY) {
                    trades.emplace_back(incomingOrder, resting, matchQty);
                } else {
                    trades.emplace_back(resting, incomingOrder, matchQty);
                }

                resting->reduceQuantity(matchQty);
                incomingOrder->reduceQuantity(matchQty);
                remainingQty -= matchQty;

                if (resting->getQuantity() == 0)
                    queue.pop_front();
            }

            if (queue.empty())
                it = opposingOrders.erase(it);
            else
                ++it;
        }

        return trades;
    }
};
