#include "Interfaces//IOrder.hpp"
#include "Events//TradeEvent.hpp"
#include <map>
#include <vector>
#include <memory>
#include <deque>
#include <algorithm>

class MatchingEngine{
  	private:
	std::vector<TradeEvent> matchBuy(
	const std::shared_ptr<IOrder>& incomingOrder,
	std::map<double, std::deque<std::shared_ptr<IOrder>>>& sellBook
	);
	std::vector<TradeEvent> matchSell(
	const std::shared_ptr<IOrder>& incomingOrder,
	std::map<double, std::deque<std::shared_ptr<IOrder>>, std::greater<>>& buyBook
	);
    public:
	std::vector<TradeEvent> match(
	const std::shared_ptr<IOrder>& incomingOrder,
	std::map<double, std::deque<std::shared_ptr<IOrder>>, std::greater<>>& buyBook,
	std::map<double, std::deque<std::shared_ptr<IOrder>>>& sellBook
	);
};