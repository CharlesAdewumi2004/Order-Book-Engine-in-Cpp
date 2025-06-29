#pragma once
#include "Interfaces/IOrder.hpp"
#include "Interfaces/IEvent.hpp"
#include <chrono>
#include <memory>

class AddOrderEvent final : public IEvent{
private:
	OrderEventType eventType;
	inline static int nextId;
	int id;
	std::shared_ptr<IOrder> order;
	std::chrono::system_clock::time_point executionTime;


public:
	explicit AddOrderEvent(std::shared_ptr<IOrder> const& Order);

	OrderEventType getEventType() const override;
	int getId() const override;
	std::shared_ptr<IOrder> getOrder() const override;
	std::chrono::system_clock::time_point getExecutionTime() const override;
};
