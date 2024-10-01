#include"../sort.hpp"
#include<tuple>
#include<iostream>

template<auto a,auto b>
using greater=std::bool_constant<(a>b)>;

template<double...v>
using double_sequence=meta_base::constant_sequence<double,v...>;

int main()
{
  std::cout<<meta_base::is_same_v<meta_base::insertion_sort_t<std::tuple<>,
    meta_base::greater>,std::tuple<>>;
  
  std::cout<<meta_base::is_same_v<meta_base::insertion_sort_t<std::tuple<char,int,double>,
    meta_base::greater>,std::tuple<double,int,char>>;
  
  static_assert(meta_base::is_same_v<meta_base::insertion_sort_t<std::tuple<int,float,char>,
    meta_base::greater>,std::tuple<int,float,char>>);
  
  std::cout<<meta_base::is_same_v<meta_base::insertion_sort_t<std::tuple<>,
    meta_base::less>,std::tuple<>>;
  
  std::cout<<meta_base::is_same_v<meta_base::insertion_sort_t<std::tuple<char,int,double>,
    meta_base::less>,std::tuple<double,int,char>>;
  
  static_assert(meta_base::is_same_v<meta_base::insertion_sort_t<std::tuple<char,float,int>,
    meta_base::less>,std::tuple<char,float,int>>);
  
  static_assert(meta_base::is_same_v<meta_base::insertion_sort_t<std::tuple<char>,
    meta_base::less>,std::tuple<char>>);
  
  static_assert(meta_base::is_same_v<double_sequence<>,
    meta_base::experimental::insertion_sort_t<double_sequence<>,greater>
    >);
  
  static_assert(meta_base::is_same_v<
    double_sequence<5.00,5.0,2.0,1.0>,
    meta_base::experimental::insertion_sort_t<double_sequence<1.0,2.00,5.00,5.0>,greater>
  >);
}
