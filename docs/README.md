# Falcon

[简体中文](README.md) | [English](README.en.md)

## 项目介绍

Falcon（隼）是一个面向实时多人游戏的 C++20 服务器项目。

## 功能特性

## 项目结构

```text
Falcon/
├── cmake/             # 公共构建配置
├── docs/              # 项目文档
├── falcon/            # 服务器源码
├── tests/             # 测试
├── tools/             # 辅助工具
├── CMakeLists.txt
└── LICENSE
```

## 环境要求

- 支持 C++20 的编译器
- CMake 3.25 及以上

## 构建

在项目根目录执行：

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel 8
```

## 使用

## 测试

## 文档

## 贡献

## 许可证

[MIT](../LICENSE)
