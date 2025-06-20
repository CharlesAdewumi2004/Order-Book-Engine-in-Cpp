#include "OrderFactory.hpp"
#include "memory"

std::shared_ptr<LimitOrder> OrderFactory::createLimitOrder(int quantity, int price, OrderType orderType){
	std::chrono::system_clock::time_point creationTime = std::chrono::system_clock::now();
    std::string orderID = std::to_string(id);
    id++;
	std::shared_ptr<LimitOrder> newOrder = std::make_shared<LimitOrder>(orderID, orderType,price, quantity, creationTime);
    return newOrder;
}

