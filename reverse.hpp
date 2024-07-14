
#ifndef REVERSE_HPP
#define REVERSE_HPP

#include"merge.hpp"

///reverse
namespace meta_base
{
    template<class List,bool=is_normal_v<List>>
    class reverse
    {
    private:
        template<class T,class E>
        struct Impl
                :type_identity<E>
        {};

        template<template<class...> class Template,class E,class Head,class...Rest>
        struct Impl<Template<Head,Rest...>,E>
                :Impl<Template<Rest...>,merge_t<Template<Head>,E>>
        {};
    public:
        using type=typename Impl<List,empty_t<List>>::type;
    };

    template<class List>
    class reverse<List,false>
    {
    private:
        template<class,class Empty>
        struct Impl
                :type_identity<Empty>
        {};

        template<template<class Value,Value...> class Template,class Empty,class Value,Value Head,Value...Rest>
        struct Impl<Template<Value,Head,Rest...>,Empty>
                :Impl<Template<Value,Rest...>,merge_t<Template<Value,Head>,Empty>>
        {};
    public:
        using type=typename Impl<List,empty_t<List>>::type;
    };

    template<class Template>
    using reverse_t=typename reverse<Template>::type;

    template<class...>class T{};

}
#endif
