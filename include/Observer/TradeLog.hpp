
#pragma once

#include "Interfaces/IOrderObserver.hpp"
#include <fstream>
#include <string>
#include "Interfaces/IEvent.hpp"
#include <vector>
#include <memory>

class TradeLog : public IOrderObserver {
public:
	TradeLog(const std::string& fileName);

	// Observer interface
	void onOrderEvent(std::shared_ptr<IEvent> ev) override;

	// New: expose in-memory events
	const std::vector<std::shared_ptr<IEvent>>& getEvents() const { return events_; }

private:
	std::ofstream out_;
	std::vector<std::shared_ptr<IEvent>> events_;
};

