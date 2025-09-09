/**
 meta库是一款基于c++17轻量级的元编程库，
 在编译期对于元序列（传统的TypeList以及c++14开始引入的Variable Template）的各种算法性操作，包含分割，合并，切片，反转，排序，增删改查等等
 与旧版本相比，使用std::make_integer_sequence 与std::tuple_element代替原来的index参数递归，减少编译时间与内存消耗
类似于python的list，支持负数下标索引

 还未增加SFINAE检查判断输入的元序列是否合理
 * */

#ifndef META_META_H
#define META_META_H

#include<tuple>
#include<limits>
#include<cstddef>

#if defined(_MSVC_LANG)
#define STL_LANG _MSVC_LANG
#else
#define STL_LANG __cplusplus
#endif

#define HAS_CXX(VERSION) (STL_LANG >= VERSION)

#define STL_17 HAS_CXX(201703L)
#define STL_20 HAS_CXX(202002L)

#if STL_17

namespace meta
{
    using meta_index_t = ptrdiff_t;
    using meta_size_t = size_t;

    template<class...>struct  meta_list{};

    template<class T>
    struct type_identity
    {using type =T;};

    //未定义类型，确保每种状态下都有值
    struct undefined_t
    {
        constexpr explicit undefined_t() = default;
    }inline constexpr undefined;

    enum class meta_error_constant{error};
   enum class meta_range_constant{less,in,greater};

    using  error_constant = std::integral_constant<meta_error_constant,meta_error_constant::error>;

    template<meta_index_t _v>
    using meta_index_constant = std::integral_constant<meta_index_t,_v>;

    template<meta_index_t ..._s>
    using meta_index_sequence = std::integer_sequence<meta_index_t ,_s...>;

    template<auto value>
    using integral_constant = std::integral_constant<decltype(value),value>;
}

namespace meta
{
    template<class>struct template_traits;

    template<template<class...>class Template,class...T>
    struct template_traits<Template<T...>>
    {
        using type = Template<>;
        static constexpr meta_size_t extent = sizeof...(T);
        using value_type = void;
    };

    template<class T,T ... _value>
    struct template_traits<std::integer_sequence<T,_value...>>
    {
        using type = std::integer_sequence<T>;
        static constexpr meta_size_t extent = sizeof...(_value);
        using value_type = T;
    };

    template<class T,T _value>
    struct template_traits<std::integral_constant<T,_value>>
    {
        static constexpr meta_size_t extent = 1;
        using value_type = T;
    };

    template<class T>
    using empty_t = typename template_traits<T>::type;

    template<class T>
    using traits_value_t = typename template_traits<T>::value_type;

    template<class T>
    inline constexpr meta_size_t extent_v = template_traits<T>::extent;

    template<meta_index_t _input_index,class Template>
    inline constexpr auto  _final_index_v =
            _input_index >= 0 ? _input_index : _input_index + static_cast<meta_index_t>(extent_v<Template>);

    template<class TL,meta_index_t ..._input_index>
    struct in_range
    {
    private:
        static constexpr meta_index_t _size = meta_index_t (extent_v<TL>);

        template<meta_index_t _input>
        static constexpr meta_index_t _final_v = _final_index_v<_input,TL>;

        template<meta_index_t  _input>
        static constexpr bool _in_range_v = _size>_final_v<_input>&&(0<=_final_v<_input>);
    public:
        static constexpr bool value = (_in_range_v<_input_index>&&...);

        template<meta_index_t _input>
        static constexpr meta_range_constant range_value =  (_size<=_final_v<_input>)
                                              ?meta_range_constant::greater
                                              :(0>_final_v<_input>?meta_range_constant::less:meta_range_constant::in);
    };

    template<class TL,meta_index_t..._input_index>
    inline constexpr bool  in_range_v = in_range<TL,_input_index...>::value;

    template<class Template,meta_index_t input>
    inline constexpr  meta_range_constant meta_range_v = in_range<Template>::template range_value<input>;

    template<class  H,class...Rest>
    inline constexpr bool is_same_v = (std::is_same_v<H,Rest>&&...&&true);
}

namespace meta
{
#define TEMPLATE template<class...>class T
#define P(n) class...n
#define P1(n)n...
#define P2(N)T<N...>

    template<class...>struct merge;

    template<class...Template>
    using merge_t  =  typename merge<Template...>::type;

    template<TEMPLATE,P(A)>
    struct merge<P2(A)>
            :type_identity<T<A...>>
    {};

    template<TEMPLATE,P(A),P(B)>
    struct merge<P2(A),P2(B)>
            :type_identity<T<P1(A),P1(B)>>
    {};

    template<TEMPLATE,P(A),P(B),P(C)>
    struct merge<P2(A),P2(B),P2(C)>
            :type_identity<T<P1(A),P1(B),P1(C)>>
    {};

    template<TEMPLATE,P(A),P(B),P(C),P(D)>
    struct merge<P2(A),P2(B),P2(C),P2(D)>
            :type_identity<T<P1(A),P1(B),P1(C),P1(D)>>
    {};

    template<TEMPLATE,P(A),P(B),P(C),P(D),P(E)>
    struct merge<P2(A),P2(B),P2(C),P2(D),P2(E)>
            :type_identity<T<P1(A),P1(B),P1(C),P1(D),P1(E)>>
    {};

    template<TEMPLATE,P(A),P(B),P(C),P(D),P(E),P(F)>
    struct merge<P2(A),P2(B),P2(C),P2(D),P2(E),P2(F)>
            :type_identity<T<P1(A),P1(B),P1(C),P1(D),P1(E),P1(F)>>
    {};

    template<TEMPLATE,P(A),P(B),P(C),P(D),P(E),P(F),P(G)>
    struct merge<P2(A),P2(B),P2(C),P2(D),P2(E),P2(F),P2(G)>
            :type_identity<T<P1(A),P1(B),P1(C),P1(D),P1(E),P1(F),P1(G)>>
    {};

    template<TEMPLATE,P(A),P(B),P(C),P(D),P(E),P(F),P(G),P(H)>
    struct merge<P2(A),P2(B),P2(C),P2(D),P2(E),P2(F),P2(G),P2(H)>
            :type_identity<T<P1(A),P1(B),P1(C),P1(D),P1(E),P1(F),P1(G),P1(H)>>
    {};

    template<TEMPLATE,P(A),P(B),P(C),P(D),P(E),P(F),P(G),P(H),P(R)>
    struct merge<P2(A),P2(B),P2(C),P2(D),P2(E),P2(F),P2(G),P2(H),R...>
            :merge<T<P1(A),P1(B),P1(C),P1(D),P1(E),P1(F),P1(G),P1(H)>,merge_t<R...>>
    {};

#undef TEMPLATE
#undef P
#undef P1
#undef P2
}

namespace meta
{
    template<class,template<class...>class,class...>struct meta_transform;

    template<template<class DataType,DataType...>class Sequence,class  DataType,DataType..._value,
            template<class...>class Traits,class...Other>
    struct meta_transform<Sequence<DataType,_value...>,Traits,Other...>
    {
    private:
        using _list = meta_list<std::integral_constant<DataType,_value>...>;
        using _type = typename Traits<_list,Other...>::type;

        template<class T>
        struct _pimpl
            :type_identity<T>
        {};

        template<class...Items>
        struct _pimpl<meta_list<Items...>>
            :type_identity<Sequence<DataType,Items::value...>>
        {};
    public:
        using type = typename _pimpl<_type>::type;
    };

    template<class>struct make_index_list;

    template<template<class...>class Template,class...Items>
    struct make_index_list<Template<Items...>>
    {
    private:
        using _sequence = std::make_integer_sequence<meta_index_t ,sizeof...(Items)>;

        template<class>struct _impl;

        template<meta_index_t ..._index>
        struct _impl<meta_index_sequence<_index...>>
                :type_identity<Template<std::pair<meta_index_constant<_index>,Items>...>>
        {};
    public:
        using type = typename _impl<_sequence>::type;
    };

    template<template<class DataType,DataType...>class Sequence,class  DataType,DataType..._value>
    struct make_index_list<Sequence<DataType,_value...>>
    {
    private:
        using _sequence = std::make_integer_sequence<meta_index_t,sizeof...(_value)>;

        template<class>struct _impl;

        template<meta_index_t ..._index>
        struct _impl<meta_index_sequence<_index...>>
                :type_identity<meta_list<std::pair<meta_index_constant<_index>,std::integral_constant<DataType,_value>>...>>
        {};
    public:
        using type = typename _impl<_sequence>::type;
    };

    template<class TypeList>
    using make_index_list_t = typename make_index_list<TypeList>::type;

    template<class Template,template<class...>class Traits,class...P>
    using meta_transform_t = typename meta_transform<Template,Traits,P...>::type;
}

namespace meta
{
    template<size_t _index,class T,class  Error,class  = void>
    struct _get
            :type_identity<Error>
    {};

    template<template<class...>class Template,class...Types,size_t  _index,class Error>
    struct _get<_index,Template<Types...>,Error,std::enable_if_t<(_index< sizeof...(Types))>>
            :std::tuple_element<_index,std::tuple<Types...>>
    {};

    template<size_t _index,template<class DataType,DataType...>class Sequence,class DataType,DataType..._value,class Error>
    struct _get<_index,Sequence<DataType,_value...>,Error>
            :_get<_index,meta_list<std::integral_constant<DataType,_value>...>,Error>
    {};

    template<meta_index_t  index,class Template,class Error = undefined_t>
    struct get
            :_get<static_cast<size_t>(_final_index_v<index,Template>),Template,Error>
    {};

    template<meta_index_t  index,class Template,class Error= undefined_t>
    using get_t =  typename get<index,Template,Error>::type;

    template<meta_index_t  index,class Sequence, class Error = error_constant>
    inline constexpr  auto get_v = get_t<index,Sequence,Error>::value;

    static_assert(std::is_same_v<get_t<100,meta_list<int,double,char>,wchar_t>,wchar_t>);

    static_assert(std::is_same_v<get_t<-1,std::index_sequence<1,2,3,4>>,std::integral_constant<size_t,4>>);
    static_assert(std::is_same_v<get_t<2,std::index_sequence<1,2,3,4>>,std::integral_constant<size_t,3>>);
    static_assert(std::is_same_v<get_t<3,std::index_sequence<1,2,3,4>>,std::integral_constant<size_t,4>>);
    static_assert(std::is_same_v<get_t<4,std::index_sequence<1,2,3,4>,error_constant>,error_constant>);

    static_assert(get_v<11,std::index_sequence<1,2,3,4,5>,error_constant> == error_constant::value);
    static_assert(get_v<1,std::index_sequence<1,2,3,4,5>> == 2);

    static_assert(std::is_same_v<get_t<3,std::index_sequence<1,2,3,4,5>,error_constant>,std::integral_constant<size_t,4>>);
}

namespace meta
{
    template<class TypeList,class Empty= empty_t<TypeList>>
    struct reverse
            :type_identity<Empty>
    {};

    template<template<class...>class TypeList,class First,class...Rest,class...Reverse>
    struct reverse<TypeList<First,Rest...>,TypeList<Reverse...>>
            :reverse<TypeList<Rest...>,TypeList<First,Reverse...>>
    {};

    template<template<class DataType,DataType...>class Sequence,class DataType,
            DataType _first,DataType..._rest,DataType..._reverse>
    struct reverse<Sequence<DataType,_first,_rest...>,Sequence<DataType,_reverse...>>
            :meta_transform<Sequence<DataType,_first,_rest...>,reverse>
    {};

    template<class T,class  U,class B>
    struct unique
    {
        using type = U;
        static constexpr bool value = B::value;
    };

    template<template<class...>class TypeList,class F,class...T,class...U,class B>
    struct unique<TypeList<F,T...>,TypeList<U...>,B>
            :std::conditional_t<(std::is_same_v<F,U>||...),unique<TypeList<T...>,TypeList<U...>,std::false_type >,
                    unique<TypeList<T...>,TypeList<U...,F>,std::true_type >>
    {};

    template<template<class DataType,DataType...>class Sequence,class DataType,
            DataType _first,DataType..._rest>
    struct unique<Sequence<DataType,_first,_rest...>,meta_list<>,std::true_type >
            :meta_transform<Sequence<DataType,_first,_rest...>,unique,meta_list<>,std::true_type>
    {};

    template<class T>using unique_t = typename unique<T,empty_t<T>,std::true_type >::type;
    template<class T>using reverse_t = typename reverse<T>::type;
    template<class T>inline constexpr bool is_unique_v = unique<T,empty_t<T>,std::true_type>::value;


    static_assert(std::is_same_v<unique_t<meta_list<>>,meta_list<>>);
    static_assert(std::is_same_v<unique_t<meta_list<int,int>>,meta_list<int>>);
    static_assert(is_unique_v<std::index_sequence<>>);
    static_assert(is_unique_v<meta_list<>>);

    static_assert(std::is_same_v<reverse_t<std::index_sequence<1,2,3,4>>,std::index_sequence<
            4,3,2,1>>);
    static_assert(std::is_same_v<reverse_t<std::index_sequence<>>,std::index_sequence<
 >>);
}

namespace  meta
{
    template<class T ,class Left,class Right,class = std::bool_constant<Left::value!=Right::value&&in_range_v<T,Left::value,Right::value>>>struct swap;

    template<class T,class Left,class Right>
    struct swap<T,Left,Right,std::false_type>
            :type_identity<T>
    {};

    template<template<class...>class TypeList,class...Items,class Left,class Right>
    struct swap<TypeList<Items...>,Left,Right,std::true_type>
    {
    private:
        using _list = make_index_list_t<TypeList<Items...>>;
        static constexpr meta_index_t _left_index = _final_index_v<Left::value,TypeList<Items...>>;
        static constexpr meta_index_t _right_index = _final_index_v<Right::value,TypeList<Items...>>;
        using _pair_l = get_t<_left_index,_list>;
        using _pair_r = get_t<_right_index,_list>;

        template<class>struct _impl{};

        template<class...Pairs>
        struct _impl<TypeList<Pairs...>>
        {
            using type =TypeList<std::conditional_t<(std::tuple_element_t<0,Pairs>::value == _left_index),
                    std::tuple_element_t<1,_pair_r>,
                    std::conditional_t<(std::tuple_element_t<0,Pairs>::value == _right_index),std::tuple_element_t<1,_pair_l>,std::tuple_element_t<1,Pairs>>>...>;
        };
    public:
        using type = typename _impl<_list>::type;
    };

    template<template<class DataType,DataType...>class Sequence,class DataType,DataType..._value,class Left,class Right>
    struct swap<Sequence<DataType,_value...>,Left,Right,std::true_type >
            :meta_transform<Sequence<DataType,_value...>,swap,Left,Right,std::true_type>
    {};

    template<class T,meta_index_t left,meta_index_t right>
    using swap_t = typename swap<T,meta_index_constant<_final_index_v<left,T>>,meta_index_constant<_final_index_v<right,T>>>::type;

    static_assert(std::is_same_v<swap_t<std::index_sequence<1,2,3,4,5,6>,2,3>,std::index_sequence<1,2,4,3,5,6>>);
    static_assert(std::is_same_v<swap_t<std::index_sequence<1>,2,3>,std::index_sequence<1>>);
    static_assert(std::is_same_v<std::index_sequence<>,swap_t<std::index_sequence<>,11,10>>);
    static_assert(std::is_same_v<swap_t<meta_list<>,1,2>,meta_list<>>);
}

namespace meta
{
    template<class,class...>
    struct append
            :type_identity<undefined_t>
    {};

    template<class,class...>
    struct prepend
            :type_identity<undefined_t>
    {};

    template<class T,class>
    struct _to_sequence
            :type_identity<T>
    {};

    template<class T>
    struct _to_meta_list
            :type_identity<T>
    {};

    template<class...Constant,template<class DataType,DataType...>class Sequence,class DataType,
            DataType ...elements>
    struct _to_sequence<meta_list<Constant...>,Sequence<DataType,elements...>>
            :type_identity<Sequence<DataType,Constant::value...>>
    {};

    template<template<class DataType,DataType...>class Sequence,class DataType,DataType ...elements>
    struct _to_meta_list<Sequence<DataType,elements...>>
            :type_identity<meta_list<integral_constant<elements>...>>
    {};

    template<template<class...>class TypeList,class...Items,class...Add>
    struct append<TypeList<Items...>,Add...>
            :type_identity<TypeList<Items...,Add...>>
    {};

    template<template<class...>class TypeList,class...Items,class...Add>
    struct prepend<TypeList<Items...>,Add...>
            :type_identity<TypeList<Add...,Items...>>
    {};

    template<template<class DataType,DataType...>class Sequence,class DataType,
            DataType ...elements,DataType...add>
    struct prepend<Sequence<DataType,elements...>,Sequence<DataType,add...>>
            :type_identity<Sequence<DataType,add...,elements...>>
    {};

    template<template<class DataType,DataType...>class Sequence,class DataType,
            DataType ...elements,DataType...add>
    struct append<Sequence<DataType,elements...>,Sequence<DataType,add...>>
            :type_identity<Sequence<DataType,elements...,add...>>
    {};

    template<class T>
    using _to_meta_list_t = typename _to_meta_list<T>::type;

    template<class T,class U>
    using _to_sequence_t = typename _to_sequence<T,U>::type;

    template<class T,class...Add>
    using append_t = typename append<T,Add...>::type;

    template<class T,class...Add>
    using prepend_t = typename prepend<T,Add...>::type;

    template<class T,template<class/*Index Parameter*/,class,/*Element Parameters*/class...>class Pred,class...OtherParameters>
    struct _meta_filter
    {
        enum  _filter_type
        {
            _insert,_remove,_replace
        };

        using _pair_list = make_index_list_t<T>;

        template<_filter_type,class TL,class...>
        struct _impl
                :type_identity<TL>
        {};

        template<template<class...>class Template,class...Items,meta_index_t  ... _index>
        struct _impl<_remove,Template<std::pair<meta_index_constant<_index>,Items>...>>
                :merge<Template<>,std::conditional_t<Pred<meta_index_constant<_index>,Items,OtherParameters...>::value,Template<>,Template<Items>>...>
        {};

        template<template<class...>class Template,class...Items,meta_index_t  ... _index,class...Add>
        struct _impl<_insert,Template<std::pair<meta_index_constant<_index>,Items>...>,Add...>
                :merge<Template<>,
                std::conditional_t<Pred<meta_index_constant<_index>,Items,OtherParameters...>::value,Template<Add...,Items>,Template<Items>>...>
        {};

        template<template<class...>class Template,class...Items,meta_index_t  ... _index,class...Replace>
        struct _impl<_replace,Template<std::pair<meta_index_constant<_index>,Items>...>,Replace...>
                :merge<Template<>,
                std::conditional_t<Pred<meta_index_constant<_index>,Items,OtherParameters...>::value,Template<Replace...>,Template<Items>>...>
        {};

        template<class...Add>
        using _insert_type = typename _impl<_insert,_pair_list ,Add...>::type;
        using _remove_type = typename _impl<_remove,_pair_list>::type;

        template<class...Replace>
        using _replace_type = typename _impl<_replace,_pair_list ,Replace...>::type;
    };

    template<class T,template<class>class UnaryPred>
    struct remove_if
    {
    private:
        template<class /*Index Constant*/,class Item>
        using _pred = UnaryPred<Item>;
    public:
        using type = typename _meta_filter<T,_pred>::_remove_type ;
    };

    template<class T,meta_index_t  index>
    struct remove
    {
        template<class Index,class>
        using _pred = std::bool_constant<Index::value == _final_index_v<index,T>>;

        using type = _to_sequence_t<typename _meta_filter<_to_meta_list_t<T>,_pred>::_remove_type,T>;
    };

    template<class T,class Type>
    struct erase
    {
    private:
        template<class,class Item>
        using _pred = std::is_same<Item,Type>;
    public:
        using type = typename _meta_filter<T,_pred>::_remove_type ;
    };

    template<class T,size_t nums>
    struct  take
    {
        template<class Index,class,class C>using _take_pred = std::bool_constant<(Index::value>=C::value)>;
        using type = _to_sequence_t<typename _meta_filter<_to_meta_list_t<T>,_take_pred,std::integral_constant<size_t,nums>>::_remove_type,T>;
    };

    template<class T,meta_index_t  index,class...Add>
    struct insert
    {
        static constexpr auto _range_v = meta_range_v<T,index>;
        template<class Index,class>
        using _pred = std::bool_constant<Index::value == _final_index_v<index,T>>;

        using type = std::conditional_t<_range_v == meta_range_constant::in,
                typename _meta_filter<T,_pred>::template _insert_type<Add...>,
                        std::conditional_t<_range_v == meta_range_constant::less,
                        prepend_t<T,Add...>,
                        append_t<T,Add...>>
                >;
    };

    template<class T,class Type>
    using erase_t = typename  erase<T,Type>::type;

    template<class T,meta_index_t  index>
    using remove_t = typename remove<T,index>::type;

    template<class T,template<class>class Pred>
    using remove_if_t = typename remove_if<T,Pred>::type;

    template<class T,meta_index_t  index,class...Add>
    using insert_t = typename insert<T,index,Add...>::type;

    template<class Index,class,class ReplaceIndex>using _replace_pred = std::bool_constant<Index::value == ReplaceIndex::value>;

    template<class T,meta_index_t index,class ... Replace>
    using replace_t = typename _meta_filter<T,_replace_pred,meta_index_constant<_final_index_v<index,T>>>::template _replace_type<Replace...>;

    template<class T,size_t nums>
    using take_t = typename take<T,nums>::type ;

    template<class T,class Type>
    inline constexpr size_t count_v = extent_v<T>-extent_v<erase_t<T,Type>>;

    static_assert(std::is_same_v<remove_t<std::index_sequence<9,0,8,6,5>,-1>,std::index_sequence<9,0,8,6>>);
    static_assert(std::is_same_v<remove_t<meta_list<>,22>,meta_list<>>);
    static_assert(std::is_same_v<remove_t<meta_list<int,double,char,wchar_t>,-1>,meta_list<int,double,char>>);
    static_assert(std::is_same_v<erase_t<std::tuple<int,double,char,int,void>,int>,std::tuple<double,char,void>>);

    static_assert(std::is_same_v<take_t<meta_list<int,double,char,void>,2>,meta_list<int,double>>);
    static_assert(std::is_same_v<take_t<meta_list<>,2>,meta_list<>>);
    static_assert(std::is_same_v<replace_t<meta_list<>,100>,meta_list<>>);
    static_assert(std::is_same_v<replace_t<meta_list<int,double,char>,-2,void>,meta_list<int,void,char>>);

    template<class...>class variant{};

    static_assert(is_same_v<variant<int,double,double>,insert_t<variant<int,double>,-1,double>>);

    static_assert(is_same_v<insert_t<variant<int,double,char>,1,void>,variant<int,void,double,char>>);
    static_assert(is_same_v<insert_t<variant<int,double,char>,0,void>,variant<void,int,double,char>>);
    static_assert(is_same_v<insert_t<variant<int,double,char>,2,void>,variant<int,double,void,char>>);

    static_assert(is_same_v<insert_t<variant<double*>,5555,void,int>,variant<double*,void,int>>);
    static_assert(is_same_v<insert_t<variant<>,-1,char*>,variant<char*>>);
    static_assert(is_same_v<insert_t<variant<int,char>,-11,char*>,variant<char*,int,char>>);
    static_assert(is_same_v<insert_t<variant<int,char>,-1,char*>,variant<int,char*,char>>);
    static_assert(is_same_v<insert_t<variant<int,char>,-2,char*>,variant<char*,int,char>>);
}

namespace meta::advance
{
    template<class Sequence,meta_index_t index,traits_value_t<Sequence>...add>
    struct insert
    {
        using _type =   meta::insert_t<_to_meta_list_t<Sequence>,index,integral_constant<add>...>;
        using type = _to_sequence_t<_type,Sequence>;
    };

    template<class T,meta_index_t  index,traits_value_t<T>...add>
    using insert_t = typename insert<T,index,add...>::type;

    template<class Sequence,traits_value_t<Sequence> value>
    struct erase
    {
    private:
        using _type = meta::erase_t<_to_meta_list_t<Sequence>,integral_constant<value>>;
    public:
        using type = _to_sequence_t<_type,Sequence>;
    };

    template<class Sequence,traits_value_t<Sequence>...add>
    struct prepend
    {
    private:
        using _type = meta::prepend_t<_to_meta_list_t<Sequence>,integral_constant<add>...>;
    public:
        using type = _to_sequence_t<_type,Sequence>;
    };

    template<class Sequence,traits_value_t<Sequence>...add>
    struct append
    {
    private:
        using _type = meta::append_t<_to_meta_list_t<Sequence>,integral_constant<add>...>;
    public:
        using type = _to_sequence_t<_type,Sequence>;
    };

    template<class Sequence,traits_value_t<Sequence> value>
    using  erase_t = typename erase<Sequence,value>::type;

    template<class Sequence,traits_value_t<Sequence>... add>
    using  prepend_t = typename prepend<Sequence,add...>::type;

    template<class Sequence,traits_value_t<Sequence>... add>
    using  append_t = typename append<Sequence,add...>::type;

    static_assert(std::is_same_v<erase_t<std::index_sequence<4,1,2,3,4,5,4>,4>,std::index_sequence<1,2,3,5>>);
    static_assert(std::is_same_v<prepend_t<std::index_sequence<4,1,2,3,4,5,4>,4>,std::index_sequence<4,4,1,2,3,4,5,4>>);
    static_assert(std::is_same_v<append_t<std::index_sequence<4,1,2,3,4,5,4>,4>,std::index_sequence<4,1,2,3,4,5,4,4>>);
    static_assert(is_same_v<insert_t<std::index_sequence<1,2,3,4,5>,0>,std::index_sequence<1,2,3,4,5>>);

    static_assert(is_same_v<
            std::index_sequence<1,2,3,4,5>,
            insert_t<std::index_sequence<1,3,4,5>,1,2>,
            insert_t<std::index_sequence<1,2,4,5>,2,3>,
            insert_t<std::index_sequence<1,2,3,5>,3,4>
    >);

    static_assert(is_same_v<insert_t<std::index_sequence<1>,-1,2>,std::index_sequence<2,1>>);
}

namespace meta
{
    template<class A,class B>
    struct less
            :std::bool_constant<sizeof(A)<sizeof (B)>
    {};

    template<class A,class B>
    struct greater
            :std::bool_constant<(sizeof(A)>sizeof (B))>
    {};

    template<auto v1,auto v2>
    struct less<std::integral_constant<decltype(v1),v1>,std::integral_constant<decltype(v2),v2>>
            :std::bool_constant<(v1<v2)>
    {};

    template<auto v1,auto v2>
    struct greater<std::integral_constant<decltype(v1),v1>,std::integral_constant<decltype(v2),v2>>
            :std::bool_constant<(v1>v2)>
    {};

    template<class T,template<class,class>class Pred= greater,class  = void>
    struct insertion_sort
    {
    private:
        //插入实现
        template<class Ordered,class New,class O= empty_t<Ordered>>
        struct _insert
                :type_identity<O>
        {};

        template<template<class...>class Template,class New,class Head,class...Rest,class...O>
        struct _insert<Template<Head,Rest...>,New,Template<O...>>
                :std::conditional_t<(sizeof...(Rest)==0),
                std::conditional_t<Pred<New,Head>::value,type_identity<Template<O...,New,Head>>,type_identity<Template<O...,Head,New>>>,
                std::conditional_t<Pred<New,Head>::value,
                type_identity<Template<O...,New,Head,Rest...>>,
                _insert<Template<Rest...>,New,Template<O...,Head>>>>
        {};

        template<class Template,class Empty>
        struct _impl
                :type_identity<Empty>
        {};

        template<template<class...>class Template,class Head,class...Rest,class Empty>
        struct _impl<Template<Head,Rest...>,Empty>
                :_impl<Template<Rest...>,typename _insert<Empty,Head>::type>
        {};

        template<template<class...>class Template,class Head,class...Rest>
        struct _impl<Template<Head,Rest...>,Template<>>
                :_impl<Template<Rest...>,Template<Head>>
        {};

        template<template<class DataType,DataType...>class Sequence,class DataType,DataType..._value>
        struct _impl<Sequence<DataType,_value...>,Sequence<DataType>>
                :meta_transform<Sequence<DataType,_value...>,_impl,meta_list<>>
        {};

    public:
        using type = typename _impl<T,empty_t<T>>::type;
    };

    template<class T,template<class,class>class Pred = greater>
    using insertion_sort_t = typename insertion_sort<T,Pred>::type;

    static_assert(std::is_same_v<insertion_sort_t<meta_list<int,char>>,meta_list<int,char>>);
    static_assert(std::is_same_v<insertion_sort_t<meta_list<char,int>>,meta_list<int,char>>);
    static_assert(std::is_same_v<insertion_sort_t<meta_list<double,wchar_t,char,int,double>>,meta_list<double,double,int,wchar_t,char>>);
    static_assert(std::is_same_v<insertion_sort_t<std::index_sequence<10,5,7,9,11,4,5>>,std::index_sequence<11, 10, 9, 7, 5, 5, 4>>);
    static_assert(std::is_same_v<insertion_sort_t<std::index_sequence<>>,std::index_sequence<>>);
}

namespace meta
{
    template<class T,meta_index_t  _left,meta_index_t _right,auto _pivot>
    struct _quick_sort_partition
    {};

    template<class T,meta_index_t  _left,meta_index_t  _right>
    struct _quick_sort_heapify
    {
        static constexpr auto _pivot = get_v<_left,T>;
    };

    template<class T,template<class,class>class BinaryPred>
    struct quick_sort
    {
        template<class>struct _impl;

        using type = typename _impl<T>::type;
    };
}

namespace meta
{
    //获取当前根节点的最值结点
    template<class T,template<class,class>class Pred,meta_index_t  _root,meta_index_t  _end,
            bool = _root*2+1<=_end,bool = _root*2+2<=_end>
    struct _heap_sort_extreme_idx
            :std::integral_constant<meta_index_t ,_root>
    {};

    template<class T,template<class,class>class Pred,meta_index_t  _root,meta_index_t  _end>
    struct _heap_sort_extreme_idx<T,Pred,_root,_end,true,false>
    {
        static constexpr auto _left_child_value  =   get_v<2*_root+1,T>;
        static constexpr auto _root_value = get_v<_root,T>;
        static constexpr meta_index_t  value =  Pred<integral_constant<_left_child_value>,integral_constant<_root_value>>::value?2*_root+1:_root;
    };

    template<class T,template<class,class>class Pred,meta_index_t  _root,meta_index_t  _end>
    struct _heap_sort_extreme_idx<T,Pred,_root,_end,true,true>
    {
        template<auto left,auto right>
        static constexpr bool _pred_v = Pred<integral_constant<left>,integral_constant<right>>::value;

        static constexpr meta_index_t _left_child = 2*_root+1;
        static constexpr auto _left_child_value  =   get_v<_left_child,T>;
        static constexpr auto _right_child_value  =   get_v<_left_child+1,T>;
        static constexpr auto _root_value = get_v<_root,T>;
        static constexpr auto value = _pred_v<_left_child_value,_right_child_value>?
                                      ( _pred_v<_left_child_value,_root_value>?_left_child:_root)
                                      :(_pred_v<_right_child_value,_root_value>?_left_child+1:_root);
    };

    template<class Sequence,
            template<class,class>class Pred,
            meta_index_t _current,meta_index_t _end,
            meta_index_t _extreme =  _heap_sort_extreme_idx<Sequence,Pred,_current,_end>::value>
    struct _heap_sort_sift_down
            :std::conditional_t<_extreme!=_current,
            _heap_sort_sift_down<swap_t<Sequence,_current,_extreme>,Pred,_extreme,_end>,
            type_identity<Sequence>
            >
    {};

    template<class Sequence,template<class,class>class Pred,meta_index_t  _current,meta_index_t _parent = (_current-1)/2>
    struct _heap_sort_sift_up
            :std::conditional_t<(_current>0),
            std::conditional_t<Pred<integral_constant<get_v<_current,Sequence>>,integral_constant<get_v<_parent,Sequence>>>::value,
            _heap_sort_sift_up<swap_t<Sequence,_current,_parent>,Pred,_parent>,type_identity<Sequence>>,
            type_identity<Sequence>
            >
    {};

    template<class Sequence,template<class,class>class Pred,meta_index_t _root,meta_index_t _end>
    using  _heap_sort_sift_down_t = typename _heap_sort_sift_down<Sequence,Pred,_root,_end>::type;

    template<class T,template<class,class>class Pred = less>
    class heap
    {
        static constexpr meta_index_t  _size = static_cast<meta_index_t>(extent_v<T>);

        template<class S,meta_index_t _index>
        struct _make_heap
                :std::conditional_t<(_index>=0),
                        _make_heap<_heap_sort_sift_down_t<S,Pred,_index, _size - 1>, _index - 1>,
                        type_identity<S>>
        {};

        template<class S,meta_index_t  _end>
        struct _sort
                :std::conditional_t<(_end >0),
                        _sort<_heap_sort_sift_down_t<swap_t<S,0,_end>,Pred,0,_end-1>, _end - 1>,
                        type_identity<S>>
        {};
    public:
        using type = typename _make_heap<T,_size/2-1>::type;
        using sort = typename _sort<type,_size>::type;
    };

    template<class T,template<class,class>class Pred = greater>
    using build_heap_t = typename heap<T,Pred>::type;

    template<class T,template<class,class>class Pred = greater>
    using heap_sort_t = typename heap<T,Pred>::sort;

    static_assert(std::is_same_v<build_heap_t<std::index_sequence<24,44, 18 ,52 ,92 ,74 ,44 ,15 ,57 ,17>>,
            std::index_sequence<92, 57 ,74, 52 ,44 ,18, 44, 15 ,24 ,17>>);

    static_assert(std::is_same_v<heap_sort_t<std::index_sequence<24,44, 18 ,52 ,92 ,74 ,44 ,15 ,57 ,17>>,
            std::index_sequence<15, 17, 18 ,24 ,44, 44 ,52 ,57 ,74, 92>>);

    static_assert(is_same_v<insertion_sort_t<std::index_sequence<24,44, 18 ,52 ,92 ,74 ,44 ,15 ,57 ,17>>,
                  reverse_t< std::index_sequence<15, 17, 18 ,24 ,44, 44 ,52 ,57 ,74, 92>>,
                  std::index_sequence<92, 74, 57, 52, 44, 44, 24, 18, 17, 15>>);
}

namespace meta
{
    template<class>inline constexpr bool _is_meta_sequence_v = false;

    template<auto _value>
    inline constexpr bool _is_meta_sequence_v<integral_constant<_value>> = false;

    template<template<class DataType,DataType...>class Sequence,class DataType,DataType..._value>
    inline constexpr bool _is_meta_sequence_v<Sequence<DataType,_value...>>  =true;

    template<auto _value ,meta_index_t  _id,meta_index_t _parent,int _rotation>
    struct _huffman_tree_node
    {
        static constexpr auto value = _value;
        static constexpr meta_index_t id =_id;
        //如果parent == -1，这是一个没有合并的节点
        static constexpr meta_index_t parent = _parent;

        //rotation==0 left subtree
        //rotation==1 right subtree
        //rotation ==-1 root node
        static constexpr meta_index_t rotation = _rotation;
    };

    template<class T,class F/*最小的节点*/,class S/**倒数第二小的节点**/,size_t _new_id>
    struct _huffman_find
    {
        using  first = F;
        using second  = S;
    };

    //template<auto _value ,meta_index_t  _id,meta_index_t _parent,int _rotation>

    template<class FirstNode,class ...RestNodes,class F,class S,meta_index_t _new_id>
    struct _huffman_find<meta_list<FirstNode,RestNodes...>,F,S,_new_id>
            :std::conditional_t<(FirstNode::value<F::value&&FirstNode::parent == -1),
            _huffman_find<meta_list<RestNodes...>,_huffman_tree_node<FirstNode::value,FirstNode::id,_new_id,0>,_huffman_tree_node<F::value,F::id,_new_id,1>,_new_id>,
            std::conditional_t<(S::value>FirstNode::value)&&FirstNode::parent==-1,
            _huffman_find<meta_list<RestNodes...>,F,_huffman_tree_node<FirstNode::value,FirstNode::id,_new_id,1>,_new_id>,
            _huffman_find<meta_list<RestNodes...>,F,S,_new_id>
            >//conditional
            >
    {};

    template<class Type,class Input/*TypeList*/,size_t _new_id>
    struct _huffman_insert
    {
    private:
        using _huffman_tree_max_node = _huffman_tree_node<std::numeric_limits<Type>::max(),-1,-1,-1>;

        template<class T>
        using _huffman_find_first_t = typename _huffman_find<T,_huffman_tree_max_node,_huffman_tree_max_node,_new_id>::first;

        template<class T>
        using _huffman_find_second_t = typename _huffman_find<T,_huffman_tree_max_node,_huffman_tree_max_node,_new_id>::second;

        using _first = _huffman_find_first_t<Input>;
        using _second = _huffman_find_second_t<Input>;

        template<class>
        struct _impl{};

        template<class...Items>
        struct _impl<meta_list<Items...>>
                :type_identity<meta_list<std::conditional_t<Items::id==_first::id||Items::id==_second::id,
                std::conditional_t<Items::id == _first::id,_first,_second>,Items>...>>
        {};
    public:
        using type= typename append<typename _impl<Input>::type,_huffman_tree_node<_first::value+_second::value,_new_id,-1,-1>>::type;
    };

    template<class Type,class TypeList,meta_index_t _new_id>
    using _huffman_insert_t =typename _huffman_insert<Type,TypeList,_new_id>::type;

    template<class Sequence>
    struct make_huffman_tree
    {
    private:
        static_assert(_is_meta_sequence_v<Sequence>,"requires meta sequence!");

        using _value_type  = traits_value_t<Sequence>;

        template<class,class>
        struct _make_node_list
        {};

        template<template<class DataType,DataType...>class  S,class DataType,DataType..._value,meta_index_t..._id>
        struct _make_node_list<S<DataType,_value...>,meta_index_sequence<_id...>>
                :type_identity<meta_list<_huffman_tree_node<_value,_id,-1,-1>...>>
        {};

        using _make_node_list_t =typename _make_node_list<Sequence,std::make_integer_sequence<meta_index_t,extent_v<Sequence>>>::type;

        template<class T,meta_index_t _new_id,meta_index_t _end>
        struct _impl
        {};

        template<class...Nodes,meta_index_t _new_id,meta_index_t _end>
        struct _impl<meta_list<Nodes...>,_new_id,_end>
        :std::conditional_t<(_new_id<=_end),
                _impl<_huffman_insert_t<_value_type,meta_list<Nodes...>,_new_id>,_new_id+1,_end>,
                type_identity<meta_list<Nodes...>>>
        {};

    public:
        using type= typename  _impl<_make_node_list_t ,meta_index_t (extent_v<Sequence>),meta_index_t(2*extent_v<Sequence>-2)>::type ;
    };

    template<class Sequence>
    using make_huffman_tree_t = typename make_huffman_tree<Sequence>::type;

    using _test_sequence = std::index_sequence<5,6,8,4,7,3>;

    static_assert(std::is_same_v<make_huffman_tree_t<_test_sequence>,
                  meta_list<
                  _huffman_tree_node<size_t(5), 0, 7, 0>,
                  _huffman_tree_node<size_t(6), 1, 7, 1>,
                  _huffman_tree_node<size_t(8), 2, 9, 0>,
                  _huffman_tree_node<size_t(4), 3, 6, 1>,
                  _huffman_tree_node<size_t(7), 4, 8, 0>,
                  _huffman_tree_node<size_t(3), 5, 6, 0>,
                  _huffman_tree_node<size_t(7), 6, 8, 1>,
                  _huffman_tree_node<size_t(11), 7, 9, 1>,
                  _huffman_tree_node<size_t(14), 8, 10, 0>,
                  _huffman_tree_node<size_t(19), 9, 10, 1>,
                  _huffman_tree_node<size_t(33), 10, -1, -1>>>);

    static_assert(std::is_same_v<make_huffman_tree_t<std::index_sequence<99>>,meta_list<_huffman_tree_node<size_t(99),0,-1,-1>>>);
    static_assert(std::is_same_v<make_huffman_tree_t<std::index_sequence<99,88>>,
            meta_list<meta::_huffman_tree_node<99ULL, 0, 2, 1>,
                    _huffman_tree_node<88ULL, 1, 2, 0>,
                    _huffman_tree_node<187ULL, 2, -1, -1>>>);

    ///Main Template  of huffman_code
    template<class T>
    class huffman_code
    {
        static_assert(_is_meta_sequence_v<T>);

        using _tree= make_huffman_tree_t<T>;

        template<class Current,class Code =  std::integer_sequence<int>>
        struct _single_code
                :std::conditional_t<Current::parent!=-1,
                _single_code<get_t<Current::parent,_tree>,advance::prepend_t<Code,Current::rotation>>,
                type_identity<Code>>
        {};

        template<class L,bool = bool(extent_v<T>)>
        struct _impl
                :type_identity<L>
        {};

        template<class...Nodes>
        struct _impl<meta_list<Nodes...>,true>
                :type_identity<meta_list<typename _single_code<Nodes>::type...>>
        {};

    public:
        using type = typename _impl<take_t<_tree,extent_v<T>>>::type;
    };

    template<class T>
    using huffman_code_t = typename huffman_code<T>::type;

    static_assert(is_same_v<huffman_code_t<std::index_sequence<>>,meta_list<>>);
    static_assert(is_same_v<huffman_code_t<std::index_sequence<5,6,8,4,7,3>>,
            meta_list<std::integer_sequence<int, 1, 1, 0>,
            std::integer_sequence<int, 1, 1, 1>,
            std::integer_sequence<int, 1, 0>,
            std::integer_sequence<int, 0, 1, 1>,
            std::integer_sequence<int, 0, 0>,
            std::integer_sequence<int, 0, 1, 0>>>);

}

namespace meta
{
    namespace _merge_sort
    {
        template<class T,template<class,class>class Binary = less>
        struct _min_sort
                :type_identity<T>
        {};

        template<template<class...>class TL,class A,class B,template<class,class>class Binary>
        struct _min_sort<TL<A,B>,Binary>
            :std::conditional_t<Binary<A,B>::value,type_identity<TL<A,B>>,type_identity<TL<B,A>>>
        {};

        template<class  S1,class S2,class O,template<class,class>class Binary =less>
        struct _merge_two_sequence
                :std::conditional_t<bool(extent_v<S1>),
                        merge<O,S1>,
                        merge<O,S2>>
        {};

        template<template<class,class>class Binary,
                template<class...>class TL,
                class H1,class H2,class...A,class...B,class...C>
        struct _merge_two_sequence<TL<H1,A...>,TL<H2,B...>,TL<C...>,Binary>
                :std::conditional_t<Binary<H1,H2>::value,
                        _merge_two_sequence<TL<A...>,TL<H2,B...>,TL<C...,H1>,Binary>,
                        _merge_two_sequence<TL<H1,A...>,TL<B...>,TL<C...,H2>,Binary>>
        {};

        template<class A,class B,template<class,class>class BinaryPred =less>
        using _merge_two_sequence_t = typename _merge_two_sequence<A,B,empty_t<A>,BinaryPred>::type;

       template<class T,size_t _half = extent_v<T>/2>
       struct _split_two
       {
           template<class Index,class,class C>using _left_pred = std::bool_constant<(Index::value>=C::value)>;
           template<class Index,class,class C>using _right_pred = std::bool_constant<(Index::value<C::value)>;
           using _left = _to_sequence_t<typename _meta_filter<_to_meta_list_t<T>,_left_pred,std::integral_constant<size_t,_half>>::_remove_type,T>;
           using _right= _to_sequence_t<typename _meta_filter<_to_meta_list_t<T>,_right_pred,std::integral_constant<size_t,_half>>::_remove_type,T>;
       };

       template<class T>
       using _split_two_left = typename _split_two<T>::_left;

        template<class T>
        using _split_two_right = typename _split_two<T>::_right;

        static_assert(std::is_same_v<_split_two_right<meta_list<int,char,double>>,meta_list<char,double>>);
        static_assert(std::is_same_v<_split_two_left<meta_list<int>>,meta_list<>>);

    }

    template<class T,template<class,class>class Pred = less>
    struct merge_sort
    {
        using _left = typename merge_sort<_merge_sort::_split_two_left<T>,Pred>::type;
        using _right = typename merge_sort<_merge_sort::_split_two_right<T>,Pred>::type;
        using type= _merge_sort::_merge_two_sequence_t<_left,_right,Pred>;
    };

    template<template<class...>class Template,class T,template<class,class>class Pred >
    struct merge_sort<Template<T>,Pred>
            :type_identity<Template<T>>
    {};

    template<template<class...>class Template,template<class,class>class Pred >
    struct merge_sort<Template<>,Pred>
            :type_identity<Template<>>
    {};

    template<template<class...>class Template,class T,class U,template<class,class>class Pred >
    struct merge_sort<Template<T,U>,Pred>
            :_merge_sort::_min_sort<Template<T,U>,Pred>
    {};

    template<template<class DataType,DataType...>class Sequence,class DataType,DataType...elements>
    struct merge_sort<Sequence<DataType,elements...>>
    {
        using _ml = _to_meta_list_t<Sequence<DataType,elements...>>;
        using _type =  typename merge_sort<_ml>::type;
        using type = _to_sequence_t<_type,Sequence<DataType,elements...>>;
    };

    template<class T,template<class,class>class Pred = less>
    using merge_sort_t =typename merge_sort<T,Pred>::type;

    static_assert(std::is_same_v<merge_sort_t<std::index_sequence<24,44, 18 ,52 ,92 ,74 ,44 ,15 ,57 ,17>>,
            std::index_sequence<15, 17, 18 ,24 ,44, 44 ,52 ,57 ,74, 92>>);

}
namespace meta
{
    inline constexpr bool is_arc = true;//有向
    inline constexpr bool is_net = true;//带权

    enum class graph_type{};

    template<class T,size_t _in_degree/*入度*/,size_t _out_degree/*出度*/,bool = is_arc>
    struct _graph_node
    {
        static constexpr size_t in  = _in_degree;
        static  constexpr size_t out = _out_degree;
    };

    template<class T,size_t _in_degree/*入度*/,size_t _out_degree/*出度*/>
    struct _graph_node<T,_in_degree,_out_degree,false>
    {
        static constexpr size_t degree  = _in_degree+_out_degree;
    };

    template<bool _in,class T>
    struct _update_graph_node
    {};

    template<bool v,class T,size_t _in_degree/*入度*/,size_t _out_degree/*出度*/>
    struct _update_graph_node<v,_graph_node<T,_in_degree,_out_degree,false>>
    {
        using type= _graph_node<T,_in_degree+1,_out_degree,false>;
    };

    template<bool _in,class T,size_t _in_degree/*入度*/,size_t _out_degree/*出度*/>
    struct _update_graph_node<_in,_graph_node<T,_in_degree,_out_degree, true>>
    {
        using type= std::conditional_t<_in,_graph_node<T,_in_degree+1,_out_degree,false>,
                _graph_node<T,_in_degree,_out_degree+1,false>>;
    };


    template<class G,class Begin,class To,bool/*arc*/,bool/*net*/>
    struct add_graph_edge
    {


    };

    template<class>
    struct make_adjacency_matrix
    {};
}

namespace meta
{
    struct null_node{};

    namespace _tree
    {
        template<auto _value,class L,class R,class P>
        struct _binary_tree_node
        {
            using left_subtree =  L;
            using right_subtree =  R;
            using parent = P;
            static constexpr auto value = _value;
        };

        template<auto _value,class P>
        using  leaf_node = _binary_tree_node<_value,null_node,null_node,P>;

        template<class Tree,auto _new_value>
        struct _bst_insert
        {
            template<class Cur,class P>
            struct _create_new_node
                    :std::conditional_t<(Cur::value>_new_value),
                    _create_new_node<typename Cur::left_subtree,Cur>,
                    _create_new_node<typename  Cur::right_subtree,Cur>>
            {};

            template<class P>
            struct _create_new_node<null_node,P>
            {
                using type = _binary_tree_node<_new_value,null_node,null_node,P>;

                using parent = _binary_tree_node<P::value,
                std::conditional_t<(P::value>type::value),type,typename P::left_subtree>,
                std::conditional_t<(P::value<type::value),type,typename P::right_subtree>,
                typename P::parent>;
            };

            template<class T,bool = bool(extent_v<Tree>)>
            struct _find_root
                    :get<0,Tree,null_node>
            {};

            using _root_t = typename _find_root<Tree>::type;
            using _new_node_t = typename _create_new_node<_root_t,null_node>::type;
            using _update_parent_t = typename _create_new_node<_root_t,null_node>::parent;

            template<class L>
            struct _impl
                    :type_identity<L>
            {};

            template<class ... Elements>
            struct _impl<meta_list<Elements...>>
            {
                using _type= meta_list<std::conditional_t<Elements::value == _update_parent_t::value,_update_parent_t,Elements>...>;
                using type = append_t<Tree,_new_node_t>;
            };

            using type = typename _impl<Tree>::type;
        };

        template<class Tree,auto _new>
        using _bst_insert_t  = typename _bst_insert<Tree,_new>::type;
    }

    template<class T>
    struct make_bst
    {
        using _value_type= traits_value_t<T>;

        static_assert(_is_meta_sequence_v<T>);

        using _items = _to_meta_list_t<unique_t<T>>;

        template<class >
        struct _init
        {};

        template<_value_type ..._values>
        struct _init<std::integer_sequence<_value_type ,_values...>>
        {
            using type = meta_list<_tree::_binary_tree_node<_values,null_node,null_node,null_node>...>;
        };

        using _init_t = typename _init<T>::type;

        template<class Tree,class L>
        struct _impl
                :type_identity<Tree>
        {};

        template<class Tree,class H,class...Items>
        struct _impl<Tree,meta_list<H,Items...>>
                :_impl<_tree::_bst_insert_t<Tree,H::value>,meta_list<Items...>>
        {};

        using type = typename _impl<meta_list<>,_items>::type;
    };

    template<class T>
    using make_bst_t = typename make_bst<T>::type;


    static_assert(is_same_v<make_bst_t<std::index_sequence<5,6,7>>,int>);

}


//5,6,2,9,8,7
#endif//only cxx17
#undef STL_LANG
#undef STL_17
#undef STL_20
#endif //META_META_H
