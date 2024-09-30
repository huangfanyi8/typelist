#include"../find.hpp"
#include<variant>

template<int  Arg>
using less_0=std::bool_constant<(Arg<0)>;

template<int Arg1,int Arg2>
using equal_arg=meta_base::is_same_c<int,Arg1,Arg2>;

template<int Num>
using is_even_number=meta_base::bool_constant<!(Num%2)>;

template<class A>
using equal_to_4=std::bool_constant<sizeof(A)==4>;

template<class A>
using less_equal_4=std::bool_constant<sizeof(A)<=4>;

template<int arg,int value>
using equal_to_number=std::bool_constant<value==arg>;

//打印所有小于或等于num的数字以及下标
template<class Type,Type...num>
void Print(const meta_base::constant_sequence<Type,num...>&sequence )
{

}

int main()
{
    using namespace meta_base;
    static_assert(is_contain_v<bool_sequence<true,true,false>,false>());
    static_assert(!is_contain_v<bool_sequence<>,false>());
    static_assert(!is_contain_v<bool_sequence<true,true>,false>());

    using LongSequence=int_sequence<-1,1,0,-2,3,-4>;

    static_assert(is_same_v<equal_range_if_c_t<LongSequence,is_even_number>,normal_sequence<2,3,5>>);

    static_assert(is_same_v<equal_range_if_c_t<LongSequence,less_0>,normal_sequence<0,3,5>>);
    static_assert(is_same_v<equal_range_if_c_t<int_sequence<>,less_0>,normal_sequence<>>);
    static_assert(is_same_v<equal_range_if_c_t<LongSequence ,equal_arg,3>,
            normal_sequence<4>,
            equal_range_c_t<LongSequence,3>>);
  
  static_assert(is_same_v<equal_range_t<std::variant<>,double>,
    equal_range_t<std::variant<int>,double>,
    normal_sequence<>>);
  
  static_assert(is_same_v<equal_range_t<std::variant<int*,double,char,double>,double>,
    normal_sequence<1,3>>);
  
  static_assert(count_v<std::variant<int,double,char>,int&&> ==0);
  static_assert(count_v<std::variant<>,int&&> ==0);
  static_assert(count_v<std::variant<int,double,char,int>,int> ==2);
  static_assert(count_v<std::variant<int,double,char,int>,front_t<std::variant<int,double,char,int>>> ==2);
  static_assert(count_if_v<std::variant<int,float,double,char,int>,equal_to_4> ==3);
  
  static_assert(is_same_v<filter_if_t<std::variant<int,float,double,char,int>,less_equal_4>,
    std::variant<double>>);
  
  static_assert(is_same_v<find_if_t<std::variant<int,float,double,char,int>,less_equal_4>,
    std::variant<int,float,char,int>>);
  
  static_assert(is_same_v<filter_if_t<std::variant<>,less_equal_4>,
    std::variant<>>);
  
  static_assert(is_same_v<find_if_t<std::variant<>,less_equal_4>,
    std::variant<>>);
  
  static_assert(is_same_v<find_if_c_t<int_sequence<>,less_0>,
    int_sequence<>>);
  
  static_assert(is_same_v<find_if_c_t<int_sequence<7,8,9,2,3,4,6,9>,equal_to_number,6>,
    int_sequence<6>>);
}