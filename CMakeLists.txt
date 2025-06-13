cmake_minimum_required(VERSION 3.16)
project(ModularLimitOrderBook VERSION 1.0 LANGUAGES CXX)

# Use Modern C++ (C++20)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable compile_commands.json for tools like clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Include directories
include_directories(include)

# Source files
file(GLOB_RECURSE SOURCES "src/*.cpp")
add_library(orderbook STATIC ${SOURCES})

# Optional: CLI application
add_executable(orderbook_cli src/main.cpp)
target_link_libraries(orderbook_cli orderbook)

# Enable testing and add test directory
enable_testing()
add_subdirectory(test)
