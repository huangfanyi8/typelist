
#ifndef META_TRAITS_HPP
#define META_TRAITS_HPP

#include<type_traits>

using std::size_t ;
using std::ptrdiff_t;

//switch , binary_t
namespace  meta_base
{
    template<class  Type>
    struct type_identity
    {
        using type=Type;
    };

    template<class Type>
    using type_identity_t=typename type_identity<Type>::type;

    template<size_t Idx,class,class...Types>
    struct Switch
    {};

    template<class Head,class...Rest>
    struct Switch<0,void,Head,Rest...>
            :type_identity<Head>
    {};

    template<class Head,class Second,class...Rest>
    struct Switch<1,void,Head,Second,Rest...>
            :type_identity<Second>
    {};

    template<class Head,class Second,class Three,class...Rest>
    struct Switch<2,void,Head,Second,Three,Rest...>
    {
        using type = Three;
    };

    template<size_t Idx,class Head,class Second,class Three,class Four,class...Rest>
    struct Switch<Idx,std::enable_if_t<Idx>=3>,Head,Second,Three,Four,Rest...>
            : Switch<Idx-3,Four,Rest...>
    {};

    template<size_t Idx,class...Types>
    using switch_t=typename Switch<Idx,void,Types...>::type;

    //std::conditional_t
    template<bool C,class T,class U>
    using binary_t=switch_t<C,U,T>;

    template<bool,class Type>
    struct Unary
    {
        using type=Type;
    };

    template<class Type>
    struct Unary<false,Type>
    {};

    template<bool  Cond,class Type>
    using unary_t=typename Unary<Cond,Type>::type ;

    static_assert(std::is_same_v<switch_t<0,int>,int>);
    static_assert(std::is_same_v<switch_t<1,int,double>,double>);
}

//map_t ,  remove_cvref_t
namespace meta_base
{
    template<class Type, template<class >class...Pred>
    struct map
            :type_identity<Type>
    {};

    template<class Type, template<class >class Pred>
    struct map<Type,Pred>
            :Pred<Type>
    {};

    template<class Type,
            template<class >class HeadP,
            template<class >class ...RestP>
    struct map<Type,HeadP,RestP...>
            :map<typename HeadP<Type>::type,RestP...>
    {};

    template<template<class...>class Template,
            template<class >class Pred,
            class...Types>
    struct map<Template<Types...>,Pred>
    :type_identity<Template<typename Pred<Types>::type...>>
{};

template<template<class...>class Template,
        template<class >class HeadP,
        template<class >class ...RestP,class...Types>
struct map<Template<Types...>,HeadP,RestP...>
:map<typename map<Template<Types...>,HeadP>::type,RestP...>
{};

template<class Type, template<class >class...Pred>
using map_t=typename map<Type,Pred...>::type;

template<class Type>
struct remove_cvref
        :type_identity<map_t<Type,std::remove_const,std::remove_reference>>
{};
}

//copy
namespace meta_base
{
    template<class Input, class Output, bool Const = std::is_const_v<Input>, bool Volatile = std::is_volatile_v<Input>>
    struct copy_cv
            :std::conditional_t<Const,
                    std::conditional_t<Volatile, std::add_cv<Output>, std::add_const<Output>>,
                    std::conditional_t<Volatile, std::add_volatile< Output>, type_identity<Output>>
            >
    {};

    template<class Input,class Output,bool R=std::is_rvalue_reference_v<Input>,bool L=std::is_lvalue_reference_v<Input>>
    struct copy_ref
            :binary_t<R,std::add_rvalue_reference<Output>,
            binary_t<L,std::add_lvalue_reference<Output>,Output>
            >
    {};

    template<class Input, class Output>
    using copy_cv_t = typename copy_cv<Input, Output>::type;

    template<class Input, class Output>
    using copy_ref_t = typename copy_ref<Input, Output>::type;
}

//is_same
namespace meta_base
{
    template<class,class...>
    struct is_same
            :std::true_type
    {};

    template<class L,class R>
    struct is_same<L,R>
            :std::is_same<L,R>
    {};

    template<class A,class B,class...Rest>
    struct is_same<A,B,Rest...>
            :std::bool_constant<is_same<A,Rest...>::value&&is_same<B,Rest...>::value>
    {};

    template<class H,class...Rest>
    inline constexpr bool is_same_v=is_same<H,Rest...>::value;
}

//logic
namespace meta_base
{
    //conjunction_v
    template<bool...Requires>
    inline constexpr bool and_v = (Requires&&...&&true);

    //disjunction_v
    template<bool...Requires>
    inline constexpr bool or_v = (Requires||...||false);

    //negation_v
    template<bool F,bool...R>
    inline constexpr bool not_v= (!R&&...&&!F);
}

//Template similar
namespace meta_base
{
    template<class,class>
    inline constexpr bool is_similar_v=false;

    template<template<class Value,Value...>class A,
            template<class Value,Value...>class B,
            class T1,class T2,T1...v1,T2...v2>
    inline constexpr bool is_similar_v<A<T1,v1...>,B<T2,v2...>> =true;

    template<template<class...>class A,
            template<class...>class B,
            class...Types,class...Types1>
    inline constexpr bool is_similar_v<A<Types...>,B<Types1...>> =true;
}

//is_specialization
namespace meta_base
{
    template<template<class...>class Template,class...>
    struct is_specialization
            :std::false_type
    {};

    template<template<class...>class Template, class...Types>
    struct is_specialization<Template,Template<Types...>>
            :std::true_type
    {};

    template<template<class...>class Template, class...Types>
    inline constexpr bool is_specialization_v = is_specialization<Template, Types...>::value;
}
#endif
