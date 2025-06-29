#pragma once
#include <memory>
#include <chrono>
#include "IOrder.hpp"

enum class OrderEventType {
	ADD,
	REMOVE,
	MATCH
};


class IEvent {
	public:
	virtual ~IEvent() = default;

	virtual int getId() const = 0;
	virtual OrderEventType getEventType() const = 0;
	virtual std::chrono::system_clock::time_point getExecutionTime() const = 0;
	virtual std::shared_ptr<IOrder> getOrder() const = 0;
};