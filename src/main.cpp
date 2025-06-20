#include <iostream>
#include <OrderFactory.hpp>
#include <memory>

int add(int a, int b) {
    return a + b;
}

int main() {
    const std::shared_ptr<IOrder> order = OrderFactory::createLimitOrder(10,10, OrderType::BUY);
    std::cout << order->getQuantity() << std::endl;
    std::cout << order->getQuantity() << std::endl;

    return 0;
}
