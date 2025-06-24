#include "MatchingEngine.hpp"

std::vector<Trade> MatchingEngine::match(
    std::shared_ptr<IOrder> incomingOrder,
    std::map<double, std::deque<std::shared_ptr<IOrder>>>& opposingOrders
) {
        std::vector<Trade> trades;
        int remainingQty = incomingOrder->getQuantity();

        // Loop through opposing price levels (sorted by price)
        for (auto it = opposingOrders.begin(); it != opposingOrders.end() && remainingQty > 0; ) {
                double priceLevel = it->first;

                // Check if the price satisfies the matching condition
                bool priceMatch = false;
                if (incomingOrder->getType() == OrderType::BUY) {
                        priceMatch = incomingOrder->getPrice() >= priceLevel;
                } else {
                        priceMatch = incomingOrder->getPrice() <= priceLevel;
                }

                if (!priceMatch) break;

                auto& queue = it->second;

                // Iterate through orders at this price level
                while (!queue.empty() && remainingQty > 0) {
                        std::shared_ptr<IOrder> restingOrder = queue.front();
                        int matchQty = std::min(remainingQty, restingOrder->getQuantity());

                        trades.emplace_back(incomingOrder, restingOrder, matchQty);

                        restingOrder->reduceQuantity(matchQty);
                        remainingQty -= matchQty;

                        if (restingOrder->getQuantity() == 0) {
                                queue.pop_front();  // Fully matched, remove
                        }
                }

                // Remove empty price level
                if (queue.empty()) {
                        it = opposingOrders.erase(it);
                } else {
                        ++it;
                }
        }

        // Optional: update incomingOrder quantity if needed
        incomingOrder->reduceQuantity(incomingOrder->getQuantity() - remainingQty);

        return trades;
}
