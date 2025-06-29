#include "Events/RemoveOrderEvent.hpp"

RemoveOrderEvent::RemoveOrderEvent(std::shared_ptr<IOrder> order)
	: id(nextId++), order(std::move(order)), executionTime(std::chrono::system_clock::now())
{
	eventType = OrderEventType::REMOVE;
}

int RemoveOrderEvent::getId() const {
	return id;
}

OrderEventType RemoveOrderEvent::getEventType() const {
  return eventType;
}

std::shared_ptr<IOrder> RemoveOrderEvent::getOrder() const {
	return order;
}

std::chrono::system_clock::time_point RemoveOrderEvent::getExecutionTime() const {
	return executionTime;
}
