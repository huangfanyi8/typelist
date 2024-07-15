#ifndef META_GET_N_HPP
#define META_GET_N_HPP

#include"erase.hpp"

namespace meta_base
{
    template<class Template,ptrdiff_t Idx,bool=is_normal_v<Template>>
    class get_n_element
    {
        static constexpr auto S_idx=pn_conversion_v<Idx,Template>;
        template<ptrdiff_t c,class,class List>
        struct Impl
                :type_identity<undefined>
        {};

        template<template<class...>class List,class Head,class...Rest>
        struct Impl<0,void,List<Head,Rest...>>
                :type_identity<Head>
        {};

        template<template<class...>class List,class Head,class Second,class...Rest>
        struct Impl<1,void,List<Head,Second,Rest...>>
                :type_identity<Second>
        {};

        template<template<class...>class List,class Head,class Second,class Third,class...Rest>
        struct Impl<2,void,List<Head,Second,Third,Rest...>>
                :type_identity<Third>
        {};

        template<template<class...>class List,class Head,class Second,class Third,class Fourth,class...Rest>
        struct Impl<3,void,List<Head,Second,Third,Fourth,Rest...>>
                :type_identity<Fourth>
        {};

        template<ptrdiff_t c,template<class...>class List,
                class Head,class Second,class Third,class Fourth,class Five,class...Rest>
        struct Impl<c,std::enable_if_t<(c>=4)>,List<Head,Second,Third,Fourth,Five,Rest...>>
                :Impl<c-4,void,List<Five,Rest...>>
        {};
    public:
        using type=typename Impl<S_idx,void,Template>::type;
    };

    template<class Template,ptrdiff_t Idx>
    using get_n_t=typename get_n_element<Template,Idx>::type ;

    template<class Template>
    using front_t=get_n_t<Template,0>;

    template<class Template>
    using back_t=front_t<reverse_t<Template>>;
}

namespace meta_base
{
    template<class Template,ptrdiff_t Position>
    class get_n_element<Template,Position,false>
    {
    private:
        static constexpr auto S_idx=pn_conversion_v<Position,Template>;

        using value_type=traits_value_t<Template>;
    private:
        template<ptrdiff_t c,class,class List>
        struct Impl
                :std::integral_constant<value_type,value_type{} >
        {};

        template<template<class Value,Value...>class List,
                class Value,
                Value Head,Value...Rest>
        struct Impl<0,void,List<Value,Head,Rest...>>
            :constant<Value,Head>
        {};

        template<template<class Value,Value...>class List,
                class Value,
                Value Head,Value Second,Value...Rest>
        struct Impl<1,void,List<Value,Head,Second,Rest...>>
            :constant<Value,Second>
        {};

        template<template<class Value,Value...>class List,
                class Value,
                Value Head,Value Second,Value Third,Value...Rest>
        struct Impl<2,void,List<Value,Head,Second,Third,Rest...>>
            :constant<Value,Third>
        {};

        template<template<class Value,Value...>class List,
                class Value,
                Value Head,Value Second,Value Third,Value Fourth,Value...Rest>
        struct Impl<3,void,List<Value,Head,Second,Third,Fourth,Rest...>>
            :constant<Value,Fourth>
        {};

        template<ptrdiff_t c,template<class Value,Value...>class List,
                class Value,
                Value Head,Value Second,Value Third,Value Fourth,Value Five,Value...Rest>
        struct Impl<c,std::enable_if_t<c>=4>,List<Value,Head,Second,Third,Fourth,Five,Rest...>>
                :Impl<c-4,void,List<Value,Five,Rest...>>
        {};

    public:
        static constexpr auto value=Impl<S_idx,void,Template>::value;
    };

    template<class Template,ptrdiff_t Idx>
    inline constexpr auto get_n_v=get_n_element<Template,Idx>::value;

    template<class Template>
    inline constexpr auto front_v=get_n_v<Template,0>;

    template<class Template>
    inline constexpr auto back_v=front_v<reverse_t<Template>>;
}
#endif //META_GET_N_HPP
