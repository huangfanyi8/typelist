/*
 *insert  algorithm
 *
 */

#ifndef META_INSERT_HPP
#define META_INSERT_HPP

#include"merge.hpp"

namespace meta_base
{
  template<class Template,ptrdiff_t Position,class,class...Increment>
  struct Insert_base
  {};

  template<template<class...>class Template,class...Types,class...Increment>
  struct Insert_base<Template<Types...>,0,void,Increment...>
          :type_identity<Template<Increment...,Types...>>
  {};

  template<ptrdiff_t Idx,template<class...>class Template,class...Increment>
  struct Insert_base<Template<>,Idx,std::enable_if_t<(Idx>0)>,Increment...>
          :type_identity<Template<Increment...>>
  {};
  
  template<ptrdiff_t Idx,
          template<class...>class Template,
          class Head,class...Rest,
          class...Increment>
  struct Insert_base<Template<Head,Rest...>,Idx,std::enable_if_t<(Idx>=1)>,Increment...>
          :merge<Template<Head>,typename Insert_base<Template<Rest...>,Idx-1,void,Increment...>::type>
  {};
  
  template<template<class...>class Template,class...Types,ptrdiff_t Idx,class...Increments>
  struct Insert_base<Template<Types...>,Idx,std::enable_if_t<(Idx<0)>,Increments...>
    :binary_t<(Idx>=-ptrdiff_t(extent_v<Template<Types...>>)),
    Insert_base<Template<Types...>,index_conversion<Idx,Template<Types...>>,void,Increments...>,
    type_identity<Template<Increments...,Types...>>>
  {};
  
  template<class Template,ptrdiff_t Position,class...Increment>
  using insert_t=typename Insert_base<Template,Position,void,Increment...>::type;

  template<class Template,class...Increment>
  using prepend_t=insert_t<Template,0,Increment...>;

  template<class Template,class...Increment>
  using append_t=insert_t<Template,extent_v<Template>,Increment...>;
}

namespace meta_base
{
  template<class Template,ptrdiff_t Position,traits_value_t<Template>...Add>
  struct insert_constant
  {
    static constexpr auto S_position=index_conversion<Position,Template>;
    static constexpr auto S_out_of_range=Is_out_of_range_v<Template,Position>;
  public:
      using value_type=traits_value_t<Template>;
  private:
      template<ptrdiff_t Idx,class List,class Empty,bool>
      struct Impl
      {};

      template<ptrdiff_t Idx,template<class V,V...>class List,class V,class Empty,V Head,V...Rest>
      struct Impl<Idx,List<V,Head,Rest...>,Empty,false>
              :binary_t<Idx!=S_position,
                      Impl<Idx+1,List<V,Rest...>,merge_t<Empty,List<V,Head>>,false>,
                      merge<Empty,List<V,Add...>,List<V,Head,Rest...>>
              >
      {};
    
      template<ptrdiff_t Idx,template<class V,V...>class List,class V,class Empty,V...Value>
      struct Impl<Idx,List<V,Value...>,Empty,true>
        :binary_t<(Position>0),
            type_identity<List<V,Value...,Add...>>,
          type_identity<List<V,Add...,Value...>>>
      {};
    
      template<ptrdiff_t Idx,template<class V,V...>class List,class V,class Empty>
      struct Impl<Idx,List<V>,Empty,false>
              :merge<Empty,List<V,Add...>>
      {};

  public:
      using type=
              typename Impl<0,Template,empty_t<Template>,S_out_of_range>::type;
  };

  template<class Template,ptrdiff_t Position,traits_value_t<Template>...Add>
  using insert_constant_t=typename insert_constant<Template,Position,Add...>::type;

  template<class Template,traits_value_t<Template> ...Add>
  using append_constant_t=typename insert_constant<Template,extent_v<Template>,Add...>::type;

  template<class Template,traits_value_t<Template>...Add>
  using prepend_constant_t=typename insert_constant<Template,0,Add...>::type;
}

namespace meta_base
{
  //Insert element before Predicate<Type,Parameters...>::value
  template<class Template, template<class>class  Predicate,class...Increments>
  struct insert_if
          :type_identity<Template>
  {};

  template<template<class...>class Template,
          template<class,class...>class  Predicate,
          class...Increment,
          class Head,class...Rest>
  struct insert_if<Template<Head,Rest...>,Predicate,Increment...>
          :binary_t<Predicate<Head>::value,
          merge<Template<Increment...,Head>,typename insert_if<Template<Rest...>,Predicate,Increment...>::type>,
          merge<Template<Head>,typename insert_if<Template<Rest...>,Predicate,Increment...>::type>
          >
  {};

  template<class Template, template<class>class  Predicate,class...Increments>
  using insert_if_t=typename insert_if<Template,Predicate,Increments...>::type;
}

namespace meta_base
{
  template<class Template,template<auto...>class Predicate,traits_value_t<Template>...Increments>
  struct insert_constant_if
    :type_identity<Template>
  {};
  
  template<template<class Value,Value...>class Template,
    class Value,
    template<Value>class Predicate,
    Value Head,Value...Rest,
    Value...Increments>
  struct insert_constant_if<Template<Value,Head,Rest...>,Predicate,Increments...>
    :binary_t<Predicate<Head>::value,
    merge<Template<Value,Increments...,Head>,typename insert_constant_if<Template<Value,Rest...>,Predicate,Increments...>::type>,
    merge<Template<Value,Head>,typename insert_constant_if<Template<Value,Rest...>,Predicate,Increments...>::type>
  >
  {};
  
  template<class Template,template<auto...>class Predicate,traits_value_t<Template>...Increments>
  using insert_constant_if_t=typename insert_constant_if<Template,Predicate,Increments...>::type;
}


#endif //META_INSERT_HPP
