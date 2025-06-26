#include "Events/RemoveOrderEvent.hpp"

RemoveOrderEvent::RemoveOrderEvent(std::shared_ptr<IOrder> order)
	: id(nextId++), order(std::move(order)), executionTime(std::chrono::system_clock::now())
{
	type = this->order->getOrderType();
}

int RemoveOrderEvent::getId() const {
	return id;
}

OrderType RemoveOrderEvent::getType() const {
	return type;
}

std::shared_ptr<IOrder> RemoveOrderEvent::getOrder() const {
	return order;
}

std::chrono::system_clock::time_point RemoveOrderEvent::getExecutionTime() const {
	return executionTime;
}
