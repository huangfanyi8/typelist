#ifndef META_UNIQUE_HPP
#define META_UNIQUE_HPP

#include"find.hpp"

namespace meta_base
{
    template<bool Is_unique,class Template,class Out>
    struct Unique_base
    {
        using type=Out;
        static constexpr bool value=Is_unique;
    };

    template<bool Is_unique,template<class ...>class Template,class Head,class Empty,class...Rest>
    struct Unique_base<Is_unique,Template<Head,Rest...>,Empty>
            :binary_t<is_contain_v<Empty,Head>(),
            Unique_base<false,Template<Rest...>,Empty>,
            Unique_base<true,Template<Rest...>,merge_t<Empty,Template<Head>>>
            >
    {};

    template<bool Is_unique,template<class Value,Value...>class Template,
            class Empty,
            class Value_type,Value_type Head,Value_type...Rest>
    struct Unique_base<Is_unique,Template<Value_type,Head,Rest...>,Empty>
            :binary_t<is_contain_v<Empty,Head>(),
                    Unique_base<false,Template<Value_type,Rest...>,Empty>,
                    Unique_base<true,Template<Value_type,Rest...>,merge_t<Empty,Template<Value_type,Head>>>
            >
    {};

    template<class Template>
    class unique
            :public Unique_base<true,Template,empty_t<Template>>
    {};

    template<class Template>
    using unique_t=typename unique<Template>::type;

    template<class Template>
    inline constexpr bool is_unique_v=bool(unique<Template>::value);
}


#endif //META_UNIQUE_HPP
