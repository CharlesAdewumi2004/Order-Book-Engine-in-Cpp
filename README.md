# Modular-Limit-Order-Book-Engine-in-Modern-C++

A modular, high-performance, and test-driven limit order book written in **Modern C++20**.  
This project models a realistic trading system with support for order submission, price-time matching, detailed trade event logging, and extensibility through clear interfaces and observer design.

---

## 🚀 Features

- ⚙️ **Modular Architecture** – Clean separation of concerns across components (`OrderBook`, `MatchingEngine`, `TradeLog`, etc.)
- 📈 **Matching Engine** – Implements price-time priority with partial fill support
- 🧠 **Order Event System** – Uses polymorphic `IEvent` interface to capture order additions, removals, and matches
- 🧾 **Trade Logging** – Persist all order events to JSONL format with timestamps for auditability
- 👀 **Observer Pattern** – Attach logging or analytics via a simple `IOrderObserver` interface
- 🧪 **Catch2 Unit Tests** – Every component is fully covered by unit and integration tests
- 💡 **Clean CMake Build** – Cross-platform build system using modern CMake (FetchContent for dependencies)
- 🔁 **CI/CD via GitHub Actions** – Ensures correctness on every push or PR

---

## 📚 Documentation

### ✅ [Requirements Document](docs/requirements.md)
Outlines all functional and non-functional requirements including use case diagrams.

### 🧩 [Design Document](docs/design.md)
Explains all classes and subsystems (`Order`, `OrderBook`, `MatchingEngine`, `TradeLog`, `OrderFactory`, etc.), persistence model, architecture patterns, and trade-offs.

---

## 🔧 Components Overview

| Component      | Role                                                        |
|----------------|-------------------------------------------------------------|
| `Order`        | Represents a single buy/sell limit order                    |
| `LimitOrder`   | Concrete implementation of `IOrder`                         |
| `OrderBook`    | Core engine managing live order state and triggering match  |
| `MatchingEngine` | Stateless engine for order matching based on price-time   |
| `TradeLog`     | Observer that logs all order events as structured JSON      |
| `IEvent` / `AddOrderEvent` / `RemoveOrderEvent` / `TradeEvent` | Event system used for loose coupling and logging |
| `OrderFactory` | Centralized order creation with timestamp and ID injection  |

---

## 🧪 Testing

This project uses **Catch2** for testing.  
You can run the tests with:

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make
ctest
