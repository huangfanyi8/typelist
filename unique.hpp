#ifndef META_UNIQUE_HPP
#define META_UNIQUE_HPP

#include"find.hpp"

namespace meta_base
{
    template<bool Is_unique,class Template,class Out>
    struct Unique_base
            :public pair<Is_unique,Out>
    {};

    template<bool Is_unique,template<class ...>class Template,class Head,class Empty,class...Rest>
    struct Unique_base<Is_unique,Template<Head,Rest...>,Empty>
            :binary_t<is_contain_v<Empty,Head>(),
            Unique_base<false,Template<Rest...>,Empty>,
            Unique_base<true,Template<Rest...>,merge_t<Empty,Template<Head>>>
            >
    {};

    template<class Template>
    class unique
            :public Unique_base<true,Template,empty_t<Template>>
    {};

    template<class Template>
    using unique_t=typename unique<Template>::type;

    template<class Template>
    inline constexpr bool is_unique_v=bool(unique<Template>::S_index);
}

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
}
#endif //META_UNIQUE_HPP
