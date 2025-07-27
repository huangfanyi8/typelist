/*
 *
 * */

#ifndef META_META_H
#define META_META_H

#include<utility>
#include <cstddef>
#include<array>
#include<limits>

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

    enum class meta_constant{error,before,after};

    using  error_constant = std::integral_constant<meta_constant,meta_constant::error>;

    template<meta_index_t _v>
    using meta_index_constant = std::integral_constant<meta_index_t,_v>;

    template<meta_index_t ..._s>
    using meta_index_sequence = std::integer_sequence<meta_index_t ,_s...>;
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
    public:
        enum  range{less,greater,in};
    private:
        template<class,class = void>
        struct _impl
                :std::false_type
        {};

        template<range..._value>
        struct _impl<std::integer_sequence<range,_value...>,std::enable_if_t<((_value==in)&&...)>>
                :std::true_type
        {};
        static constexpr meta_index_t _size = meta_index_t (extent_v<TL>);
    public:
        using type = std::integer_sequence<range,((_size<=_final_index_v<_input_index,TL>)
                                                  ?greater
                                                  :0>_final_index_v<_input_index,TL>
                                                                                           ?less
                                                                                           :in)...>;

        static constexpr bool value = _impl<type>::value;
    };

    template<class TL,meta_index_t..._input_index>
    inline constexpr bool  in_range_v = in_range<TL,_input_index...>::value;
}

namespace meta
{
    template<class...>
    struct merge
            :type_identity<undefined_t>
    {};

    template<template<class...>class TypeList,class...Types>
    struct merge<TypeList<Types...>>
            :type_identity<TypeList<Types...>>
    {};

    template<template<class...>class TypeList,class...A,class...B>
    struct merge<TypeList<A...>,TypeList<B...>>
            :type_identity<TypeList<A...,B...>>
    {};

    template<template<class...>class TypeList,class...A,class...B,class...C>
    struct merge<TypeList<A...>,TypeList<B...>,TypeList<C...>>
            :type_identity<TypeList<A...,B...,C...>>
    {};

    template<template<class...>class TypeList,class...A,class...B,class...C,class ... D>
    struct merge<TypeList<A...>,TypeList<B...>,TypeList<C...>,TypeList<D...>>
            :type_identity<TypeList<A...,B...,C...,D...>>
    {};

    template<template<class DataType,DataType...>class Sequence,class DataType,DataType..._value>
    struct merge<Sequence<DataType,_value...>>
            :type_identity<Sequence<DataType,_value...>>
    {};

    template<template<class DataType,DataType...>class Sequence,class DataType,
            DataType..._value,DataType..._b>
    struct merge<Sequence<DataType,_value...>,Sequence<DataType,_b...>>
            :type_identity<Sequence<DataType,_value...,_b...>>
    {};

    template<template<class DataType,DataType...>class Sequence,class DataType,
            DataType..._value,DataType..._b,DataType..._c>
    struct merge<Sequence<DataType,_value...>,Sequence<DataType,_b...>,Sequence<DataType,_c...>>
            :type_identity<Sequence<DataType,_value...,_b...,_c...>>
    {};

    template<template<class DataType,DataType...>class Sequence,class DataType,
            DataType..._value,DataType..._b,DataType..._c,DataType..._d>
    struct merge<Sequence<DataType,_value...>,Sequence<DataType,_b...>,Sequence<DataType,_c...>,Sequence<DataType,_d...>>
            :type_identity<Sequence<DataType,_value...,_b...,_c...,_d...>>
    {};

    template<class A,class B,class C,class D,class E,class...R>
    struct merge<A,B,C,D,E,R...>
            :merge<typename merge<A,B,C,D>::type,typename merge<E,R...>::type>
    {};

    template<class...Templates>
    using merge_t =typename merge<Templates...>::type;
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

        template<size_t ..._index>
        struct _impl<std::index_sequence<_index...>>
                :type_identity<meta_list<std::pair<std::integral_constant<size_t ,_index>,std::integral_constant<DataType,_value>>...>>
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
    template<class T,class,class Error = undefined_t ,class = void>
    struct get
            :type_identity<Error>
    {};

    template<template<class...>class TypeList,class F,class...Rest,class Error>
    struct get<TypeList<F,Rest...>,meta_index_constant<0>,Error>
    {using type = F;};

    template<template<class...>class TypeList,class F,class B,class...Rest,class Error>
    struct get<TypeList<F,B,Rest...>,meta_index_constant<1>,Error>
    {using type = B;};

    template<template<class...>class TypeList,class F,class B,class C,class...Rest,class  Error>
    struct get<TypeList<F,B,C,Rest...>,meta_index_constant<2>,Error>
    {using type = C;};

    template<template<class...>class TypeList,class F,class B,class C,class D,class...Rest,class Error>
    struct get<TypeList<F,B,C,D,Rest...>,meta_index_constant<3>,Error>
    {using type = D;};

    template<template<class...>class TypeList,meta_index_t _index,class F,class B,class C,class D,class...Rest,class Error>
    struct get<TypeList<F,B,C,D,Rest...>,meta_index_constant<_index>,Error,std::enable_if_t<(_index>3)>>
            :get<TypeList<D,Rest...>,meta_index_constant<_index-3>,Error>
    {};

    template<meta_index_t  _index,template<class DataType,DataType...>class Sequence,class DataType,DataType..._value,class Error>
    struct get<Sequence<DataType,_value...>,meta_index_constant<_index>,Error>
            :get<meta_list<std::integral_constant<DataType,_value>...>,meta_index_constant<_index>,Error>
    {};

    template<class TypeList , meta_index_t _index,class Error = undefined_t>
    using get_t = typename get<TypeList,meta_index_constant<_final_index_v<_index,TypeList>>,Error>::type;

    template<class Sequence, meta_index_t _index,class Error = error_constant>
    inline constexpr  auto get_v = get_t<Sequence,_index,Error>::value;

    static_assert(std::is_same_v<get_t<meta_list<int,double,char>,100,wchar_t>,wchar_t>);

    static_assert(std::is_same_v<get_t<std::index_sequence<1,2,3,4>,-1>,std::integral_constant<size_t,4>>);
    static_assert(std::is_same_v<get_t<std::index_sequence<1,2,3,4>,2>,std::integral_constant<size_t,3>>);
    static_assert(std::is_same_v<get_t<std::index_sequence<1,2,3,4>,3>,std::integral_constant<size_t,4>>);
    static_assert(std::is_same_v<get_t<std::index_sequence<1,2,3,4>,4,error_constant>,error_constant>);

    static_assert(get_v<std::index_sequence<1,2,3,4,5>,11,error_constant> == error_constant::value);
    static_assert(get_v<std::index_sequence<1,2,3,4,5>,1> == 2);

    static_assert(std::is_same_v<get_t<std::index_sequence<1,2,3,4,5>,3,error_constant>,std::integral_constant<size_t,4>>);
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
        using _pair_l = get_t<_list,_left_index>;
        using _pair_r = get_t<_list,_right_index>;

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

    static_assert(static_cast<meta_index_t>(meta_constant::error) == 0);

    static_assert(std::is_same_v<swap_t<meta_list<>,1,2>,meta_list<>>);
}

namespace meta
{
    template<class T,template<class...>class Pred,class Add>
    class  modify
    {
    private:
        using list =  make_index_list_t<T>;

        template<class>
        struct _impl{};

        template<template<class...>class Template,size_t..._index,class...Items>
        struct _impl<Template<std::pair<std::integral_constant<size_t,_index>,Items>...>>
                :merge<std::conditional_t<Pred<std::pair<std::integral_constant<size_t,_index>,Items>>::value,
                Template<>,
                Template<>>...>
        {};

    };
}

namespace meta::sort
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

namespace meta::sort
{
    template<class T>
    struct quick_sort
    {
        template<class>struct _impl;

        using type = typename _impl<T>::type;
    };
}

namespace meta::algorithm
{
    template<class T,meta_index_t  _root,meta_index_t  _end,
            bool =  _root*2+1<=_end,bool = _root*2+2<=_end>
    struct _three_max
    {
        static constexpr meta_index_t  value =  _root;
    };

    template<class T,meta_index_t _root,meta_index_t  _end>
    struct _three_max<T,_root,_end,true,false>
    {
        static constexpr auto _left_child_value  =   get_v<T,2*_root+1>;
        static constexpr auto _root_value = get_v<T,_root>;
        static constexpr meta_index_t  value =  _left_child_value>_root_value?2*_root+1:_root;
    };

    template<class T,meta_index_t _root,meta_index_t  _end>
    struct _three_max<T,_root,_end,true,true>
    {
        static constexpr meta_index_t _left_child = 2*_root+1;
        static constexpr auto _left_child_value  =   get_v<T,_left_child>;
        static constexpr auto _right_child_value  =   get_v<T,_left_child+1>;
        static constexpr auto _root_value = get_v<T,_root>;
        static constexpr auto value = _left_child_value>_right_child_value?
                                             (_left_child_value>_root_value?_left_child:_root)//
                                                                                 :(_right_child_value>_root_value?_left_child+1:_root);
    };

    template<class T,T..._value>
    inline constexpr auto integer_sequence = std::integer_sequence<T,_value...>{};
    template<meta_index_t Idx,class T,T..._value>
    inline constexpr auto get_n_v(std::integer_sequence<T,_value...>)
    {
        std::array array{_value...};
        return array[Idx];
    }


    template<meta_index_t _left,meta_index_t _right,class T,T..._value,meta_index_t..._idx>
    inline constexpr auto swap( std::integer_sequence<meta_index_t ,_idx...>,
            std::integer_sequence<T,_value...>,
            std::integral_constant<meta_index_t,_left>,
            std::integral_constant<meta_index_t ,_right>)
    {
        constexpr T _left_value = get_n_v<_left>(integer_sequence<T,_value...>);
        constexpr T _right_value = get_n_v<_right>(integer_sequence<T,_value...>);
        return integer_sequence<T,_idx==_left?_right_value:_idx==_right?_left_value:_value...>;
    }


    static_assert(get_n_v<6>(integer_sequence<int,1,4,5,6,7,8,9,9,1,3>) ==9);
    static_assert(
        std::is_same_v<decltype(swap(integer_sequence<meta_index_t ,0,1,2,3,4,5,6,7,8>,integer_sequence<int,1,2,3,4,5,6,7,8,9>,std::integral_constant<meta_index_t ,2>{},
                                     std::integral_constant<meta_index_t ,3>{})),std::integer_sequence<int,1,2,4,3,5,6,7,8,9>>
    );

    template<class T,template<class,class>class Pred,meta_index_t  _root,meta_index_t  _end,
            bool = _root*2+1<=_end,bool = _root*2+2<=_end>
    struct _heap_adjust_helper
            :std::integral_constant<meta_index_t ,_root>
    {};

    template<class T,template<class,class>class Pred,meta_index_t  _root,meta_index_t  _end>
    struct _heap_adjust_helper<T,Pred,_root,_end,true,false>
    {
        static constexpr auto _left_child_value  =   get_v<T,2*_root+1>;
        static constexpr auto _root_value = get_v<T,_root>;
        static constexpr meta_index_t  value =  _left_child_value>_root_value?2*_root+1:_root;
    };

    template<class T,template<class,class>class Pred,meta_index_t  _root,meta_index_t  _end>
    struct _heap_adjust_helper<T,Pred,_root,_end,true,true>
    {
        static constexpr meta_index_t _left_child = 2*_root+1;
        static constexpr auto _left_child_value  =   get_v<T,_left_child>;
        static constexpr auto _right_child_value  =   get_v<T,_left_child+1>;
        static constexpr auto _root_value = get_v<T,_root>;
        static constexpr auto value = _left_child_value>_right_child_value?
                                      (_left_child_value>_root_value?_left_child:_root)//
                                                                          :(_right_child_value>_root_value?_left_child+1:_root);
    };

    template<class T,auto _current,auto _end,auto _max = _three_max<T,_current,_end>::value>
    struct heap_adjust
            :std::conditional_t<_max!=_current,
                    heap_adjust<swap_t<T,_max,_current>,_max,_end>,
             type_identity<T>>
    {};

    template<class  T,meta_index_t root,meta_index_t end>
    using  heap_adjust_t = typename heap_adjust<T,root,end>::type;

    template<class T,meta_index_t _index>
    struct make_heap
            :std::conditional_t<(_index>=0),make_heap<heap_adjust_t<T,_index, meta_index_t (extent_v<T>) - 1>, _index - 1>,type_identity<T>>
    {};

    template<class T>
    using make_heap_t =typename make_heap<T, meta_index_t(extent_v<T>) / 2 - 1>::type;

    template<class T>
    struct heap_sort
    {
        using _type = make_heap_t<T>;

        template<class U,meta_index_t _index>
        struct _impl
                :_impl<heap_adjust_t <swap_t<U,0,_index>,0,_index-1>,_index-1>
        {};

        template<class U>
        struct _impl<U,0>
                :type_identity<U>
        {};

        using type =typename _impl<_type,meta_index_t (extent_v<T>) - 1>::type;
    };

    template<class T>
    using heap_sort_t = typename heap_sort<T>::type;

    static_assert(std::is_same_v<make_heap_t<std::index_sequence<>>,std::index_sequence<>>);
    static_assert(std::is_same_v<make_heap_t<std::index_sequence<24,44, 18 ,52 ,92 ,74 ,44 ,15 ,57 ,17>>,
    std::index_sequence<92, 57 ,74, 52 ,44 ,18, 44, 15 ,24 ,17>>);

    static_assert(std::is_same_v<heap_sort_t<std::index_sequence<24,44, 18 ,52 ,92 ,74 ,44 ,15 ,57 ,17>>,
    std::index_sequence<15, 17, 18 ,24 ,44, 44 ,52 ,57 ,74, 92>>);
    static_assert(get_v<std::index_sequence<1,4,5,8,7,9,10>,6> == 10);

    static_assert(std::is_same_v<heap_adjust_t<std::index_sequence<1,4,5,8,7,9,10>,2,6>,std::index_sequence<1,4,10,8,7,9,5>>);
}

namespace meta::algorithm
{
    template<class>inline constexpr bool _is_meta_sequence_v = false;

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

    template<class FirstNode,class ...RestNodes,class F,class S,size_t _new_id>
    struct _huffman_find<meta_list<FirstNode,RestNodes...>,F,S,_new_id>
            :std::conditional_t<(FirstNode::value<F::value&&FirstNode::parent == -1),
            _huffman_find<meta_list<RestNodes...>,_huffman_tree_node<FirstNode::value,FirstNode::id,_new_id,0>,_huffman_tree_node<F::value,F::id,_new_id,1>,_new_id>,
            std::conditional_t<(S::value>FirstNode::value)&&FirstNode::parent==-1,
            _huffman_find<meta_list<RestNodes...>,F,_huffman_tree_node<FirstNode::value,FirstNode::id,_new_id,1>,_new_id>,
            _huffman_find<meta_list<RestNodes...>,F,S,_new_id>
            >//conditional
            >
    {};

    template<class,class...>struct append;

    template<template<class...>class TypeList,class...Items,class...Add>
    struct append<TypeList<Items...>,Add...>
            :type_identity<TypeList<Items...,Add...>>
    {};

    template<class Type,class Input/*TypeList*/,size_t _new_id>
    struct _huffman_insert
    {
    private:
        static constexpr auto _max_value = std::numeric_limits<Type >::max();

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

    template<class Type,class TypeList,size_t _new_id>
    using _huffman_insert_t =typename _huffman_insert<Type,TypeList,_new_id>::type;

    template<class Sequence>
    struct make_huffman_tree
    {
    private:
        static_assert(_is_meta_sequence_v<Sequence>,"requires meta sequence!");

        using _value_type = traits_value_t<Sequence>;
        template<class,class>struct _make_node_list{};

        template<template<class DataType,DataType...>class  S,class DataType,DataType..._value,size_t..._id>
        struct _make_node_list<S<DataType,_value...>,std::index_sequence<_id...>>
                :type_identity<meta_list<_huffman_tree_node<_value,_id,-1,-1>...>>
        {};

        using _make_node_list_t =typename _make_node_list<Sequence,std::make_index_sequence<extent_v<Sequence>>>::type;

        template<class T,size_t _new_id,size_t _end>
        struct _impl
        {};

        template<class...Nodes,size_t _new_id,size_t _end>
        struct _impl<meta_list<Nodes...>,_new_id,_end>
        :std::conditional_t<(_new_id<=_end),
                _impl<_huffman_insert_t<_value_type,meta_list<Nodes...>,_new_id>,_new_id+1,_end>,
                type_identity<meta_list<Nodes...>>>
        {};

    public:

        using type= typename  _impl<_make_node_list_t ,extent_v<Sequence>,2*extent_v<Sequence>-2>::type ;
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
}


//5,6,2,9,8,7
#endif//only cxx17
#undef STL_LANG
#undef STL_17
#undef STL_20
#endif //META_META_H
