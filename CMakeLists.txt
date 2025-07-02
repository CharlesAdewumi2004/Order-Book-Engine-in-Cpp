cmake_minimum_required(VERSION 3.16)
project(OrderBookEngine LANGUAGES CXX)

# C++ Standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---------------------------------------------------------
# Fetch Catch2
include(FetchContent)

FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.5.2
)
FetchContent_MakeAvailable(Catch2)

# ---------------------------------------------------------
# Include directories
include_directories(include)

# ---------------------------------------------------------
# Core library
add_library(orderbook STATIC
        src/LimitOrder.cpp
        src/OrderFactory.cpp
        src/Trade.cpp
        src/OrderBook.cpp
        src/AddOrderEvent.cpp
        src/RemoveOrderEvent.cpp
        src/TradeLog.cpp
        src/MatchingEngine.cpp
)

target_include_directories(orderbook PUBLIC include)

# ---------------------------------------------------------
# CLI executable
add_executable(orderbook_cli
        src/main.cpp
)

target_link_libraries(orderbook_cli PRIVATE orderbook)

# ---------------------------------------------------------
# Unit tests
enable_testing()

add_executable(unit_tests
        test/test_main.cpp
        test/test_order.cpp
        test/test_order_factory.cpp
        test/test_match_engine.cpp
        test/test_orderbook.cpp
        test/test_tradelog.cpp
)


target_link_libraries(unit_tests
        PRIVATE orderbook
        PRIVATE Catch2::Catch2WithMain
)

# ---------------------------------------------------------
# Catch2 automatic test registration
include(CTest)
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(Catch)
catch_discover_tests(unit_tests)
