

#ifndef META_TEST_ROTATE_HPP
#define META_TEST_ROTATE_HPP


#include"../rotate.hpp"

namespace meta_base::test
{
    static_assert(is_same_v<rotate_t<variant<int>,11>,variant<int>>);
    static_assert(is_same_v<rotate_t<variant<>,-1>,variant<>>);
    static_assert(is_same_v<rotate_t<variant<int,int*>,11>,variant<int*,int>>);
    static_assert(is_same_v<rotate_t<variant<int,void,float>,-1>,variant<void,float,int>>);
    static_assert(is_same_v<rotate_t<variant<int,void,float>,-2>,variant<float,int,void>>);
    static_assert(is_same_v<rotate_t<variant<int,void,float>,-3>,variant<int,void,float>>);
    static_assert(is_same_v<rotate_t<char_sequence<'1','2','3','4'>,1>,char_sequence<'4','1','2','3'>>);
    static_assert(is_same_v<rotate_t<char_sequence<>,-1>,char_sequence<>>);
    static_assert(is_same_v<rotate_t<int_sequence<>,-1>,int_sequence<>>);
    static_assert(is_same_v<rotate_t<range_sequence<9,0>,-1>,range_sequence<0,9>>);
}
#endif //META_TEST_ROTATE_HPP
