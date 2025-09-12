# ___Meta___
>`TMP`(Template Meta Programming) for modern c++

[![Standard](https://img.shields.io/badge/c%2B%2B-14/17/20-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B#Standardization)
![Static Badge](https://img.shields.io/badge/modern-TMP-green)

***
## 介绍
 __Meta__ 是一个基于`c++14/c++17`轻量级元编程库，它不仅提供常规的增删改查，排序，还包括图，树算法。
 支持操作任意模板以及变量模。
 

***

## 构建

 * Meta是纯头文件(header-only)模板库，使用`CMake`构建,使用时只需要下载并包含`meta.hpp`单一头文件。
  
 **for C++14**:
```
#include"include/meta.hpp"
```
 **for C++17**:
```
#include"meta/meta.hpp"
```
***
## 示例

 - ### merge(合并)
```c++  
/*合并TL的元素*/  
/*引入头文件*/
template<class...>struct TL{};
template<class...>struct TL1{};

//error static_assert(std::is_same_v<meta::merge_t<TL<int>,TL1<void>>,TL<int,void>>);
static_assert(std::is_same_v<meta::merge_t<TL<>,TL<>>,TL<>>);
static_assert(std::is_same_v<meta::merge_t<TL<int>,TL<void>>,TL<int,void>>);
```

 - ### unique(去重)

```
/*去除TL中相等的元素*/
/*引入头文件*/

template<int...v>
using int_sequence = std::integer_sequence<int,v...>;

template<class...>struct TL{};
template<class...>struct TL1{};

static_assert(meta::is_unique_v<TL<int>>);
static_assert(meta::is_unique_v<TL<int,void,int>> == false);
static_assert(meta::is_unique_v<TL<int,void,char>> );
static_assert(meta::is_unique_v<int_sequence<>>);
static_assert(!meta::is_unique_v<int_sequence<0,1,2,3,4,0>>);
static_assert(!meta::is_unique_v<std::index_sequence<1,2,3,4,0,0>>);
```
 - ### get_n(下标索引)
```c++
/*
获取TL中第n个元素，支持负数下标(从-1开始).
如果下标不合法，默认返回common::undefined或者common::error_constant
*/
#include"meta/meta.hpp"
#include<tuple>

using namespace meta;
static_assert(get_n_v<std::integer_sequence<std::ptrdiff_t,-3,-2,-1,0>,-4> ==-3);
static_assert(is_same_v<get_n_t<std::tuple<int,double,int*,int&,char&,int*>,3>,int&>);
static_assert(is_same_v<get_n_t<std::tuple<int,double,int*,int&,char&,int*>,-3>,int&>);
static_assert(is_same_v<get_n_t<std::tuple<int,double,int*,int&,char&,int*>,-2>,char&>);
static_assert(is_same_v<get_n_t<std::tuple<int,double,int*,int&,char&,int*>,-100>,undefined>);
```
 - ### reverse(反转)
```c++
#include"include/meta.hpp"
#include<variant>

static_assert(std::is_same_v<std::int_sequence<1,2,3,4,5>,meta::reverse_t<std::int_sequence<5,4,3,2,1>>>);
static_assert(std::is_same_v<meta::reverse_t<std::variant<int,char*>>,std::variant<char*,int>>);
```
 - ### insert(插入)
```c++
//在第n个位置插入元素，支持负数下标

#include"include/meta.hpp"
using namespace meta;

template<class...>class variant{};

  static_assert(is_same_v<
  insert_t<variant<int,double,char>,3,void>,
  variant<int,double,char,void>,
  insert_t<variant<int,double,char>,4,void>>);

	static_assert(is_same_v<
	insert_t<std::index_sequence<>,0,std::index_sequence<1,2,3,4,5>>,
	std::index_sequence<1,2,3,4,5>,
	insert_t<std::index_sequence<1,3,4,5>,1,std::index_sequence<2>>,
	insert_t<std::index_sequence<1,2,4,5>,2,std::index_sequence<3>>,
	insert_t<std::index_sequence<1,2,3,5>,3,std::index_sequence<4>>,
	insert_t<std::index_sequence<1,2,3,4>,4,std::index_sequence<5>>,
	insert_t<std::index_sequence<1,2,3,4>,5,std::index_sequence<5>>
				>);
```
 - ### erase(删除)
```c++
  static_assert(::std::is_same_v<erase_t<std::index_sequence<1,2,3,4,5,6,7,8>,-6>,std::index_sequence<1,2,4,5,6,7,8>>);
  static_assert(::std::is_same_v<erase_t<std::index_sequence<1,2,3,4,5,6,7,8>,-7>,std::index_sequence<1,3,4,5,6,7,8>>);
  static_assert(::std::is_same_v<erase_t<std::index_sequence<1,2,3,4,5,6,7,8>,-8>,std::index_sequence<2,3,4,5,6,7,8>>);
  static_assert(::std::is_same_v<erase_t<std::index_sequence<1,2,3,4,5,6,7,8>,-88>,std::index_sequence<1,2,3,4,5,6,7,8>>);
```
***
## 算法
 >目前以实现的算法包括:

  | name                    | C++17 | C++14   |
  |---------------------|------------|------------|
  | `insertion-sort`      |`ok`  |  `ok`          |
  | `merge-sort` | `ok` | `ok`           |
  | `heap-sort`   | `ok` | `no`           |
  | `huffmantree/huffmanoode`        | `ok` | `ok` |
  | `DFS/BFS`          | `ok` | `ok` |
  |	`json`             |   `no`         |  `no`          |


