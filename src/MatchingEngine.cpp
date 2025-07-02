#include "MatchingEngine.hpp"

std::vector<TradeEvent> MatchingEngine::match(
    const std::shared_ptr<IOrder>& incomingOrder,
    std::map<double, std::deque<std::shared_ptr<IOrder>>, std::greater<>>& buyBook,
    std::map<double, std::deque<std::shared_ptr<IOrder>>>& sellBook
) {
    // 1. Perform matching
    std::vector<TradeEvent> trades;
    if (incomingOrder->getType() == OrderType::BUY) {
        trades = matchBuy(incomingOrder, sellBook);
    } else {
        trades = matchSell(incomingOrder, buyBook);
    }

    // 2. Clean up BUY book: remove zero‐qty orders & empty price levels
    for (auto it = buyBook.begin(); it != buyBook.end(); ) {
        auto &queue = it->second;
        queue.erase(
            std::remove_if(queue.begin(), queue.end(),
                [](auto &o){ return o->getQuantity() <= 0; }
            ),
            queue.end()
        );
        if (queue.empty()) {
            it = buyBook.erase(it);
        } else {
            ++it;
        }
    }

    // 3. Clean up SELL book: remove zero‐qty orders & empty price levels
    for (auto it = sellBook.begin(); it != sellBook.end(); ) {
        auto &queue = it->second;
        queue.erase(
            std::remove_if(queue.begin(), queue.end(),
                [](auto &o){ return o->getQuantity() <= 0; }
            ),
            queue.end()
        );
        if (queue.empty()) {
            it = sellBook.erase(it);
        } else {
            ++it;
        }
    }

    return trades;
}


std::vector<TradeEvent> MatchingEngine::matchBuy(
    const std::shared_ptr<IOrder>& incomingOrder,
    std::map<double, std::deque<std::shared_ptr<IOrder>>>& sellBook
) {
    std::vector<TradeEvent> trades;
    int remainingQty = incomingOrder->getQuantity();

    for (auto it = sellBook.begin(); it != sellBook.end() && remainingQty > 0; ) {
        double priceLevel = it->first;
        // Only match if incoming bid ≥ ask price
        if (incomingOrder->getPrice() < priceLevel) {
            break;
        }

        auto& queue = it->second;
        while (!queue.empty() && remainingQty > 0) {
            auto resting = queue.front();
            int matchQty = std::min(remainingQty, resting->getQuantity());

            // Record the trade (buy side first)
            trades.emplace_back(incomingOrder, resting, matchQty);

            // Reduce both sides
            incomingOrder->reduceQuantity(matchQty);
            resting->reduceQuantity(matchQty);
            remainingQty -= matchQty;

            if (resting->getQuantity() == 0) {
                queue.pop_front();
            }
        }

        // Erase empty price level
        if (queue.empty()) {
            it = sellBook.erase(it);
        } else {
            ++it;
        }
    }

    return trades;
}

std::vector<TradeEvent> MatchingEngine::matchSell(
    const std::shared_ptr<IOrder>& incomingOrder,
    std::map<double, std::deque<std::shared_ptr<IOrder>>, std::greater<>>& buyBook
) {
    std::vector<TradeEvent> trades;
    int remainingQty = incomingOrder->getQuantity();

    for (auto it = buyBook.begin(); it != buyBook.end() && remainingQty > 0; ) {
        double priceLevel = it->first;
        // Only match if incoming ask ≤ bid price
        if (incomingOrder->getPrice() > priceLevel) {
            break;
        }

        auto& queue = it->second;
        while (!queue.empty() && remainingQty > 0) {
            auto resting = queue.front();
            int matchQty = std::min(remainingQty, resting->getQuantity());

            // Record the trade (buy side is the resting order)
            trades.emplace_back(resting, incomingOrder, matchQty);

            // Reduce both sides
            incomingOrder->reduceQuantity(matchQty);
            resting->reduceQuantity(matchQty);
            remainingQty -= matchQty;

            if (resting->getQuantity() == 0) {
                queue.pop_front();
            }
        }

        // Erase empty price level
        if (queue.empty()) {
            it = buyBook.erase(it);
        } else {
            ++it;
        }
    }

    return trades;
}
