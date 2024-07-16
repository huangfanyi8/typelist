#include"../find.hpp"

template<int  Arg>
using less_0=std::bool_constant<(Arg<0)>;

template<int Arg1,int Arg2>
using equal_arg=meta_base::is_same_c<int,Arg1,Arg2>;

template<int Num>
using is_even_number=meta_base::bool_constant<!(Num%2)>;

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


}