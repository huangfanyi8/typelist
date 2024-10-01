
#ifndef META_SWAP_HPP
#define META_SWAP_HPP

#include"get_n.hpp"
#include"insert.hpp"

///replace  swap
namespace meta_base
{
    template<class Template,template<class,class...>class Predicate,class Replace,class...Parameters>
    class replace_if
            :public type_identity<Template>
    {};

    template<template<class...>class Template,//TypeList
            template<class,class...>class Predicate,class...Parameters,//Predicate and parameters
            template<class...>class Replace,class...Sub,
            class Head,class...Rest
            >
    class replace_if<Template<Head,Rest...>,Predicate,Replace<Sub...>,Parameters...>
    {
        template<class List,class Empty>
        struct Impl
                :type_identity<Empty>
        {};

        template<template<class...>class _Template,class _Head,class _Empty,class..._Rest>
        struct Impl<_Template<_Head,_Rest...>,_Empty>
                :binary_t
                         <
                                 !(Predicate<Head,Parameters...>::value),
                                 Impl<Template<_Rest...>,append_t<_Empty,_Head>>,
                                 Impl<Template<_Rest...>,append_t<_Empty,Sub...>>
                         >
        {};

    public:
        using type=typename Impl<Template<Head,Rest...>,Template<>>::type;
    };
}

namespace meta_base
{
    template<class Template,
                    template<traits_value_t<Template>,traits_value_t<Template>...>class Predicate,
                    class Replace,
                    traits_value_t<Template>...>
    class replace_if_c
            :public type_identity<undefined>
    {};

    template<template<class Value,Value...>class Template,//TypeList
            class Value,Value Head,Value...Rest,
            template<Value,Value...>class Predicate,Value...Parameters,//Predicate and parameters
            template<Value...>class Replace,Value...Sub
    >
    class replace_if_c<Template<Value,Head,Rest...>,Predicate,Replace<Sub...>,Parameters...>
    {
        template<class List,class Empty>
        struct Impl
                :type_identity<Empty>
        {};

        template<template<class V,V...>class _Template,Value _Head,class _Empty,Value..._Rest>
        struct Impl<_Template<Value,_Head,_Rest...>,_Empty>
                :binary_t
                         <
                                 !(Predicate<Head,Parameters...>::value),
                                 Impl<Template<Value,_Rest...>,append_constant_t<_Empty,_Head>>,
                                 Impl<Template<Value,_Rest...>,append_constant_t<_Empty,Sub...>>
                         >
        {};

    public:
        using type=typename Impl<Template<Value,Head,Rest...>,Template<Value>>::type;
    };

    template<class Template,
            template<traits_value_t<Template>,traits_value_t<Template>...>class Predicate,
            class Replace,
            traits_value_t<Template>...P>
    using replace_if_c_t=typename replace_if_c<Template,Predicate,Replace,P...>::type;

    template<int Arg1,int Arg2>
    using equal_c=bool_constant<Arg2==Arg1>;

    static_assert(is_same_v<replace_if_c_t<int_sequence<1,2,3,4,5>,equal_c,int_sequence<0>,2>,undefined>);

}

namespace meta_base
{
    template<class List,size_t X,class ...Replace>
    class replace
    {
        static_assert(X<extent_v<List>);
        template<class _List,class _Empty,size_t Idx>
        struct Impl
                :type_identity<_Empty>
        {};

        template<template<class...>class Template,class _Head,class _Empty,class..._Rest,size_t Idx>
        struct Impl<Template<_Head,_Rest...>,_Empty,Idx>
        :binary_t
                <
                        (Idx!=X),
                        Impl<Template<_Rest...>,append_t<_Empty,_Head>,Idx+1>,
        Impl<Template<_Rest...>,append_t<_Empty,Replace...>,Idx+1>
        >
        {};

    public:
        using type=typename Impl<List,empty_t<List>,0>::type;
    };

    template<class List,size_t X,class ...Replace>
    using replace_t=typename replace<List,X,Replace...>::type;

    template<class List,size_t X,size_t Y>
    class swap
    {
        static_assert(X<extent_v<List>,"Out of range");
        static_assert(Y<extent_v<List>,"Out of range");

        using _X_t=get_n_t<List,static_cast<size_t>(X)>;
        using _Y_t=get_n_t<List,static_cast<size_t>(Y)>;

    public:

        using type=replace_t<replace_t<List,X,_Y_t>,Y,_X_t >;
    };

    template<class List,size_t X,size_t Y>
    using swap_t=typename swap<List,X,Y>::type ;
}

namespace meta_base
{
    template<class Template,size_t Position,traits_value_t<Template>...Replace>
    struct replace_c
    {
        static_assert(Position<extent_v<Template>);

        template<class List,class Empty,size_t Idx>
        struct Impl
                :type_identity<Empty>
        {};

        template<template<class Value,Value...>class C,
                class Empty,
                class Value,Value Head,Value...Rest,
                size_t Idx>
        struct Impl<C<Value,Head,Rest...>,Empty,Idx>
                :binary_t
                         <
                                 (Idx!=Position),
                                 Impl<C<Value,Rest...>,append_constant_t<Empty,Head>,Idx+1>,
                                 Impl<C<Value,Rest...>,append_constant_t<Empty,Replace...>,Idx+1>
                         >
        {};

    public:
        using type=typename Impl<Template,empty_t<Template>,0>::type;
    };

    template<class List,size_t X,traits_value_t<List> ...Replace>
    using replace_c_t=typename replace_c<List,X,Replace...>::type;

    template<template<class Value,Value...>class C,
            class Value,Value...Args,size_t X,size_t Y>
    class swap<C<Value,Args...>,X,Y>
    {
        static_assert(X<extent_v<C<Value,Args...>>,"Out of range");
        static_assert(Y<extent_v<C<Value,Args...>>,"Out of range");

        static constexpr auto _X_v=get_n_v<C<Value,Args...>,static_cast<size_t>(X)>;
        static constexpr auto _Y_v=get_n_v<C<Value,Args...>,static_cast<size_t>(Y)>;

    public:

        using type=replace_c_t<replace_c_t<C<Value,Args...>,X,_Y_v>,Y,_X_v >;
    };
}

#endif //META_SWAP_HPP
