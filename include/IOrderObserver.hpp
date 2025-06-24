// IOrderObserver.hpp
#pragma once

#include <memory>
#include "IOrder.hpp"
#include "OrderEventType.hpp"

class IOrderObserver {
public:
    virtual void onOrderEvent(OrderEventType event,
                              const std::shared_ptr<IOrder>& order) = 0;

    virtual ~IOrderObserver() = default;
};
