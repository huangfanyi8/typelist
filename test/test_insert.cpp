#include"../insert.hpp"

//insert_constant
namespace meta_base::test
{
    static_assert(is_same_v<int_sequence<1,6>,
            insert_constant_t<int_sequence<1>,9,6>>);

    static_assert(is_same_v<int_sequence<1,6>,
            insert_constant_t<int_sequence<1>,9,6>>);

    static_assert(is_same_v<int_sequence<1,6>,
            insert_constant_t<int_sequence<1>,2,6>>);

    static_assert(is_same_v<int_sequence<1,6>,
            insert_constant_t<int_sequence<1>,3,6>>);

    static_assert(is_same_v<int_sequence<1,3,6,4,5,6>,
            insert_constant_t<int_sequence<1,3,4,5,6>,2,6>>);

    static_assert(is_same_v<int_sequence<1,2,6,3,4,5,6>,
            insert_constant_t<int_sequence<1,2,3,4,5,6>,2,6>>);

    static_assert(is_same_v<int_sequence<1,2,3,6,4,5,6>,
            insert_constant_t<int_sequence<1,2,3,4,5,6>,3,6>>);

    static_assert(is_same_v<range_sequence<>,
            insert_constant_t<range_sequence<>,100>>);

    static_assert(is_same_v<range_sequence<6,7,8,9,1,2,3,4,5>,
            insert_constant_t<range_sequence<1,2,3,4,5>,0,6,7,8,9>>);

    static_assert(is_same_v<int_sequence<1,2,3,4,5,6,6>,
            insert_constant_t<int_sequence<1,2,3,4,5,6>,99,6>>);

    static_assert(is_same_v<range_sequence<0>,
            insert_constant_t<range_sequence<>,0,0>>);
}

int main()
{
    using namespace meta_base;

    static_assert(is_same_v<variant<int,double>,append_t<variant<int>,double>>);
    static_assert(is_same_v<variant<double>,append_t<variant<>,double>>);
    static_assert(is_same_v<variant<double>,insert_t<variant<>,0,double>>);

    static_assert(is_same_v<insert_t<variant<int,double,char>,1,void>,
                  variant<int,void,double,char>
                                     >);
    static_assert(is_same_v<insert_t<variant<int,double,char>,0,void>,
            variant<void,int,double,char>
    >);
    static_assert(is_same_v<insert_t<variant<int,double,char>,2,void>,
                  variant<int,double,void,char>
                                     >);

    static_assert(is_same_v<insert_t<variant<int,double,char>,3,void>,
                  variant<int,double,char,void>
                                     >);

    static_assert(is_same_v<insert_t<variant<int,double,char>,5,void>,
                  variant<int,double,char,void>
                                     >);

    static_assert(is_same_v<insert_t<variant<>,5555,void,int>,
            variant<void,int>
                  >);
    static_assert(is_same_v<prepend_t<variant<int>,double>,variant<double,int>>);

    static_assert(is_same_v<variant<int,double>,
                  insert_t<variant<int>,9,double>>);
}
