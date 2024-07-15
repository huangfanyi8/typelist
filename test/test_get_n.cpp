#include"../get_n.hpp"

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
