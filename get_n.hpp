/*
 * 获取TypeList 或者变量模板中的第n个元素
 *  支持负数下标
 *  访问越界时支持自定义类型返回默认为common::undefined
 *
 *
 * */

#ifndef META_GET_N_HPP
#define META_GET_N_HPP

#include"reverse.hpp"

namespace common
{
  template<class T,/*TypeList*/
    std::ptrdiff_t Idx,/*index sub*/
    class ErrorType/*error type*/
  >
  #if NON_STL_20
  requires (is_template_v<T>)
  #endif
  class _get_n_base
  {
    static_assert(is_template_v<T>,"T must be a variadic template");
    using ptrdiff_t=std::ptrdiff_t;
    
    static constexpr auto S_idx
      =Idx>=0?Idx:ptrdiff_t(extent_v<T>)+Idx;
    
    template<ptrdiff_t c,class,class=void>
    struct Impl
      :type_identity<ErrorType>
    {};
    
    template<template<class...>class List,class Head,class...Rest>
    struct Impl<0,List<Head,Rest...>>
      :type_identity<Head>
    {};
    
    template<template<class Value,Value...>class List,
      class Value,
      Value Head,Value...Rest>
    struct Impl<0,List<Value,Head,Rest...>>
      :type_identity<constant<Value,Head>>
    {};
    
    template<ptrdiff_t c,template<class...>class List,
      class Head,class Second,class...Rest>
    #if NON_STL_20
    requires(c>0)
    struct Impl<c,List<Head,Second,Rest...>>
      #else
      struct Impl<c,List<Head,Second,Rest...>,std::enable_if_t<(c>0)>>
      #endif
      :Impl<c-1,List<Second,Rest...>>
    {};
    
    template<ptrdiff_t c,template<class Value,Value...>class List,
      class Value,
      Value Head,Value...Rest>
    #if NON_STL_20
    requires(c>0)
    struct Impl<c,List<Value,Head,Rest...>>
      #else
      struct Impl<c,List<Value,Head,Rest...>,std::enable_if_t<(c>0)>>
      #endif
      :Impl<c-1,List<Value,Rest...>>
    {};
  
  public:
    using type=typename Impl<S_idx,remove_cvref_t<T>>::type;
  };
  
  template<class T,/*TypeList*/
    std::ptrdiff_t Idx,/*index sub*/
    class ErrorType/*error type*/
  >
  class get_n
    :public _get_n_base<T,Idx,ErrorType>
  {};
  
  template<class T,/*TypeList*/
    std::ptrdiff_t Idx,/*index sub*/
    class ErrorType=undefined/*error type*/
  >
  using get_n_t=typename _get_n_base<T,Idx,ErrorType>::type;
  
  template<class T,/*TypeList*/
    std::ptrdiff_t Idx,/*index sub*/
    class ErrorType=error_constant, /*error type*/
    class=std::enable_if_t<!is_normal_v<T>>
  >
  INLINE constexpr auto get_n_v=_get_n_base<T,Idx,ErrorType>::type::value;
  
  template<class Template>
  using front_t = get_n_t<Template, 0>;
  
  template<class Template>
  using back_t = front_t<reverse_t<Template>>;
  
  template<class Template,class ErrorType=error_constant>
  INLINE constexpr auto front_v=get_n_v<Template,0>;
  
  template<class Template,class ErrorType=error_constant>
  INLINE constexpr auto back_v=front_v<reverse_t<Template>>;
}

#endif
