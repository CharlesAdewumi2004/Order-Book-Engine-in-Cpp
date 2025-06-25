#pragma once
#include "Interfaces/IOrder.hpp"
#include "Interfaces/IEvent.hpp"
#include <chrono>
#include <memory>

class AddOrderEvent final :  IEvent{
private:
	inline static int nextId;
	int id;
    OrderType type;
	std::shared_ptr<IOrder> order;
	std::chrono::system_clock::time_point executionTime;


public:
	AddOrderEvent(std::shared_ptr<IOrder> buy, std::shared_ptr<IOrder> sell);

	int getId() const override;
    OrderType getType() const;
	std::shared_ptr<IOrder> getOrder() const;
	std::chrono::system_clock::time_point getExecutionTime() const override;
};
