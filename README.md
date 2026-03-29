# 🌟 Meta - Modern C++ Template Meta Programming Library

> 💡 **TMP** (Template Meta Programming) for Modern C++

[![Standard](https://img.shields.io/badge/c%2B%2B-14/17/20-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B#Standardization)
![Static Badge](https://img.shields.io/badge/modern-TMP-green)
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![Header-only](https://img.shields.io/badge/header--only-yes-success)](.)

---

## 📖 目录

- [介绍](#-介绍)
- [特性](#-特性)
- [快速开始](#-快速开始)
- [使用示例](#-使用示例)
- [算法支持](#-算法支持)
- [构建与集成](#-构建与集成)
- [贡献](#-贡献)
- [许可证](#-许可证)

---

## 📘 介绍

**Meta** 是一个基于 **C++14/C++17** 的轻量级元编程库，专为现代 C++ 开发设计。它不仅提供常规的模板元编程操作（增删改查、排序等），还包括高级的图论和树算法支持。

✨ **核心优势：**
- 🚀 零运行时开销 - 所有计算在编译期完成
- 🎯 类型安全 - 利用 C++ 类型系统保证正确性
- 📦 易于集成 - 纯头文件库，无需编译
- 🔧 灵活扩展 - 支持自定义模板和变量模板

---

## ⚡ 特性

- ✅ **基础操作**: 合并、去重、插入、删除、反转
- ✅ **索引访问**: 支持正负下标，安全边界检查
- ✅ **排序算法**: 插入排序、归并排序、堆排序
- ✅ **数据结构**: 支持 `TL`, `std::tuple`, `std::variant`, `std::integer_sequence` 等
- ✅ **图算法**: DFS、BFS 遍历
- ✅ **树算法**: 霍夫曼树构建与编码
- ✅ **跨标准兼容**: 完美支持 C++14/17/20

---

## 🚀 快速开始

### 安装

Meta 是纯头文件 (header-only) 模板库，使用 `CMake` 构建。使用时只需要下载并包含头文件：

#### C++14
```cpp
#include "include/meta.hpp"
```

#### C++17
```cpp
#include "meta/meta.hpp"
```

### 环境要求

- **编译器**: GCC 5+, Clang 3.4+, MSVC 2015+
- **标准**: C++14 / C++17 / C++20
- **依赖**: 无外部依赖

---

## 💻 使用示例

### 1️⃣ Merge (合并)

合并两个类型列表的元素：

```cpp
#include "meta/meta.hpp"
#include <type_traits>

template<class...> struct TL {};
template<class...> struct TL1 {};

// 合并空列表
static_assert(std::is_same_v<meta::merge_t<TL<>, TL<>>, TL<>>);

// 合并非空列表
static_assert(std::is_same_v<meta::merge_t<TL<int>, TL<void>>, TL<int, void>>);
```

---

### 2️⃣ Unique (去重)

去除类型列表中重复的元素：

```cpp
#include "meta/meta.hpp"
#include <type_traits>

template<int... V>
using int_sequence = std::integer_sequence<int, V...>;

template<class...> struct TL {};

// 检查唯一性
static_assert(meta::is_unique_v<TL<int>>);
static_assert(!meta::is_unique_v<TL<int, void, int>>);
static_assert(meta::is_unique_v<TL<int, void, char>>);

// 支持整数序列
static_assert(meta::is_unique_v<int_sequence<>>);
static_assert(!meta::is_unique_v<int_sequence<0, 1, 2, 3, 4, 0>>);
static_assert(!meta::is_unique_v<std::index_sequence<1, 2, 3, 4, 0, 0>>);
```

---

### 3️⃣ Get N (下标索引)

获取类型列表中第 N 个元素，**支持负数下标**：

```cpp
#include "meta/meta.hpp"
#include <tuple>
#include <type_traits>

using namespace meta;

// 支持负数下标（从末尾计数）
static_assert(get_n_v<std::integer_sequence<std::ptrdiff_t, -3, -2, -1, 0>, -4> == -3);

// 支持 tuple
static_assert(is_same_v<get_n_t<std::tuple<int, double, int*, int&, char&, int*>, 3>, int&>);
static_assert(is_same_v<get_n_t<std::tuple<int, double, int*, int&, char&, int*>, -3>, int&>);
static_assert(is_same_v<get_n_t<std::tuple<int, double, int*, int&, char&, int*>, -2>, char&>);

// 越界返回 undefined
static_assert(is_same_v<get_n_t<std::tuple<int, double, int*, int&, char&, int*>, -100>, undefined>);
```

---

### 4️⃣ Reverse (反转)

反转类型列表的顺序：

```cpp
#include "meta/meta.hpp"
#include <variant>
#include <type_traits>

static_assert(std::is_same_v<
    std::integer_sequence<int, 1, 2, 3, 4, 5>,
    meta::reverse_t<std::integer_sequence<int, 5, 4, 3, 2, 1>>
>);

static_assert(std::is_same_v<
    meta::reverse_t<std::variant<int, char*>>,
    std::variant<char*, int>
>);
```

---

### 5️⃣ Insert (插入)

在指定位置插入元素，**支持负数下标**：

```cpp
#include "meta/meta.hpp"
#include <type_traits>

using namespace meta;

template<class...> class variant {};

// 在末尾插入
static_assert(is_same_v<
    insert_t<variant<int, double, char>, 3, void>,
    variant<int, double, char, void>
>);

// 在中间插入
static_assert(is_same_v<
    insert_t<std::index_sequence<1, 3, 4, 5>, 1, std::index_sequence<2>>,
    std::index_sequence<1, 2, 3, 4, 5>
>);
```

---

### 6️⃣ Erase (删除)

删除指定位置的元素：

```cpp
#include "meta/meta.hpp"
#include <type_traits>

// 删除倒数第 6 个元素
static_assert(std::is_same_v<
    erase_t<std::index_sequence<1, 2, 3, 4, 5, 6, 7, 8>, -6>,
    std::index_sequence<1, 2, 4, 5, 6, 7, 8>
>);

// 删除第一个元素
static_assert(std::is_same_v<
    erase_t<std::index_sequence<1, 2, 3, 4, 5, 6, 7, 8>, -8>,
    std::index_sequence<2, 3, 4, 5, 6, 7, 8>
>);

// 越界不删除
static_assert(std::is_same_v<
    erase_t<std::index_sequence<1, 2, 3, 4, 5, 6, 7, 8>, -88>,
    std::index_sequence<1, 2, 3, 4, 5, 6, 7, 8>
>);
```

---

## 🧮 算法支持

目前已实现的算法：

| 算法名称 | C++17 | C++14 | 说明 |
|:---------|:-----:|:-----:|:-----|
| `insertion-sort` | ✅ | ✅ | 插入排序 |
| `merge-sort` | ✅ | ✅ | 归并排序 |
| `heap-sort` | ✅ | ❌ | 堆排序 (仅 C++17) |
| `huffman-tree` | ✅ | ✅ | 霍夫曼树构建 |
| `DFS` | ✅ | ✅ | 深度优先搜索 |
| `BFS` | ✅ | ✅ | 广度优先搜索 |
| `json` | ❌ | ❌ | 计划中 |

> ✅ = 已实现 &nbsp;&nbsp; ❌ = 未实现

---

## 🛠️ 构建与集成

### CMake 集成

```cmake
# 添加到你的 CMakeLists.txt
add_subdirectory(path/to/meta)
target_link_libraries(your_target PRIVATE meta)
```

### 直接使用

1. 克隆或下载仓库
2. 将 `include/meta.hpp` 或 `meta/meta.hpp` 添加到你的项目
3. 在代码中包含头文件即可

---

## 🤝 贡献

欢迎贡献代码！请遵循以下步骤：

1. Fork 本仓库
2. 创建特性分支 (`git checkout -b feature/AmazingFeature`)
3. 提交更改 (`git commit -m 'Add some AmazingFeature'`)
4. 推送到分支 (`git push origin feature/AmazingFeature`)
5. 开启 Pull Request

---

## 📄 许可证

本项目采用 **MIT 许可证** - 查看 [LICENSE](LICENSE) 文件了解详情。

---

## 🙏 致谢

感谢所有为这个项目做出贡献的开发者！

---

<div align="center">

**Made with ❤️ using Modern C++**

[⬆️ 返回顶部](#-meta---modern-c-template-meta-programming-library)

</div>
