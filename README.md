# Cooperative Multitasking API

## Overview

This repository contains a cooperative multitasking API developed in raw C99 and x86_64 assembly. The API provides an async/await-like interface, with additional support for use across multiple concurrent threads. This functionality is a key component of a larger graphics/compute engine project currently under development.

## Features

- **Async/Await-like Interface:** Allows asynchronous programming similar to JavaScript's async/await, but with enhanced capabilities for multi-threading.
- **Multi-threading Support:** Enables concurrent execution across multiple threads, maximizing performance and efficiency.

## Getting Started

### Prerequisites

To build and use this API, you will need:

- A C99-compliant compiler (e.g., GCC, Clang)
- A supported operating system (Linux, MacOS, or Windows with a compatible build environment)

### Installation

1. Clone the repository:
   ```sh
   git clone https://github.com/yourusername/cooperative-multitasking-api.git
   cd cooperative-multitasking-api
2. Build with CMake
