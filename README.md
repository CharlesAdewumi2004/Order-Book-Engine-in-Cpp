# Modular-Limit-Order-Book-Engine-in-Modern-C-
A C++ project for implementing a order book
---
## Documentation

### Requirements Document
[Requirements Document](docs/requirements.md)

### Design Document
[Design Document](docs/desgin.md)

## CI/CD

This project uses a fully automated CI/CD pipeline powered by GitHub Actions to ensure high code quality, cross-platform compatibility, and continuous test coverage.

### Key Features
Cross-Platform Builds:
Automatically builds and tests the project on both Linux (GCC, Clang) and Windows (MSVC) environments using a matrix strategy. This guarantees portability and platform independence.

Automated Unit Testing with Catch2:
All unit tests written using the Catch2 framework are automatically compiled and executed. The pipeline runs ctest after every push and pull request to verify correctness.

Push & PR Enforcement:
Merges to main are only permitted if all tests pass successfully across all platforms. This ensures no broken code is introduced into the production branch.

Build Type:
Builds are performed in Release mode for performance and correctness checking, with optional Debug builds supported locally.
### Workflow Overview
Trigger:
On every push or pull_request to the main branch.

Steps:

Checkout code

Configure the project using CMake

Build the project with multiple compilers and platforms

Run tests via ctest

Fail the workflow if any test fails or the build breaks

### GitHub Actions Workflow File
You can find the configuration in [WorkFlow](.github/workflows/ci.yml)
