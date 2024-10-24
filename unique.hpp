/*
 * unique 去重算法，
 * unique_t<T> :删除T中重复的元素,返回T
 * is_unique_v<T>，判断T中是否存在相同元素
 * */

#ifndef META_UNIQUE_HPP
#define META_UNIQUE_HPP

#include"merge.hpp"

namespace common::unique_details
{
  template<bool v,/*is_unique*/
    class Input,/*TypeList*/
    class Output,/*ouput template*/
    template<class,class>class Traits/*type traits*/
    >
  struct _unique_helper
    :bool_constant<v>,
    type_identity<Output>
  {};
  
  template<bool v,/*is_unique*/
    template<class...>class Input,/*TypeList*/
    class Output,/*ouput template*/
    template<class,class>class Traits/*type traits*/,
    class Head,class...Rest
  >
  struct _unique_helper<v,Input<Head,Rest...>,Output,Traits>
    :std::conditional_t<Traits<Output,Head>::value,
    _unique_helper<false,Input<Rest...>,Output,Traits>,
      _unique_helper<true,Input<Rest...>,merge_t<Output,Input<Head>>,Traits>>
  {};
  
  template<bool v,
      template<class Value,Value...>class Input,
    class Output,
    class ValueType,ValueType Head,ValueType...Rest,
    template<class,class>class Traits/*type traits*/>
  struct _unique_helper<v,Input<ValueType,Head,Rest...>,Output,Traits>
    :std::conditional_t<Traits<Output,constant<ValueType,Head>>::value,
      _unique_helper<false,Input<ValueType,Rest...>,Output,Traits>,
      _unique_helper<true,Input<ValueType,Rest...>,merge_t<Output,Input<ValueType,Head>>,Traits>
    >
  {};
  
  template<class Template,template<class,class>class Traits>
  struct _unique
    :_unique_helper<true,Template,make_empty_t<Template>,Traits>
  {};
}

namespace common
{
  template<class Input,class Type>
  struct _find_helper
    :bool_constant<false>
  {};
  
  #if NON_STL_17
  template<template<class Value,Value...>class Input,
    class ValueType,ValueType Head,ValueType...Rest,ValueType Aim>
  struct _find_helper<Input<ValueType,Head,Rest...>,constant<ValueType,Aim>>
    :bool_constant<((Aim==Head)||...||(Rest==Aim))>
  {};
  
  template<template<class...>class Input,/*TypeList*/
    class Type,
    class Head,class...Rest>
  struct _find_helper<Input<Head,Rest...>,Type>
    :bool_constant<(is_same_v<Type,Head>||...||is_same_v<Rest,Type>)>
  {};
  #else
  template<template<class Value,Value...>class Input,
    class ValueType,ValueType Head,ValueType...Rest,ValueType Aim>
  struct _find_helper<Input<ValueType,Head,Rest...>,constant<ValueType,Aim>>
    :std::conditional_t<Aim==Head,
    bool_constant<true>,
    _find_helper<Input<ValueType,Rest...>,constant<ValueType,Aim>>>
  {};
  
  template<template<class...>class Input,/*TypeList*/
    class Type,
    class Head,class...Rest>
    struct _find_helper<Input<Head,Rest...>,Type>
      :std::conditional_t<is_same_v<Type,Head>,
      bool_constant<true>,
      _find_helper<Input<Rest...>,Type>>
    {};
  #endif
  template<class Input,/*TypeList*/class Type>
  using _find=_find_helper<remove_cvref_t<Input>,Type>;
  
  template<class Input>
  using unique=unique_details::_unique<remove_cvref_t<Input>,_find>;
  
  template<class Input>
  using unique_t=typename unique<Input>::type;
  
  template<class Input>
  INLINE constexpr bool is_unique_v=unique<Input>::value;
}


#endif //META_UNIQUE_HPP
