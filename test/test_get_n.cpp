/*
 * 获取普通模板或变量模板的第n个参数（下标从0开始）
 * 支持负数下标
 * 对于普通模板，如果范围超标，则返回undefined
 * 对于变量模板，返回0
 *
 * 预计实现c++20 的CPO模式解决下标越界问题
 * */

#include"../get_n.hpp"
#include<variant>
#include<tuple>

using std::variant;
using std::tuple;

int main()
{
    using namespace meta_base;
    static_assert(is_same_v<get_n_t<variant<>,0>,undefined>);
    static_assert(is_same_v<get_n_t<variant<int>,-100>,undefined>);
    static_assert(is_same_v<front_t<variant<>>,undefined>);
    static_assert(is_same_v<get_n_t<variant<int>,22>,undefined>);
    static_assert(is_same_v<get_n_t<variant<int>,0>,int>);
    static_assert(is_same_v<front_t<variant<int>>,int>);
    static_assert(is_same_v<get_n_t<variant<int,double,char>,1>,double>);
    static_assert(is_same_v<get_n_t<variant<int,double,char>,2>,char>);
    static_assert(is_same_v<front_t<variant<int,double,char>>,int>);
    static_assert(is_same_v<back_t<variant<int,double,char>>,char>);
    static_assert(is_same_v<back_t<variant<char>>,char>);
    static_assert(is_same_v<back_t<variant<>>,undefined>);
    
  static_assert(is_same_v<get_n_t<tuple<int,double,int*,int&,char&,int*>,3>,int&>);
  static_assert(is_same_v<get_n_t<tuple<int,double,int*,int&,char&,int*>,-3>,int&>);
  static_assert(is_same_v<get_n_t<tuple<int,double,int*,int&,char&,int*>,-2>,char&>);
  static_assert(is_same_v<get_n_t<tuple<int,double,int*,int&,char&,int*>,-1>,std::add_pointer_t<int>>);
  static_assert(is_same_v<get_n_t<tuple<int,double,int*,int&,char&,int*>,-4>,int*>);
  static_assert(is_same_v<get_n_t<tuple<int,double,int*,int&,char&,int*>,-100>,undefined>);
  
    static_assert(get_n_v<char_sequence<'1','2','3'>,0> =='1');
    static_assert(get_n_v<char_sequence<'1','2','3'>,1> =='2');
    static_assert(get_n_v<char_sequence<'1','2','3'>,4> ==0);
    static_assert(get_n_v<char_sequence<'1','2','3'>,3> ==0);
    static_assert(get_n_v<char_sequence<>,-11> ==0);
    static_assert(get_n_v<char_sequence<'1','2','3'>,2> =='3');
    static_assert(get_n_v<char_sequence<'1','2','3'>,-2> =='2');

    static_assert(get_n_v<bool_sequence<true,false,true>,2> == true);
    static_assert(get_n_v<bool_sequence<true,false,true>,-2> == false);
    static_assert(get_n_v<range_sequence<-3,-2,-1,0,1,2,3,4>,2> == -1);
    static_assert(get_n_v<range_sequence<-3,-2,-1,0>,-4> ==-3);
    static_assert(get_n_v<range_sequence<-3,-2,-1,0>,-1> ==0);
}
