#include "Observer/TradeLog.hpp"
#include "Interfaces/IEvent.hpp"
#include "Events/AddOrderEvent.hpp"
#include "Events/TradeEvent.hpp"
#include <chrono>
#include <iomanip>

TradeLog::TradeLog(const std::string& fileName)
  : out_(fileName)
{
    if (!out_) {
        throw std::runtime_error("TradeLog: cannot open " + fileName);
    }
}

void TradeLog::onOrderEvent(const std::shared_ptr<IEvent> ev) {
    if (!ev || !out_.is_open()) return;

    // Store in-memory for tests
    events_.push_back(ev);

    // Serialize to file
    using namespace std::chrono;
    const auto tp = ev->getExecutionTime();
    const auto ms = duration_cast<milliseconds>(tp.time_since_epoch()).count();

    switch (ev->getEventType()) {
      case OrderEventType::ADD: {
        auto o = ev->getOrder();
        out_ << "{"
             << "\"type\":\"add\","
             << "\"order_id\":\"" << o->getId() << "\","
             << "\"side\":\""   << (o->getOrderType()==OrderType::BUY?"BUY":"SELL") << "\","
             << "\"price\":"   << o->getPrice() << ","
             << "\"quantity\":"<< o->getQuantity() << ","
             << "\"timestamp\":"<< ms
             << "}\n";
        break;
      }
      case OrderEventType::REMOVE: {
        auto o = ev->getOrder();
        out_ << "{"
             << "\"type\":\"cancel\","
             << "\"order_id\":\"" << o->getId() << "\","
             << "\"side\":\""     << (o->getOrderType()==OrderType::BUY?"BUY":"SELL") << "\","
             << "\"timestamp\":" << ms
             << "}\n";
        break;
      }
      case OrderEventType::MATCH: {
        if (const auto te = std::dynamic_pointer_cast<TradeEvent>(ev)) {
          out_ << "{"
               << "\"type\":\"match\","
               << "\"buy_id\":\""  << te->getBuyOrder()->getId() << "\","
               << "\"sell_id\":\"" << te->getSellOrder()->getId() << "\","
               << "\"price\":"    << te->getPrice() << ","
               << "\"quantity\":" << te->getQty()   << ","
               << "\"timestamp\":"<< ms
               << "}\n";
        }
        break;
      }
    }

    out_.flush();
}
