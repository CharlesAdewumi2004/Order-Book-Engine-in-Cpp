#pragma once
#include <memory>
#include <chrono>
#include "IOrder.hpp"

class IEvent {
	public:
	virtual ~IEvent() = default;

	virtual int getId() const = 0;
		virtual std::chrono::system_clock::time_point getExecutionTime() const = 0;
};