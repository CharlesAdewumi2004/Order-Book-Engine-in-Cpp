cmake_minimum_required(VERSION 3.16)
project(OrderBookEngine CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# Fetch Catch2
FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.5.2 # or latest stable
)
FetchContent_MakeAvailable(Catch2)

# Include source files
add_library(orderbook STATIC
        src/Order.cpp
        src/OrderBook.cpp
        src/MatchingEngine.cpp
        # add other components here
)

target_include_directories(orderbook PUBLIC include)

# Main executable (optional CLI)
add_executable(orderbook_cli src/main.cpp)
target_link_libraries(orderbook_cli PRIVATE orderbook)

# === UNIT TESTS ===
enable_testing()

add_executable(unit_tests
        test/test_main.cpp
        test/test_order.cpp
        # add other test files
)

target_link_libraries(unit_tests
        PRIVATE orderbook
        PRIVATE Catch2::Catch2WithMain
)

# For automatic test discovery
include(CTest)
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(Catch)
catch_discover_tests(unit_tests)
