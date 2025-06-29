#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>

#include <fstream>
#include <cstdio>
#include <memory>
#include <filesystem>
#include <string>

#include "Observer/TradeLog.hpp"
#include "Events/AddOrderEvent.hpp"
#include "Events/RemoveOrderEvent.hpp"
#include "Events/TradeEvent.hpp"
#include "OrderFactory.hpp"

namespace fs = std::filesystem;

// Helper to read exactly one nonempty line and normalize line endings
static std::string loadOneLine(const std::string& fname) {
    std::ifstream in(fname);
    REQUIRE(in.good());
    std::string line;
    while (std::getline(in, line)) {
        line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());  // normalize line endings
        if (!line.empty()) return line;
    }
    FAIL("No non-empty line in " << fname);
    return "";
}

//------------------[ ADD ]-------------------
TEST_CASE("TradeLog logs ADD events as JSON", "[TradeLog][ADD]") {
    const std::string fname = "tradelog_add.jsonl";
    std::remove(fname.c_str());

    TradeLog log{fname};
    auto o = OrderFactory::createLimitOrder(2, 10, OrderType::BUY);
    auto ev = std::make_shared<AddOrderEvent>(o);
    log.onOrderEvent(ev);

    std::string line = loadOneLine(fname);
    CHECK(line.starts_with("{\"type\":\"add\""));
    CHECK(line.find("\"order_id\":\"" + o->getId() + "\"") != std::string::npos);
    CHECK(line.find("\"side\":\"BUY\"") != std::string::npos);
    CHECK(line.find("\"price\":10")     != std::string::npos);
    CHECK(line.find("\"quantity\":2")   != std::string::npos);
    CHECK(line.find("\"timestamp\":")   != std::string::npos);

    std::remove(fname.c_str());
}

//------------------[ REMOVE ]-------------------
TEST_CASE("TradeLog logs REMOVE events as JSON", "[TradeLog][REMOVE]") {
    const std::string fname = "tradelog_remove.jsonl";
    std::remove(fname.c_str());

    TradeLog log{fname};
    auto o = OrderFactory::createLimitOrder(5, 99, OrderType::SELL);
    auto ev = std::make_shared<RemoveOrderEvent>(o);
    log.onOrderEvent(ev);

    std::string line = loadOneLine(fname);
    CHECK(line.starts_with("{\"type\":\"cancel\""));
    CHECK(line.find("\"order_id\":\"" + o->getId() + "\"") != std::string::npos);
    CHECK(line.find("\"side\":\"SELL\"") != std::string::npos);
    CHECK(line.find("\"timestamp\":")   != std::string::npos);

    std::remove(fname.c_str());
}

//------------------[ MATCH ]-------------------
TEST_CASE("TradeLog logs MATCH events as JSON", "[TradeLog][MATCH]") {
    const std::string fname = "tradelog_match.jsonl";
    std::remove(fname.c_str());

    TradeLog log{fname};
    auto sell = OrderFactory::createLimitOrder(3, 20, OrderType::SELL);
    auto buy  = OrderFactory::createLimitOrder(3, 20, OrderType::BUY);
    auto ev = std::make_shared<TradeEvent>(buy, sell, 3);
    log.onOrderEvent(ev);

    std::string line = loadOneLine(fname);
    CHECK(line.starts_with("{\"type\":\"match\""));
    CHECK(line.find("\"buy_id\":\""  + buy ->getId() + "\"")  != std::string::npos);
    CHECK(line.find("\"sell_id\":\"" + sell->getId() + "\"") != std::string::npos);
    CHECK(line.find("\"price\":20")   != std::string::npos);
    CHECK(line.find("\"quantity\":3") != std::string::npos);
    CHECK(line.find("\"timestamp\":") != std::string::npos);

    std::remove(fname.c_str());
}

//------------------[ CONSTRUCTOR ERROR ]-------------------
TEST_CASE("TradeLog constructor throws on bad path", "[TradeLog][ERROR]") {
    std::string bad = "/this_path_does_not_exist/log.txt";
    REQUIRE_THROWS_AS(TradeLog{bad}, std::runtime_error);
}
