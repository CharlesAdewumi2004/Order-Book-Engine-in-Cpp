#include "Events/AddOrderEvent.hpp"

AddOrderEvent::AddOrderEvent(std::shared_ptr<IOrder> const& order)
	: id(nextId++), order(std::move(order)), executionTime(std::chrono::system_clock::now())
{
	eventType = OrderEventType::ADD;
}

int AddOrderEvent::getId() const {
	return id;
}

OrderEventType AddOrderEvent::getEventType() const {
	return eventType;
}

std::shared_ptr<IOrder> AddOrderEvent::getOrder() const {
	return order;
}

std::chrono::system_clock::time_point AddOrderEvent::getExecutionTime() const {
	return executionTime;
}
