#ifndef  META_ERASE_HPP
#define  META_ERASE_HPP

#include"reverse.hpp"

namespace meta_base
{
    template<class Template,ptrdiff_t Position>
    class Erase_helper
    {};

    template<template<class...>class Template,ptrdiff_t Idx>
    class Erase_helper<Template<>,Idx>
    {
    public:
        using type=Template<>;
    };

    template<template<class Value,Value...>class Template,class Value,ptrdiff_t Idx>
    class Erase_helper<Template<Value>,Idx>
    {
    public:
        using type=Template<Value>;
    };

    template<template<class Value,Value...>class Template,
            ptrdiff_t Position,
            class Value,Value Head,Value...Rest>
    class Erase_helper<Template<Value,Head,Rest...>,Position>
    {
        static_assert(!Is_out_of_range_v<Template<Value,Head,Rest...>,Position>,"Out of range!");

        static constexpr auto S_pos=index_conversion<Position,Template<Value,Head,Rest...>>;
    public:
        using value_type=Value;
    private:
        template<ptrdiff_t Index,class List,class Out>
        struct Impl
                :type_identity<Out>
        {};

    template<template<class V,V...>class List,class Out,class V,ptrdiff_t Index,V H,V...R>
    struct Impl<Index,List<V,H,R...>, Out>
            :binary_t<Index==S_pos,
                    Impl<Index+1,List<V,R...>,Out>,
                    Impl<Index+1,List<V,R...>,merge_t<Out,List<V,H>>>
    >
    {};
    public:
        using type=typename Impl<0,Template<Value,Head,Rest...>,Template<Value>>::type;
    };

    template<template<class ...>class Template,ptrdiff_t Position,class Head,class...Rest>
    class Erase_helper<Template<Head,Rest...>,Position>
    {
        static_assert(!Is_out_of_range_v<Template<Head,Rest...>,Position>,"Out of range!");

        static constexpr auto S_pos=index_conversion<Position,Template<Head,Rest...>>;
    private:
        template<ptrdiff_t Index,class List,class Out>
        struct Impl
                :type_identity<undefined>
        {};

        template<template<class ...>class List,class Out,ptrdiff_t Index,class H,class...R>
        struct Impl<Index,List<H,R...>, Out>
                :binary_t<Index==S_pos,
                        merge<Out,List<R...>>,
                        Impl<Index+1,List<R...>,merge_t<Out,List<H>>>
        >
        {};
    public:
        using type=typename Impl<0,Template<Head,Rest...>,Template<>>::type;
    };

    template<class Template,ptrdiff_t Position>
    using erase_t=typename Erase_helper<Template,Position>::type;

    template<class Template>
    using pop_front_t=typename Erase_helper<Template,0>::type;

    template<class Template>
    using pop_back_t=reverse_t<pop_front_t<reverse_t<Template>>>;
}

#endif