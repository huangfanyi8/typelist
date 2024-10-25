#ifndef META_FILTER_HPP
#define META_FILTER_HPP

#include"merge.hpp"

namespace common::filter_details
{
  template<int model,class,template<class...>class,class...>
  struct _found_base
    :bool_constant<false>
  {};
  
  template<template<class...>class List,template<class...>class Pred,
  class Head,class...Rest,class...Parameters>
  struct _found_base<1,List<Head,Rest...>,Pred,Parameters...>
    :bool_constant<(Pred<Head,Parameters...>::value||...||Pred<Rest,Parameters>::value)>
  {};
  
  template<class Template,template<class,class...>class Predicate,class...Parameters>
  class _filter_base
  {
  private:
    static constexpr auto S_extent_v=extent_v<Template>;
    
    template<size_t Index,class,class List,class Out,class Sequence>
    struct Impl
    {
      static constexpr size_t S_size=extent_v<Out>;
      using rest=List;
      using out=Out;
      using result_sequence=Sequence;
    };
    
    template<size_t Index,
      template<class...>class List, class Out,class Sequence,class Head,class...Rest>
    struct Impl<Index,std::enable_if_t<(Index<S_extent_v)>,List<Head,Rest...>,Out,Sequence>
      :std::conditional_t
      <Predicate<Head,Parameters...>::value,
      Impl<Index+1,void,List<Rest...>,merge_t<Out,List<Head>>,merge_t<Sequence,constant<size_t,Index>>>,
      Impl<Index+1,void,List<Rest...,Head>,Out,Sequence>
      >
    {};
  
  public:
    using rest=typename Impl<0,void,Template,make_empty_t<Template>,normal_sequence<>>::rest;
    using out=typename Impl<0,void,Template,make_empty_t<Template>,normal_sequence<>>::out;
    using result_sequence=typename Impl<0,void,Template,make_empty_t<Template>,normal_sequence<>>::result_sequence;
    static constexpr auto count=Impl<0,void,Template,make_empty_t<Template>,normal_sequence<>>::S_size;
  };
}
#endif //META_FILTER_HPP
