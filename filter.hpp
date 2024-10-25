#ifndef META_FILTER_HPP
#define META_FILTER_HPP

#include"merge.hpp"

namespace common::filter_details
{
  template<class Constant>
  struct _advance
  {};
  
  template<class Type,Type v>
  struct _advance<constant<Type,v>>
    :type_identity<constant<Type,v+1>>
  {};
  
  template<class C>
  using _advance_t=typename _advance<C>::type;
  
  template<int model,class,template<class...>class,class...>
  struct _found_base
    :bool_constant<false>
  {};
  
  //判断是否出现
  template<template<class...>class List,template<class...>class Pred,
  class Head,class...Rest,class...Parameters>
  struct _found_base<1,List<Head,Rest...>,Pred,Parameters...>
    :bool_constant<(Pred<Head,Parameters...>::value||...||Pred<Rest,Parameters>::value)>
  {};
  
  //计算出现次数
  template<template<class...>class List,template<class...>class Pred,
    class Head,class...Rest,class...Parameters>
  struct _found_base<2,List<Head,Rest...>,Pred,Parameters...>
    :index_constant<(Pred<Head,Parameters...>::value+...+Pred<Rest,Parameters>::value)>
  {};
  
  //出现的位置
  template<template<class...>class List,template<class...>class Pred,
    class Head,class...Rest,class...Parameters,class Sequence,class Init>
  struct _found_base<3,List<Head,Rest...>,Pred,Sequence,Init,Parameters...>
    :std::conditional_t<Pred<Head,Parameters...>::value,
    _found_base<3,List<Rest...>,Pred,merge_t<Sequence,List<Init>>,_advance_t<Init>,Parameters...>,
      _found_base<3,List<Rest...>,Pred,Sequence,_advance_t<Init>,Parameters...>
    >
  {};
  
  template<template<class...>class List,template<class...>class Pred,
    class...Parameters,class Sequence,class Init>
  struct _found_base<3,List<>,Pred,Sequence,Init,Parameters...>
    :type_identity<Sequence>
  {};
  
  //过滤掉满足Pred的元素
  template<template<class...>class List,template<class...>class Pred,
    class Head,class...Rest,class Other,class Out>
  struct _found_base<4,List<Head,Rest...>,Pred,Other,Out>
    :std::conditional_t<Pred<Other,Head>::value,
      _found_base<4,List<Rest...>,Pred,Other,Out>,
      _found_base<4,List<Rest...>,Pred,Other,merge_t<Out,List<Head>>>
      >//conditional_t
  {};
  
  template<template<class...>class List,template<class...>class Pred,
    class Other,class Out>
  struct _found_base<4,List<>,Pred,Other,Out>
    :type_identity<Out>
  {};
  
  //第一次出现的位置
  template<template<class...>class List,template<class...>class Pred,class Init,
    class Head,class...Rest,class Type>
  struct _found_base<5,List<Head,Rest...>,Pred,Type,Init>
    :std::conditional_t<Pred<Head,Type>::value,Init,_found_base<5,List<Rest...>,Pred,Type,_advance_t<Init>>>
  {};
  
  //insert_if
  template<template<class...>class List,
    template<class...>class Pred,
    class Head,class...Rest,
    class...Para,
    class...Increments>
  struct _found_base<6,List<Head,Rest...>,Pred,List<Increments...>,Para...>
    :std::conditional_t<Pred<Head,Para...>::value,
      merge<List<Increments...,Head>,
      typename _found_base<6,List<Rest...>,Pred,List<Increments...>,Para...>::type>,
      merge<List<Head>,
      typename _found_base<6,List<Rest...>,Pred,List<Increments...>,Para...>::type>
    >
  {};
  
  template<template<class...>class List,
    template<class...>class Pred,
    class...Para,
    class...Increments>
  struct _found_base<6,List<>,Pred,List<Increments...>,Para...>
    :type_identity<List<>>
  {};
  
  
}

namespace common::filter_details
{
  //insert_if
  template<class Template,template<class>class Predicate,class...Increments>
  struct _insert_if
  {};
  
  template<template<class...>class List,
    template<class>class Pred,class...Add,class...Types>
  struct _insert_if<List<Types...>,Pred,Add...>
    :_found_base<6,List<Types...>,Pred,List<Add...>>
  {};
  
  template<class Template,template<class>class Predicate,class...Increments>
  using _insert_if_t=copy_cvref_t<Template,typename _insert_if<remove_cvref_t<Template>,Predicate,Increments...>::type>;
  
  //移除相同的元素
  template<class Template,class T>
  using _remove_t=typename _found_base<4,Template,is_same,T,make_empty_t<Template>>::type;
  
  template<class Template,ptrdiff_t Idx>
  struct _erase
  {
  private:
    
    static constexpr auto S_index_v=_conversion_v<Idx,Template>;
    
    template<class ,class Type,ptrdiff_t I>
    struct _index:constant<ptrdiff_t,-1>
    {};
    
    template<template<class...>class List,class Type,ptrdiff_t I,class Head,class...Rest>
    struct _index<List<Head,Rest...>,Type,I>
      :std::conditional_t<is_same_v<Head,Type>,
        constant<ptrdiff_t,I>,
        _index<List<Rest...>,Type,I+1>>
    {};
    
    template<class A,class B>
    using _same=bool_constant<_index<A,B,0>::value==S_index_v>;
  public:
    using type=typename
    _found_base<4,Template,_same,Template,make_empty_t<Template>>::type;
  };
  
  template<class Template,ptrdiff_t Idx>
  using  _erase_t=typename _erase<Template,Idx>::type;
  
  template<class Template,class Type>
  using _equal_range_t=typename _found_base<3,Template,is_same,make_empty_t<Template>,index_constant<0>,Type>::type;
  
  template<class Template,template<class,class...>class Predicate,class...Parameters>
  INLINE constexpr size_t _count_if_v=_found_base<2,Template,Predicate,Parameters...>::value;
  
  template<class Template,class T>
  inline constexpr size_t _count_v=_found_base<2,Template,is_same,T>::value;
}

namespace common
{
  template<class Template,template<class,class...>class Predicate,class...Parameters>
  struct count_if
    :index_constant<filter_details::_count_if_v<remove_cvref_t<Template>,Predicate,Parameters...>>
  {};
  
  template<class Template,class T>
  struct count
    :count_if<remove_cvref_t<Template>,is_same,T>
  {};
  
  template<class Template,class T>
  using remove_t=copy_cvref_t<Template,filter_details::_remove_t<remove_cvref_t<Template>,T>>;
  
  template<class Template,class T>
  using remove=type_identity<remove_t<Template,T>>;
  
  template<class Template,ptrdiff_t Idx>
  using erase=copy_cvref_t<Template,filter_details::_erase<Template,Idx>>;
  
  template<class Template,ptrdiff_t Idx>
  using erase_t=typename erase<Template,Idx>::type;
  
  template<class Template,class Type>
  struct _equal_range
  {};
  
  template<template<class ...>class Template,class...Types,class Type>
  struct _equal_range<Template<Types...>,Type>
  {
  private:
    template<class>
    struct _merge
      :type_identity<std::index_sequence<>>
    {};
    
    template<template<class...>class List,size_t...rest>
    struct _merge<List<index_constant<rest>...>>
      :type_identity<std::index_sequence<rest...>>
    {};
    
    using _type=filter_details::_equal_range_t<Template<Types...>,Type>;
    
  public:
    using type=typename _merge<_type>::type;
  };
  
  template<class Template,class Type>
  struct equal_range
    :copy_cvref<Template,typename _equal_range<remove_cvref_t<Template>,Type>::type>
  {};
  
  template<class Template,class Type>
  using equal_range_t=typename equal_range<Template,Type>::type;
  
  template<class Template,template<class>class Predicate,class...Increments>
  using insert_if_t=copy_cvref_t<Template,typename filter_details::_insert_if<remove_cvref_t<Template>,Predicate,Increments...>::type>;
}
#endif //META_FILTER_HPP
