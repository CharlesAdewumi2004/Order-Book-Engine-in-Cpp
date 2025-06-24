# Modular-Limit-Order-Book-Engine-in-Modern-C++

A high-performance, modular, and extensible limit order book written in Modern C++20.  
This project simulates core functionalities of a financial trading system, including order submission, matching, transaction logging, and snapshot-based recovery — with a strong emphasis on software engineering best practices.

---

## 🔧 Features

- Modular Architecture – Designed around SOLID principles and C++ best practices
-  Matching Engine – Price-time priority matching with support for partial fills
-  Persistent Logging – Append-only trade and order log with JSONL format
-  Snapshot Recovery – Periodic state saving and recovery
-  Observable Pattern – Loggers and serializers attach to `OrderBook` via interfaces
-  Fully Unit Tested – Written with Catch2 and integrated into CI

---

##  Documentation

###  [Requirements Document](docs/requirements.md)
Covers the functional and non-functional goals of the system, including use case diagrams and design constraints.

###  [Design Document](docs/desgin.md)
Detailed explanation of each component, design principles used (e.g., SOLID, RAII), UML class diagrams, persistence strategy, and design trade-offs.

---

##  CI/CD

This project uses a fully automated CI/CD pipeline powered by **GitHub Actions** to ensure continuous verification and maintain production-quality standards.

### Key Features

-  **Cross-Platform Builds**  
  Builds and tests are performed across:
  - Linux: GCC & Clang
  - Windows: MSVC  
  This ensures compiler compliance and platform independence.

-  **Automated Unit Testing with Catch2**  
  Every push and pull request triggers the `unit_tests` executable via `ctest`, enforcing complete test coverage.

-  **Push & PR Enforcement**  
  Code is not merged into the `main` branch unless all tests pass successfully across all platforms and configurations.

-  **Fast Feedback Loop**  
  Tests are executed on GitHub-hosted runners using the latest stable CMake and compilers.

### CI Workflow Overview

- **Trigger**:  
  On every `push` or `pull_request` targeting `main`.

- **Steps**:
  1. Checkout repository
  2. Configure with CMake (multi-platform)
  3. Build with C++20 compiler matrix
  4. Run Catch2 tests using `ctest`
  5. Fail build on test or compiler failure

### 🛠 Workflow Config

The workflow is defined in:  
 [`.github/workflows/ci.yml`](.github/workflows/ci.yml)

---

## Contributing

Contributions are welcome!  
If you’d like to add matching strategies, alternate order types, or new serialization formats — feel free to fork the repo and open a pull request.

---

## 🧪 Testing

To run unit tests locally:

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make
ctest
