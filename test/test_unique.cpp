
///Test unique algorithm

#include"../unique.hpp"

#include<variant>
//unique
namespace meta_base::test
{
    template<class...>class TypeList{};
    static_assert(is_same_v<unique_t<TypeList<>>,TypeList<>>);
    static_assert(is_same_v<unique_t<TypeList<int>>,TypeList<int>>);
    static_assert(is_same_v<unique_t<TypeList<int>>,TypeList<int>>);
    static_assert(is_same_v<unique_t<TypeList<int,void,void,int,char,float,float,char>>,TypeList<int,void,char,float>>);

    static_assert(is_unique_v<TypeList<>>);
    static_assert(is_unique_v<TypeList<int>>);
    static_assert(is_unique_v<TypeList<int,void,int>> == false);
    static_assert(is_unique_v<TypeList<int,void,char>> );

    static_assert(is_unique_v<bool_sequence<>>);
  
  static_assert(is_unique_v<int_sequence<>>);
  static_assert(!is_unique_v<int_sequence<0,1,2,3,4,0>>);
  static_assert(!is_unique_v<int_sequence<1,2,3,4,0,0>>);
}

int main()
{
    static_assert(meta_base::is_unique_v<std::variant<int,char,double>>);
    static_assert(!meta_base::is_unique_v<std::variant<int,char,double,int>>);
}
