# Falcon

[简体中文](README.md) | [English](README.en.md)

## Introduction

Falcon is a C++20 server project for real-time multiplayer games.

## Features

## Project Structure

```text
Falcon/
├── cmake/             # Shared build settings
├── docs/              # Documentation
├── falcon/            # Server source code
├── tests/             # Tests
├── tools/             # Utilities
├── CMakeLists.txt
└── LICENSE
```

## Requirements

- A compiler with C++20 support
- CMake 3.25 or later

## Build

Run from the project root:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel 8
```

## Usage

## Testing

## Documentation

## Contributing

## License

[MIT](../LICENSE)
