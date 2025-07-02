// src/main.cpp
#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <memory>

#include "OrderBook.hpp"
#include "OrderFactory.hpp"
#include "Observer/TradeLog.hpp"

static void printBook(const OrderBook& book) {
    std::cout << "\n=== BUY SIDE ===\n";
    for (const auto& [price, deque] : book.getBuyOrders()) {
        for (auto& o : deque) {
            std::cout << "[ID=" << o->getId()
                      << " Q=" << o->getQuantity()
                      << " P=" << price << "]  ";
        }
        std::cout << "\n";
    }
    std::cout << "\n=== SELL SIDE ===\n";
    for (const auto& [price, deque] : book.getSellOrders()) {
        for (auto& o : deque) {
            std::cout << "[ID=" << o->getId()
                      << " Q=" << o->getQuantity()
                      << " P=" << price << "]  ";
        }
        std::cout << "\n";
    }
    std::cout << std::endl;
}

int main() {
    OrderBook book;
    auto logger = std::make_shared<TradeLog>("trades.jsonl");

    book.addObserver(logger);

    // keep track of every order we create so we can remove by ID
    std::map<std::string, std::shared_ptr<IOrder>> allOrders;

    std::cout << "Welcome to OrderBook CLI!\n";
    std::cout << "Commands:\n"
                 "  add BUY|SELL <qty> <price>\n"
                 "  remove <order_id>\n"
                 "  print\n"
                 "  exit\n\n";

    std::string line;
    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, line)) break;
        std::istringstream iss(line);
        std::string cmd;
        iss >> cmd;
        if (cmd == "exit") {
            break;
        }
        else if (cmd == "print") {
            printBook(book);
        }
        else if (cmd == "add") {
            std::string side;
            int qty;
            double price;
            if (!(iss >> side >> qty >> price)) {
                std::cout << "Usage: add BUY|SELL <qty> <price>\n";
                continue;
            }
            OrderType t = (side == "BUY")
                            ? OrderType::BUY
                            : OrderType::SELL;
            auto order = OrderFactory::createLimitOrder(qty, price, t);
            allOrders[order->getId()] = order;
            book.addOrder(order);
            std::cout << "Added " << side
                      << " order ID=" << order->getId()
                      << " Q=" << qty
                      << " P=" << price << "\n";
        }
        else if (cmd == "remove") {
            std::string id;
            if (!(iss >> id)) {
                std::cout << "Usage: remove <order_id>\n";
                continue;
            }
            auto it = allOrders.find(id);
            if (it == allOrders.end()) {
                std::cout << "No such order: " << id << "\n";
            } else {
                book.removeOrder(it->second);
                std::cout << "Removed order " << id << "\n";
                allOrders.erase(it);
            }
        }
        else {
            std::cout << "Unknown command: " << cmd << "\n";
        }
    }

    std::cout << "Goodbye.\n";
    return 0;
}
