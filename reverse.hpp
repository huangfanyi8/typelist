
#ifndef REVERSE_HPP
#define REVERSE_HPP

#include"common.hpp"
#include"merge.hpp"

///reverse
namespace common
{
  template<class List,bool=is_normal_v<List>>
  class _reverse
  {
    static_assert(is_template_v<List>,"incorrect!");
  private:
    using P=remove_cvref_t<List>;
    
    template<class T,class E>
    struct Impl
      :type_identity<E>
    {};

    template<template<class...> class Template,class E,class Head,class...Rest>
    struct Impl<Template<Head,Rest...>,E>
      :Impl<Template<Rest...>,merge_t<Template<Head>,E>>
    {};
  private:
    using _type=typename Impl<P,make_empty_t<P>>::type;
  public:
    using type=copy_cvref_t<List,_type>;
  };

  template<class List>
  class _reverse<List,false>
  {
    static_assert(is_template_v<List>,"incorrect!");
  private:
    using P=remove_cvref_t<List>;
    
    template<class,class Empty>
    struct Impl
            :type_identity<Empty>
    {};

    template<template<class Value,Value...> class Template,class Empty,class Value,Value Head,Value...Rest>
    struct Impl<Template<Value,Head,Rest...>,Empty>
            :Impl<Template<Value,Rest...>,merge_t<Template<Value,Head>,Empty>>
    {};
  private:
    using _type=typename Impl<P,make_empty_t<P>>::type;
  public:
    using type=copy_cvref_t<List,_type>;
  };
  
  template<class List,class=std::enable_if_t<is_template_v<List>>>
  struct reverse
    :_reverse<List>
  {};

  template<class Template>
  using reverse_t=typename reverse<Template>::type;
}

#endif
