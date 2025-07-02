// MatchingEngine.hpp
#pragma once

#include "Interfaces//IOrder.hpp"
#include "Events//TradeEvent.hpp"
#include <map>
#include <vector>
#include <memory>
#include <algorithm>

class MatchingEngine {
public:
    template<typename MapT>
    static std::vector<TradeEvent> match(const std::shared_ptr<IOrder>& incomingOrder,MapT& opposingOrders) {
        std::vector<TradeEvent> trades;
        int remainingQty = incomingOrder->getQuantity();

        for (auto it = opposingOrders.begin();
             it != opposingOrders.end() && remainingQty > 0; )
        {
            double priceLevel = it->first;
            bool priceMatch = (incomingOrder->getOrderType() == OrderType::BUY)
                ? (incomingOrder->getPrice() >= priceLevel)
                : (incomingOrder->getPrice() <= priceLevel);

            if (!priceMatch)
                break;

            auto& queue = it->second;
            while (!queue.empty() && remainingQty > 0) {
                auto resting = queue.front();
                int matchQty = std::min(remainingQty, resting->getQuantity());

                // Build trade with correct buy/sell order positions:
                if (incomingOrder->getOrderType() == OrderType::BUY) {
                    trades.emplace_back(incomingOrder, resting, matchQty);
                } else {
                    trades.emplace_back(resting, incomingOrder, matchQty);
                }

                // Subtract matched quantity
                resting->reduceQuantity(matchQty);
                remainingQty -= matchQty;

                if (resting->getQuantity() == 0)
                    queue.pop_front();
            }

            // Remove empty price level
            if (queue.empty())
                it = opposingOrders.erase(it);
            else
                ++it;
        }

        return trades;
    }
};
