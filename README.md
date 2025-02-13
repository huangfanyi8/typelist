# type-list

__type-list__ 是一个基于`c++14`及以上标准轻量级元编程库，它基于可变参数模板(`variadic   templates`）以及变量模板(`variable template`)提供包括排序，插入，查找，删除等多种算法.  
支持任意可变参数模板与变量模板。
***

## 构建

 - type-list是纯头文件模板库，使用`CMake`构建。使用时只需要下载并包含`meta.hpp`单一头文件

```
#include"include/meta.hpp"
```

## 示例

 - ### merge(合并)
```c++
/*合并TL的元素*/
#includ"meta.hpp"

using namespace common;

template<class...>struct TL{};
template<class...>struct TL1{};

//error static_assert(std::is_same_v<merge_t<TL<int>,TL1<void>>,TL<int,void>>);
static_assert(std::is_same_v<merge_t<TL<>,TL<>>,TL<>>);
static_assert(std::is_same_v<merge_t<TL<int>,TL<void>>,TL<int,void>>);
```
 - ### unique(去重)

```c++
/*去除TL中相等的元素*/
#include"meta.hpp"

using namespace common;

template<int...v>
using int_sequence = std::integer_sequence<int,v...>;

template<class...>struct TL{};
template<class...>struct TL1{};

static_assert(is_unique_v<TL<int>>);
static_assert(is_unique_v<TL<int,void,int>> == false);
static_assert(is_unique_v<TL<int,void,char>> );
static_assert(is_unique_v<int_sequence<>>);
static_assert(!is_unique_v<int_sequence<0,1,2,3,4,0>>);
static_assert(!is_unique_v<std::index_sequence<1,2,3,4,0,0>>);
```
 - ### get_n(查找)
```c++
/*
获取TL中第n个元素，支持负数下标(从-1开始).
如果下标不合法，默认返回common::undefined或者common::error_constant
*/
#include"meta.hpp"
#include<tuple>

using namespace common;
static_assert(get_n_v<std::integer_sequence<std::ptrdiff_t,-3,-2,-1,0>,-4> ==-3);
static_assert(is_same_v<get_n_t<std::tuple<int,double,int*,int&,char&,int*>,3>,int&>);
static_assert(is_same_v<get_n_t<std::tuple<int,double,int*,int&,char&,int*>,-3>,int&>);
static_assert(is_same_v<get_n_t<std::tuple<int,double,int*,int&,char&,int*>,-2>,char&>); static_assert(is_same_v<get_n_t<std::tuple<int,double,int*,int&,char&,int*>,-100>,undefined>);
```
 - ### reverse(反转)
```c++
#include"meta.hpp"
#include<variant>

static_assert(std::is_same_v<common::int_sequence<1,2,3,4,5>,
                commmon::reverse_t<common::int_sequence<5,4,3,2,1>>>);
  
  static_assert(std::is_same_v<common::reverse_t<std::variant<int,char*>>,std::variant<char*,int>>);
```
 - ### insert(插入)
```c++
//在第n个位置插入元素，支持负数下标

#include"meta.hpp"
using namespace common;

template<class...>class variant{};

  static_assert(is_same_v<
  insert_t<variant<int,double,char>,3,void>,
  variant<int,double,char,void>,
  insert_t<variant<int,double,char>,4,void>
                >);

	static_assert(is_same_v<
	insert_t<index_sequence<>,0,index_sequence<1,2,3,4,5>>,
	index_sequence<1,2,3,4,5>,
	insert_t<index_sequence<1,3,4,5>,1,index_sequence<2>>,
	insert_t<index_sequence<1,2,4,5>,2,index_sequence<3>>,
	insert_t<index_sequence<1,2,3,5>,3,index_sequence<4>>,
	insert_t<index_sequence<1,2,3,4>,4,index_sequence<5>>,
	insert_t<index_sequence<1,2,3,4>,5,index_sequence<5>>
				>);
```
 - ### erase(删除)
```c++
  static_assert(::std::is_same_v<erase_t<index_sequence<1,2,3,4,5,6,7,8>,-6>,index_sequence<1,2,4,5,6,7,8>>);
  static_assert(::std::is_same_v<erase_t<index_sequence<1,2,3,4,5,6,7,8>,-7>,index_sequence<1,3,4,5,6,7,8>>);
  static_assert(::std::is_same_v<erase_t<index_sequence<1,2,3,4,5,6,7,8>,-8>,index_sequence<2,3,4,5,6,7,8>>);
  static_assert(::std::is_same_v<erase_t<index_sequence<1,2,3,4,5,6,7,8>,-88>,index_sequence<1,2,3,4,5,6,7,8>>);
```


