/*
 *Insert  Algorithm
 * MetaFunction insert<Template<Types...>,Idx,Add...>
 *
 * if (Idx==0)  return  TypeList<Types...,Add...>
 * if(Idx==1)  input TypeList<Head,Rest...> --> return  TypeList<Head,Add...Types...>
 *  if(Idx==2)  input TypeList<Head,Sec,Rest...> --> return  TypeList<Head,Sec,Add...Types...>
 */

#ifndef META_INSERT_HPP
#define META_INSERT_HPP

#include"merge.hpp"

namespace common::insert_detail
{
  template<class Template/*Type List*/,
    ptrdiff_t/*insert pos*/,
    class,/*insertion data*/
    class=void/*SFINAE*/>
  struct _insert_helper
  {};
  
  template<template<class...>class Template,class...Types,
    template<class...>class AddTemplate,class...Increment>
  struct _insert_helper<Template<Types...>,0,AddTemplate<Increment...>>
    :type_identity<Template<Increment...,Types...>>
  {};
  
  template<ptrdiff_t Idx,
  template<class...>class Template,
    template<class...>class AddTemplate,
    class...Increment>
  struct _insert_helper<Template<>,Idx,AddTemplate<Increment...>,std::enable_if_t<(Idx>0)>>
    :type_identity<Template<Increment...>>
  {};
  
  template<ptrdiff_t Idx,
    template<class...>class Template,
    template<class...>class AddTemplate,
    class Head,class...Rest,
    class...Increment>
  struct _insert_helper<Template<Head,Rest...>,Idx,AddTemplate<Increment...>,std::enable_if_t<(Idx>=1)>>
    :merge<Template<Head>,typename _insert_helper<Template<Rest...>,Idx-1,AddTemplate<Increment...>>::type>
  {};
  
  template<template<class...>class Template,class...Types,
    ptrdiff_t Idx,
    template<class...>class AddTemplate,class...Increments>
  struct _insert_helper<Template<Types...>,
    Idx,AddTemplate<Increments...>,std::enable_if_t<(Idx<0)>>
    :std::conditional_t<(Idx>=-ptrdiff_t(extent_v<Template<Types...>>)),
      _insert_helper<Template<Types...>,_conversion_v<Idx,Template<Types...>>,AddTemplate<Increments...>>,
      type_identity<Template<Increments...,Types...>>>
  {};
}

namespace common
{
  template<class Template,ptrdiff_t Position,class...Increment>
  using insert_t
    =copy_cvref_t<Template,typename insert_detail::_insert_helper<remove_cvref_t<Template>,Position,_aux<Increment...>>::type>;

  template<class Template,class...Increment>
  using prepend_t=insert_t<Template,0,Increment...>;

  template<class Template,class...Increment>
  using append_t=insert_t<Template,extent_v<Template>,Increment...>;
}

namespace common
{
  template<class Template,ptrdiff_t Position,auto...Add>
  struct insert_c
  {
    static constexpr auto S_position=_conversion_v<Position,Template>;
    static constexpr auto S_out_range= _is_out_of_range<Template,Position>;
  public:
      using value_type=traits_value_t<Template>;
      static_assert((is_same_v<value_type,decltype(Add)>&&...&&true));
  private:
      template<ptrdiff_t Idx,class List,class Empty,bool>
      struct Impl
      {};

      template<ptrdiff_t Idx,template<class V,V...>class List,class V,class Empty,V Head,V...Rest>
      struct Impl<Idx,List<V,Head,Rest...>,Empty,false>
              :std::conditional_t<Idx!=S_position,
                      Impl<Idx+1,List<V,Rest...>,merge_t<Empty,List<V,Head>>,false>,
                      merge<Empty,List<V,Add...>,List<V,Head,Rest...>>
              >
      {};
    
      template<ptrdiff_t Idx,template<class V,V...>class List,class V,class Empty,V...Value>
      struct Impl<Idx,List<V,Value...>,Empty,true>
        :std::conditional_t<(Position>0),
            type_identity<List<V,Value...,Add...>>,
          type_identity<List<V,Add...,Value...>>>
      {};
    
      template<ptrdiff_t Idx,template<class V,V...>class List,class V,class Empty>
      struct Impl<Idx,List<V>,Empty,false>
              :merge<Empty,List<V,Add...>>
      {};

  public:
      using type=
              typename Impl<0,Template,make_empty_t<Template>,S_out_range>::type;
  };

  template<class Template,ptrdiff_t Position,traits_value_t<Template>...Add>
  using insert_c_t=copy_cvref_t<Template,typename insert_c<remove_cvref_t<Template>,Position,Add...>::type>;

  template<class Template,traits_value_t<Template> ...Add>
  using append_c_t=typename insert_c<Template,extent_v<Template>,Add...>::type;

  template<class Template,traits_value_t<Template>...Add>
  using prepend_c_t=typename insert_c<Template,0,Add...>::type;
}

namespace common::insert_detail
{
  template<class Template,/*Type List*/
    template<class,class...>class Predicate,/*type traits*/
      class Add,/*insert items*/
        class...Para>
  struct _insert_if_helper
    :type_identity<Template>
  {};
  
  template<template<class...>class Template,
    template<class,class...>class Predicate,/*type traits*/
    template<class...>class Add,/*insert items*/
    class...Increment,
    class Head,class...Rest,
      class...Para>
  struct _insert_if_helper<Template<Head,Rest...>,Predicate,Add<Increment...>,Para...>
    :std::conditional_t<Predicate<Head,Para...>::value,
      merge<Template<Increment...,Head>,
      typename _insert_if_helper<Template<Rest...>,Predicate,Add<Increment...>,Para...>::type>,
      merge<Template<Head>,
      typename _insert_if_helper<Template<Rest...>,Predicate,Add<Increment...>,Para...>::type>
    >
  {};
  
  template<template<class Value,Value...>class Template,
    template<class,class...>class Predicate,/*type traits*/
    template<class Value,Value...>class Add,
    class Value,
    Value Head,Value...Rest,
    Value...Increments>
  struct _insert_if_helper<Template<Value,Head,Rest...>,Predicate,Add<Value,Increments...>>
    :std::conditional_t<Predicate<constant<Value,Head>>::value,
      merge<Template<Value,Increments...,Head>,
      typename _insert_if_helper<Template<Value,Rest...>,Predicate,Add<Value,Increments...>>::type>,
      merge<Template<Value,Head>,
      typename _insert_if_helper<Template<Value,Rest...>,Predicate,Add<Value,Increments...>>::type>
    >
  {};
}

namespace common
{
  template<class Template,template<class>class Predicate,class...Increments>
  struct insert_if
    :insert_detail::_insert_if_helper<Template,Predicate,_aux<Increments...>>
  {};
  
  template<class Template,template<class>class Predicate,class...Increments>
  using insert_if_t=copy_cvref_t<Template,typename insert_if<remove_cvref_t<Template>,Predicate,Increments...>::type>;
  
  template<class Template,template<auto>class Predicate,auto...Increments>
  struct insert_if_c
  {};
  
  template<template<class V,V...>class Template,class Value,Value...v,
  Value...Add,template<auto>class Predicate>
  struct insert_if_c<Template<Value,v...>,Predicate,Add...>
  {
  private:
    template<class T>
    using P=Predicate<T::value>;
    
    using _type=insert_detail::_insert_if_helper<Template<Value,v...>,P,Template<Value,Add...>>;
    
  public:
    using type=typename _type::type;
  };
  
  template<class Template,template<auto>class Predicate,auto...Increments>
  using insert_if_c_t=copy_cvref_t<Template,typename insert_if_c<remove_cvref_t<Template>,Predicate,Increments...>::type>;
}

#endif //META_INSERT_HPP
