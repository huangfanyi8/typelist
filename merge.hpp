#ifndef MERGE_HPP
#define MERGE_HPP

#include"Meta_traits.hpp"

namespace meta_base
{
    template<class...Lists>
    class merge
    {};

    template<class Head,class...Rest>
    class merge<Head,Rest...>
    {
    private:
        template<class...List>
        struct Impl
        {};

        template<template<class...>class Template,class...Types>
        struct Impl<Template<Types...>>
        {
            using type=Template<Types...>;
        };

        template<template<class Value,Value...>class Template,class Value,Value...value>
        struct Impl<Template<Value,value...>>
        {
            using type=Template<Value,value...>;
        };

        template<template<class...>class Template,class...P,class...O>
        struct Impl<Template<P...>,Template<O...>>
        {
            using type=Template<P...,O...>;
        };

        template<template<class Value,Value...>class Template,class Value,Value...P,Value...O>
        struct Impl<Template<Value,P...>,Template<Value,O...>>
        {
            using type=Template<Value,P...,O...>;
        };

        template<class T,class U,class...Other>
        struct Impl<T,U,Other...>
                :Impl<typename Impl<T,U>::type,Other...>
        {};

    public:
        using type=typename Impl<Head,Rest...>::type;
    };

    template<class...Lists>
    using merge_t=typename merge<Lists...>::type;
}
#endif
