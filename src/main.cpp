#include <iostream>
#include <LimitOrder.hpp>
#include <memory>

int add(int a, int b) {
    return a + b;
}

int main() {
    std::cout << "2 + 3 = " << add(2, 3) << std::endl;
    std::shared_ptr<IOrder> order = std::make_shared<LimitOrder>("1", OrderType::BUY, 50, 50, std::chrono::system_clock::now());
    std::cout << order->getQuantity() << std::endl;
    return 0;
}
