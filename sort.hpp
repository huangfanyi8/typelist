#ifndef META_SORT_HPP
#define META_SORT_HPP

#include"get_n.hpp"
#include"predicate.hpp"
#include"find.hpp"

namespace meta_base
{
  template<template<class,class>class Binary,class Left,class Right>
  struct Sort_compare_pred
  {
    //相等返回1
    //true返回1
    //false返回2
    constexpr static size_t value=!Binary<Left,Right>::value&&!Binary<Right,Left>::value
    ?1:Binary<Left,Right>::value?1:0;
  };
  
  template<class Template,template<class,class>class Binary>
  class Insertion_sort_helper
  {
  private:
    template<class Target,class Input,class Output>
    struct Insert
      :type_identity<Output>
    {};
    
    template<template<class...>class Input,class Output,class Target,class Head,class...Rest>
    struct Insert<Target,Input<Head,Rest...>,Output>
      :binary_t<Binary<Target,Head>::value,
      merge<Output,Input<Target>,Input<Head,Rest...>>,
      Insert<Target,Input<Rest...>,merge_t<Output,Input<Head>>>
      >
    {};
    
  private:
    template<class Input,class Output,class Back>
    struct Impl
      :type_identity<Output>
    {};
    
    template<template<class...>class Input,class Output,class Back,class Head,class...Rest>
    struct Impl<Input<Head,Rest...>,Output,Back>
      :binary_t<Sort_compare_pred<Binary,Back,Head>::value,
      Impl<Input<Rest...>,merge_t<Output,Input<Head>>,Head>,
      Impl<Input<Rest...>,typename Insert<Head,Output,Input<>>::type,Head>
      >
    {};
    
  public:
    using type=typename Impl<Template,empty_t<Template>,front_t<Template>>::type;
  };
  
  template<class Template,template<class,class>class Binary>
  using insertion_sort_t=typename Insertion_sort_helper<Template,Binary>::type;
}

namespace meta_base::experimental
{
  template<template<auto,auto>class Binary,auto Left,auto Right>
  struct Sort_compare_pred
  {
    //相等返回1
    //true返回1
    //false返回2
    constexpr static size_t value=!Binary<Left,Right>::value&&!Binary<Right,Left>::value
                                  ?1:Binary<Left,Right>::value?1:0;
  };
  
    //insertion sort
  template<class OutPut,class Empty,auto Value,template<auto,auto>class Binary>
  struct Insert
  {
    using type=append_constant_t<OutPut,Value>;
  };
  
  template<class OutPut,class Empty,auto Value, template<auto,auto>class Binary>
  using Insert_t=typename Insert<OutPut,Empty,Value,Binary>::type;
  
  template<template<class Value,Value...>class OutPut,
    class Empty,
    class Value,
    template<Value,Value>class Binary,
      Value Front,Value Head,Value ...Rest>
  struct Insert<OutPut<Value,Head,Rest...>,Empty,Front,Binary>
    :meta_base::binary_t<Sort_compare_pred<Binary,Front,Head>::value,
    meta_base::merge<Empty,OutPut<Value,Front>,OutPut<Value,Head,Rest...>>,
    Insert<OutPut<Value,Rest...>,meta_base::append_constant_t<Empty,Front>,Front,Binary>
    >
  {};
  

  template<class Template,class Empty,auto Back,template<auto,auto>class Binary>
  struct insertion_sort_base
  {public:using type=Empty;};
  
  template<template<class Value,Value...>class Template,
    class Empty,
    class Value,
    template<Value,Value>class Binary,
    Value Back,
    Value Head,Value ...Rest>
  struct insertion_sort_base<Template<Value,Head,Rest...>,Empty,Back,Binary>
    :meta_base::binary_t<Binary<Back,Head>::value,
    insertion_sort_base<Template<Value,Rest...>,meta_base::append_constant_t<Empty,Head>,Head,Binary>,
    insertion_sort_base<Template<Value,Rest...>,Insert_t<Empty,Template<Value>,Head,Binary>,Head,Binary>
    >
  {};
  
  template<class Template,template<auto,auto>class Binary>
  struct insertion_sort
  {using type=Template;};
  
  template<template<class Value,Value...>class Template,
    class Value,
    template<Value,Value>class Binary,
    Value Head,Value ...Rest>
  struct insertion_sort<Template<Value,Head,Rest...>,Binary>
    :insertion_sort_base<Template<Value,Head,Rest...>,Template<Value>,Head,Binary>
  {};
  
  template<class Template,template<auto,auto>class Binary>
  using insertion_sort_t=typename insertion_sort<Template,Binary>::type ;
  
  template<int a,int b>
  using greater=std::bool_constant<(a>b)>;

  
  static_assert(meta_base::is_same_v<
    meta_base::int_sequence<7,6,5,4,4,3,2,1>,
  insertion_sort_t<meta_base::int_sequence<1,2,3,4,4,5,6,7>,greater>
  >);
  
  static_assert(meta_base::is_same_v<
    meta_base::int_sequence<9>,
    insertion_sort_t<meta_base::int_sequence<9>,greater>
  >);
  
  static_assert(meta_base::is_same_v<
    meta_base::int_sequence<9,7>,
    insertion_sort_t<meta_base::int_sequence<7,9>,greater>
  >);
}

namespace meta_base
{
  //quick sort
  
  
  //bubble sort
  
}
#endif //META_SORT_HPP
