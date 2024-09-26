/*
 * 常见比较谓词
 * */

#ifndef META_COMPARE_HPP
#define META_COMPARE_HPP

#include"Meta_traits.hpp"

#include"predicate.hpp"

namespace meta_base
{
  template<class ,template<class,class>class Binary>
  struct choice
        :type_identity<undefined>
  {};
  
  template<template<class...>class Template,template<class,class>class Binary,class Head>
  struct choice<Template<Head>,Binary>
    :type_identity<Head>
  {};
  
  template<template<class...>class Template,template<class,class>class Binary,
        class Head,class Sec,class...Rest>
  struct choice<Template<Head,Sec,Rest...>,Binary>
    :binary_t<Binary<Head,Sec>::value,
            choice<Template<Head,Rest...>,Binary>,
            choice<Template<Sec,Rest...>,Binary>
    >
  {};
  
  template<class Template,template<class,class>class Binary>
  using get_t=typename choice<Template,Binary>::type;
  
  template<class Template>
  using max_t=typename choice<Template,greater>::type;
  
  template<class Template>
  using min_t=typename choice<Template,less>::type;
}

namespace meta_base
{

}
#endif //META_COMPARE_HPP
