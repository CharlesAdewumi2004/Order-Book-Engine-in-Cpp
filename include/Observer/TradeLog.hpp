
#pragma once

#include "Interfaces/IOrderObserver.hpp"
#include <fstream>
#include <string>

class TradeLog : public IOrderObserver {
public:
	explicit TradeLog(const std::string& fileName);
	~TradeLog();

	void onOrderEvent(std::shared_ptr<IEvent> ev) override;

private:
	std::ofstream out_;
};
