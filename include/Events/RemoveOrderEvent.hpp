#pragma once
#include "Interfaces/IOrder.hpp"
#include "Interfaces/IEvent.hpp"
#include <chrono>
#include <memory>

class RemoveOrderEvent final : public IEvent{
private:
	inline static int nextId;
	int id;
    OrderEventType eventType;
	std::shared_ptr<IOrder> order;
	std::chrono::system_clock::time_point executionTime;


public:
	explicit RemoveOrderEvent(std::shared_ptr<IOrder> buy);

	OrderEventType getEventType() const override;
	int getId() const override;
	std::shared_ptr<IOrder> getOrder() const override;
	std::chrono::system_clock::time_point getExecutionTime() const override;
};