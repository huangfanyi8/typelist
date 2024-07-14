#ifndef META_INSERT_HPP
#define META_INSERT_HPP

#include"merge.hpp"

namespace meta_base
{
    template<class Template,size_t Position,class,class...Increment>
    struct Insert_base
    {};

    template<template<class...>class Template,class...Types,class...Increment>
    struct Insert_base<Template<Types...>,0,void,Increment...>
            :type_identity<Template<Increment...,Types...>>
    {};

    template<size_t  Idx,template<class...>class Template,class...Increment>
    struct Insert_base<Template<>,Idx,std::enable_if_t<Idx!=0>,Increment...>
            :type_identity<Template<Increment...>>
    {};

    //insert_t<variant<int,double,char>,3,void>
    template<size_t Idx,template<class...>class Template,class Head,class...Increment,class...Rest>
    struct Insert_base<Template<Head,Rest...>,Idx,std::enable_if_t<(Idx>=1)>,Increment...>
            :merge<Template<Head>,typename Insert_base<Template<Rest...>,Idx-1,void,Increment...>::type>
    {};

    template<class Template,size_t Position,class...Increment>
    using insert_t=typename Insert_base<Template,Position,void,Increment...>::type;

    template<class Template,class...Increment>
    using prepend_t=insert_t<Template,0,Increment...>;

    template<class Template,class...Increment>
    using append_t=insert_t<Template,size_t(-1),Increment...>;
}

namespace meta_base
{
    template<class Template,size_t Position,traits_value_t<Template>...Add>
    struct insert_constant
    {
    public:
        using value_type=traits_value_t<Template>;
    private:
        template<ptrdiff_t Idx,class List,class Empty>
        struct Impl
        {};

        template<ptrdiff_t Idx,template<class V,V...>class List,class V,class Empty,V Head,V...Rest>
        struct Impl<Idx,List<V,Head,Rest...>,Empty>
                :binary_t<Idx!=Position,
                        Impl<Idx+1,List<V,Rest...>,merge_t<Empty,List<V,Head>>>,
                        merge<Empty,List<V,Add...>,List<V,Head,Rest...>>
                >
        {};

        template<ptrdiff_t Idx,template<class V,V...>class List,class V,class Empty>
        struct Impl<Idx,List<V>,Empty>
                :merge<Empty,List<V,Add...>>
        {};

    public:
        using type=
                typename Impl<0,Template,empty_t<Template>>::type;
    };

    template<class Template,size_t Position,traits_value_t<Template>...Add>
    using insert_constant_t=typename insert_constant<Template,Position,Add...>::type;

    template<class Template,traits_value_t<Template> ...Add>
    using append_constant_t=typename insert_constant<Template,extent_v<Template>,Add...>::type;

    template<class Template,traits_value_t<Template>...Add>
    using prepend_constant_t=typename insert_constant<Template,0,Add...>::type;
}

namespace meta_base
{
    template<class>
    struct insert_if
    {

    };
}

#endif //META_INSERT_HPP
