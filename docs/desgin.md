# Design

---

## 1. Core Components

---

### 1.1 `Order`

Represents a single buy or sell limit order submitted by a trader.

**Fields:**
- `id`: unique identifier for the order
- `type`: whether the order is a `"BUY"` or `"SELL"` order
- `price`: the price at which the trader is willing to buy/sell
- `quantity`: the number of units in the order
- `timestamp`: time the order was submitted, used for tie-breaking

**Design Notes:**
- `Order` is a lightweight data structure with no internal logic.
- Orders are compared by price, then by timestamp to enforce price-time priority.

---

### 1.2 `OrderBook`

Manages all active buy and sell orders in the system.

**Responsibilities:**
- Add new orders to the appropriate side of the book
- Cancel existing orders by ID
- Automatically invoke the matching engine after order addition
- Provide access to current best bid/ask
- Display the order book state for user interaction or testing

**Internal Structure:**
- `buyBook`: a `std::map<price, deque<Order>, std::greater<>>` storing buy orders in descending price order
- `sellBook`: a `std::map<price, deque<Order>>` storing sell orders in ascending price order

Orders at each price level are stored in a `deque` to preserve insertion order (for timestamp priority).

---

### 1.3 `MatchingEngine`

Encapsulates the logic for matching incoming orders with existing ones in the order book using price-time priority.

**Responsibilities:**
- Match a new buy order against the lowest available sell orders (ask ≤ bid)
- Match a new sell order against the highest available buy orders (bid ≥ ask)
- Execute full or partial trades as needed
- Reduce or remove matched orders from the book
- Return executed trades for optional logging

**Design Notes:**
- Matching is automatically triggered by `OrderBook::addOrder()`
- Matching logic is isolated for clarity and future extensibility
- Partial fills are supported: an order may result in multiple smaller trades

--- 

### 1.4 `TradeLog`

`TradeLog` serves as a persistent, append-only record of all operations performed on the order book — including order additions, cancellations, and matches.

**Responsibilities:**
- Record every significant event that alters the state of the order book
    - `add` → when a new order is submitted
    - `cancel` → when an order is removed
    - `match` → when a trade is executed
- Append each event to a log file in real time
- Provide access to the in-memory log for inspection or analysis
- Optionally expose a pointer or mechanism to mark replay progress

**Format:**
- Entries are saved in a structured, machine-readable format (e.g., JSON Lines or CSV)
- Example (JSONL):
  ```json
  {"type": "add", "order_id": 101, "side": "BUY", "price": 100, "quantity": 5, "timestamp": ...}
  {"type": "match", "buy_id": 101, "sell_id": 102, "price": 100, "quantity": 3, "timestamp": ...}


---

### ✅ 1.5 `OrderBookSerializer` 

`OrderBookSerializer` (also known as the Snapshot Manager) is responsible for capturing the complete state of the system at specific points in time.

**Responsibilities:**
- Serialize the entire state of the `OrderBook` to a persistent file (e.g., JSON)
- Optionally serialize a pointer or snapshot of the `TradeLog` state
- Save system state on explicit command (e.g., via CLI)
- Support optional automatic snapshotting after every 10 operations
- Restore the system from a previous snapshot + log if needed

**Design Notes:**
- The serializer operates independently of `OrderBook` and `TradeLog` logic
- Snapshot format is distinct from the append-only log — it reflects the in-memory state at a point in time
- Enables recovery via snapshot + replay model: load snapshot, then replay any remaining `TradeLog` entries

**Persistence Strategy:**
- Append-only event log (`TradeLog`) for fine-grained replayability
- Periodic or user-triggered snapshot (`OrderBookSerializer`) for fast startup and recovery

---
## 2. Flow of Operations

### 2.1 Submitting a New Order

1. A user issues a command to place a new buy or sell order.
2. The order is validated and converted into an internal format.
3. It is added to the appropriate side of the order book based on its type (buy/sell).
4. The system immediately checks for available opposing orders that can fulfill it.

---

### 2.2 Matching Orders

5. Matching is performed automatically when a new order is added.
6. The system compares the new order against existing orders on the opposite side of the book.
7. If price and quantity conditions are met:
    - Trades are executed
    - The order may be partially or fully filled
    - The resting order(s) may be removed or updated

---

### 2.3 Recording Events

8. Each state-changing operation — order placement, cancellation, or match — is logged.
9. A structured log entry is appended to a persistent event log for traceability and recovery.

---

### 2.4 Saving a Snapshot

10. The system may save a full snapshot of its state either:
    - Manually (via user command)
    - Automatically, after a fixed number of operations
11. The snapshot contains the current state of the order book and a reference point in the event log.

---

### 2.5 Querying the Book

12. Users may request to view the order book at any time.
13. The system responds with the current buy and sell levels, optionally sorted and summarized.

---

### 2.6 Canceling Orders

14. A user may cancel a previously placed order using its unique identifier.
15. If the order is still active, it is removed from the book and the cancellation is logged.

---

### 2.7 Exiting and Recovery

16. On system shutdown, a final snapshot may be written to disk.
17. Upon restarting:
    - The system restores from the most recent snapshot
    - It replays any remaining operations from the event log to reach the latest state

---

### 2.8 State Diagram

![State Diagram](uml/state%20diagram.png)

---

# 3. Persistence Model

This section describes how the system ensures data durability, recoverability, and fault tolerance using a dual-layered persistence strategy: an append-only event log and periodic snapshots.

### 3.1 Overview

The system persists state in two complementary ways:
- **TradeLog**: A real-time, append-only log of all events (add, match, cancel) for auditability and replay.
- **OrderBookSerializer**: A snapshotting component that saves the complete in-memory state of the system periodically or on demand.

Together, they enable fast recovery after shutdown or crash, without requiring a full replay of the entire history.

---

### 3.2 Trade Log (Event Journal)

- Every state-changing event (order added, matched, canceled) is immediately written to the TradeLog.
- The log is stored in a structured, human-readable format such as JSON Lines (JSONL) or CSV.
- This log allows the system to reconstruct every operation in sequence, providing full replayability.

---

### 3.3 Snapshots (OrderBookSerializer)

- Snapshots are created manually (via CLI) or automatically (after every N operations, e.g., every 10).
- A snapshot captures:
    - The full state of the OrderBook (active buy and sell orders)
    - A reference to the TradeLog's position at the time of snapshot (e.g., op count or file offset)

Snapshots offer a fast-loading restore point, so the system doesn't need to replay the entire event log from the beginning.

---

### 3.4 Recovery Workflow

On restart:
1. The system loads the most recent snapshot of the OrderBook.
2. It then replays all events in the TradeLog that occurred *after* the snapshot point.
3. This ensures full state reconstruction up to the most recent operation.

This model is inspired by event-sourced and journaling systems used in production databases and trading engines.

---

### 3.5 Benefits

| Feature                | TradeLog                | Snapshot                  |
|------------------------|-------------------------|---------------------------|
| **Granularity**        | Fine-grained (1 event)  | Coarse (full state)       |
| **Speed of Load**      | Slow                    | Fast                      |
| **Disk Usage**         | Linear growth           | Constant size (or rotated)|
| **Recovery**           | Replay from snapshot    | Load snapshot + replay log|

By combining both mechanisms, the system balances performance, reliability, and traceability.


## 4. Design Principles Applied

This project is built with long-term maintainability, extensibility, and correctness in mind. The following key software design principles are actively applied:

### 4.1 SOLID Principles

- **Single Responsibility Principle (SRP):**  
  Each class has a focused, well-defined purpose:
    - `OrderBook` manages live orders and coordinates matching
    - `MatchingEngine` performs price-time matching logic
    - `TradeLog` handles event logging
    - `OrderBookSerializer` manages persistence and recovery

- **Open/Closed Principle (OCP):**  
  Matching strategies can be extended or swapped (e.g., pro-rata matching) without modifying `OrderBook` logic.

- **Liskov Substitution Principle (LSP):**  
  Future abstractions (e.g., interfaces for loggers or matching engines) will allow polymorphism without breaking functionality.

- **Interface Segregation Principle (ISP):**  
  Interfaces are kept minimal and focused (e.g., `OrderBookSerializer` does not expose `TradeLog` internals).

- **Dependency Inversion Principle (DIP):**  
  High-level modules (e.g., CLI, controller) depend on abstractions like `OrderBook`, not on file or I/O specifics.

---

### 4.2 C++-Specific Principles

- **RAII (Resource Acquisition Is Initialization):**  
  Memory and file management are wrapped in RAII-safe classes (`std::vector`, `std::ofstream`, `smart pointers`), ensuring deterministic cleanup.

- **Rule of 5 / Rule of 0:**  
  Custom constructors and destructors are only written when needed. Most types follow the Rule of 0 by relying on STL and smart pointers.

- **Const-Correctness:**  
  Read-only methods are marked `const` to enforce immutability and avoid accidental state changes.

- **Separation of Interface and Implementation:**  
  Public interfaces are declared in header files (`.hpp`), while logic resides in implementation files (`.cpp`), improving build hygiene.

---

### 4.3 Architectural Design Patterns

- **Composition over Inheritance:**  
  Components like `OrderBook`, `TradeLog`, and `MatchingEngine` are composed, not subclassed, for better encapsulation.

- **Strategy Pattern (planned):**  
  Matching logic is isolated to enable runtime or compile-time switching of order matching strategies.

- **Snapshot + Event Log Model:**  
  Combines fast snapshot-based recovery with fine-grained TradeLog replayability, inspired by event sourcing architectures in distributed systems.

# 5. Future Work

This project is designed with extensibility in mind. Potential enhancements include:

- Support for market and stop-loss orders
- Customizable matching strategies (e.g., pro-rata, FIFO overrides)
- Multithreaded order book engine
- Real-time data feed integration
- HTTP/REST API for remote interaction
- Interactive TUI or GUI interface for order monitoring
- Persistent storage via a database backend (e.g., SQLite or PostgreSQL)

