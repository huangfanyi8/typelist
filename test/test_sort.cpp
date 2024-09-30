#include"../sort.hpp"
#include<tuple>
#include<iostream>

template<int a,int b>
using greater=std::bool_constant<(a>b)>;

int main()
{
  std::cout<<meta_base::is_same_v<meta_base::insertion_sort_t<std::tuple<>,
    meta_base::greater>,std::tuple<>>;
  
  std::cout<<meta_base::is_same_v<meta_base::insertion_sort_t<std::tuple<char,int,double>,
    meta_base::greater>,std::tuple<double,int,char>>;
  
  static_assert(meta_base::is_same_v<meta_base::insertion_sort_t<std::tuple<char,int,float>,
    meta_base::greater>,std::tuple<int,float,char>>);
  
  std::cout<<meta_base::is_same_v<meta_base::insertion_sort_t<std::tuple<>,
    meta_base::less>,std::tuple<>>;
  
  std::cout<<meta_base::is_same_v<meta_base::insertion_sort_t<std::tuple<char,int,double>,
    meta_base::less>,std::tuple<double,int,char>>;
  
  static_assert(meta_base::is_same_v<meta_base::insertion_sort_t<std::tuple<char,int,float>,
    meta_base::less>,std::tuple<char,float,int>>);
  
  static_assert(meta_base::is_same_v<meta_base::int_sequence<>,
    meta_base::insertion_sort_c_t<meta_base::int_sequence<>,greater>>);
  
  static_assert(meta_base::is_same_v<meta_base::int_sequence<7,6,5,4,3,2,1,0>,
    meta_base::insertion_sort_c_t<meta_base::int_sequence<1,2,3,4,5,7,6,0>,greater>>);
}
