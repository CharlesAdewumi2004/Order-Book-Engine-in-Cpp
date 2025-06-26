// IOrderObserver.hpp
#pragma once

#include <memory>
#include "Interfaces/IEvent.hpp"

class IOrderObserver {
public:
    virtual void onOrderEvent (std::shared_ptr<IEvent>) = 0;

    virtual ~IOrderObserver() = default;
};
