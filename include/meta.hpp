
/*
c++14实现轻量级元编程库，
使用std::integral_constant作为模板参数代替c++17的auto关键字实现泛型模板参数
 */

#ifndef META_HPP
#define META_HPP

#include<utility>
#include<cstddef>

#define NON_STL_CXX_14 201402L
#define NON_STL_CXX_17 201703L
#define NON_STL_CXX_20 202002L

#if defined(_MSVC_LANG)
#define STL_LANG _MSVC_LANG
#else
#define STL_LANG __cplusplus
#endif

#define NON_STL_HAS_CXX(VERSION)\
(STL_LANG >= VERSION)

#define NON_STL_14 NON_STL_HAS_CXX(NON_STL_CXX_14)
#define NON_STL_17 NON_STL_HAS_CXX(NON_STL_CXX_17)
#define NON_STL_20 NON_STL_HAS_CXX(NON_STL_CXX_20)

#if NON_STL_17
#define INLINE inline
#else
#define INLINE
#endif

/*advance*/
namespace common
{
  class undefined{};
  
  template<class...> struct _aux{};
  
  template<class Constant,ptrdiff_t Dis>
  struct _advance
  {};
  
  template<class Type,Type v,ptrdiff_t Dis>
  struct _advance<std::integral_constant<Type,v>,Dis>
  {
    using type = std::integral_constant<Type,v+static_cast<Type>(Dis)>;
  };

  template<class Constant>
  struct _next
    :_advance<Constant,1>
  {};

  template<class Constant>
  struct _prev
    : _advance<Constant,-1>
  {};
  
  template<class C,ptrdiff_t D>
  using _advance_t=typename _advance<C,D>::type;
  
  template<class C>
  using _next_t=typename _next<C>::type;
  
  template<class C>
  using _prev_t=typename _prev<C>::type;
  
  template<ptrdiff_t v>
  using range_constant = std::integral_constant<ptrdiff_t ,v>;
  
  template<size_t v>
  using index_constant = std::integral_constant<size_t,v>;
  
  template<size_t...v>
  using index_sequence = std::integer_sequence<size_t,v...>;
  
  template<ptrdiff_t...v>
  using range_sequence = std::integer_sequence<ptrdiff_t,v...>;
  
  template<bool...v>
  using bool_sequence = std::integer_sequence<bool,v...>;
  
  template<char...v>
  using char_sequence = std::integer_sequence<char,v...>;
  
  template<int...v>
  using int_sequence = std::integer_sequence<int,v...>;
  
  template<bool v>
  using bool_constant = std::integral_constant<bool,v>;
  
  template<ptrdiff_t v>
  using ssize_constant = std::integral_constant<ptrdiff_t ,v>;
  
  using error_constant= std::integral_constant<int,-1>;
}

/*type_traits*/
namespace common
{
  template<class Type>
  struct type_identity
  {
    using type=Type;
  };
  
  template<class T>
  struct _type_identity
  {
    using _type = T;
  };
  
  template<class Type,template<class>class...Pred>
  struct map
    :type_identity<Type>
  {};
  
  template<class Type,template<class>class Pred>
  struct map<Type,Pred>
    :Pred<Type>
  {};
  
  template<class Type,
    template<class>class HeadP,
    template<class>class ...RestP>
  struct map<Type,HeadP,RestP...>
    :map<typename HeadP<Type>::type,RestP...>
  {};
  
  //bool A B
  //bool A bool B C
  //bool A bool B bool C D
  template<class Bool,class If,class Else,class ...>
  struct conditional
  {};
  
  template<class Bool,class A,class B>
  struct conditional<Bool,A,B>
    :std::conditional<bool(Bool::value),A,B>
  {};
  
  template<class Bool1,class If,class Bool2,class ElseIf1,class... Rest>
  struct conditional<Bool1,If,Bool2,ElseIf1, Rest...>
    :std::conditional<bool(Bool1::value),If,typename conditional<Bool2,ElseIf1,Rest...>::type>
  {};


  template<class...>
  struct is_same
    :std::true_type
  {};
  
  template<class L,class R>
  struct is_same<L,R>
    :std::is_same<L,R>
  {};
  
  template<class A,class B,class...Rest>
  struct is_same<A,B,Rest...>
    :std::integral_constant<bool,is_same<A,Rest...>::value&&is_same<B,Rest...>::value>
  {};
  
  template<int type,class...>
  struct _logic_pred
    :std::conditional_t<bool(type==1),std::false_type,std::true_type>
  {};
  
  template<int type,class T>
  struct _logic_pred<type,T>
    :std::conditional<bool(type!=2),T,bool_constant<!T::value>>::type
  {};
  
  template<int type,class H,class...R>
  struct _logic_pred<type,H,R...>
    : conditional<bool_constant<(type==0)>,
      typename conditional<H,_logic_pred<type,R...>,std::false_type>::type,
      bool_constant<(type==1)>,
      std::conditional_t<H::value,std::true_type,_logic_pred<type,R...>>,
      std::conditional_t<H::value,std::false_type,_logic_pred<type,R...>>>::type
  {};
  
  template<template<class...>class Template,class...>
  struct is_specialization
    :std::false_type
  {};
  
  template<template<class...>class Template, class...Types>
  struct is_specialization<Template,Template<Types...>>
    :std::true_type
  {};
  
  template<class T>
  struct is_bounded_array
    : std::false_type
  {};
  
  template<class T, std::size_t N>
  struct is_bounded_array<T[N]>
    : std::true_type
  {};
  
  template<class From, class To ,class = void>
  struct is_nothrow_convertible
    : _logic_pred<0,std::is_void<From>, std::is_void<To>>
  {};
  
  template<class From, class To>
  struct is_nothrow_convertible<From, To,
    std::void_t<decltype(static_cast<To(*)()>(nullptr))>>
    : bool_constant<noexcept(std::declval<void(&)(To) noexcept>()(std::declval<From>()))>
  {};
  
  template<class Type,template<class >class...Pred>
  using map_t=typename map<Type,Pred...>::type;
  
  template<class T>
  using remove_cvref = std::remove_cv<std::remove_reference_t<T>>;
  
  template<class Type>
  using remove_cvref_t = typename remove_cvref<Type>::type;
  
  template<class Bool,class If,class Else,class ... Rest>
  using conditional_t = typename conditional<Bool,If,Else,Rest...>::type;
  
  template<class...R>
  using conjunction = _logic_pred<0,R...>;
  
  template<class...R>
  using disjunction = _logic_pred<1,R...>;
  
  template<class...R>
  using negation = typename _logic_pred<2,R...>::type;
  
  template<class From,class To>
  using copy_const_t = std::conditional_t<std::is_const<From>::value, std::add_const_t<To>, To>;
  
  template<class From,class To>
  using copy_volatile_t = std::conditional_t<std::is_volatile<From>::value, std::add_volatile_t<To>, To>;
  
  template<class From,class To>
  using copy_cv_t = copy_const_t<From, copy_volatile_t<From, To>>;
  
  
  template<class From,class To>
  using copy_reference_t = conditional_t<
    std::is_rvalue_reference<From>, std::add_rvalue_reference_t<To>,
    std::is_lvalue_reference<From>, std::add_lvalue_reference_t<To>,
    To>;
  
  template<class From,class To>
  using copy_cvref_t = copy_reference_t<From, copy_cv_t<From, To>>;
  
  template<class From,class To>
  using copy_cvref = type_identity<copy_reference_t<From, copy_cv_t<From, To>>>;
  
}

/*Template traits*/
namespace common
{
  namespace _details
  {
    template<class T>
    struct _is_template
      :std::false_type
    {};
    
    template<template<class...>class TL,class...T>
    struct _is_template<TL<T...>>
      :std::true_type
    {};
    
    template<template<class VT,VT...>class TL,class VT,VT..._v>
    struct _is_template<TL<VT,_v...>>
      :std::true_type
    {};
    
    template<class Value, Value v>
    struct _is_template<std::integral_constant<Value,v>>
      :std::false_type
    {};
    
    template<class>
    struct _meta_traits
    {
      static_assert(false,"T must be a variadic template");
    };
    
    template<template<class ...> class Template, class...Types>
    struct _meta_traits<Template<Types...>>
    {
      using type=Template<>;
      static constexpr bool S_normal = true;
      using value_type=undefined;
      static constexpr size_t size= sizeof...(Types);
    };
    
    template<template<class Value, Value...> class Template, class Value, Value...value>
    struct _meta_traits<Template<Value, value...>>
    {
      using type=Template<Value>;
      static constexpr bool S_normal = false;
      using value_type=Value;
      static constexpr size_t size= sizeof...(value);
    };
  }
  
  template<class T>
  struct is_template
    :_details::_is_template<remove_cvref_t<T>>
  {};
  
  template<class T>
  struct meta_traits
    : _details::_meta_traits<remove_cvref_t<T>>
  {};
  
  template<class List>
  using make_empty_t = typename meta_traits<remove_cvref_t<List>>::type;
  
  template<class List>
  using make_empty = type_identity<make_empty_t<List>>;
  
  template<class Template>
  using traits_value_t = typename meta_traits<Template>::value_type;
  
  template<class TL>
  using is_empty = bool_constant<meta_traits<TL>::size==0>;
  
  template<class TL>
  using is_normal = bool_constant<meta_traits<TL>::S_normal>;
  
  template<class TL>
  using extent = index_constant<meta_traits<TL>::size>;
  
  namespace _details
  {
    template<class List,ptrdiff_t Idx,ptrdiff_t Extent=ptrdiff_t(extent<List>::value)>
    struct _out_of_range
      :bool_constant<(Idx>=0)?(Idx>=Extent):(Idx<-Extent)>
    {};
    
    template<ptrdiff_t Idx,class T>
    struct _actual_index
      :std::conditional_t<(Idx>=0),ssize_constant<Idx>,ssize_constant<Idx+ptrdiff_t(extent<T>::value)>>
    {};
  }
}

/*inline advance*/
namespace common
{
  template<class List>
  INLINE constexpr bool is_empty_v = !meta_traits<List>::size;
  
  template<class List>
  INLINE constexpr bool is_normal_v = meta_traits<List>::S_normal;
  
  template<class Template>
  INLINE constexpr auto extent_v = meta_traits<Template>::size;
  
  template<class T>
  INLINE constexpr bool is_template_v = is_template<remove_cvref_t<T>>::value;
  
  template<typename T>
  INLINE constexpr bool is_bounded_array_v = is_bounded_array<std::remove_reference_t<T>>::value;
  
  template<class...T>
  INLINE constexpr bool is_same_v = is_same<T...>::value;
}
//merge
namespace common
{
  template<class...Lists>
  class _merge
  {};
  
  template<class Head,class...Rest>
  class _merge<Head,Rest...>
  {
    static_assert(conjunction<is_template<Head>,is_template<Rest>...>::value);
  private:
    template<class...>
    struct _impl
    {};
    
    template<template<class...>class Template,class...Types>
    struct _impl<Template<Types...>>
      :type_identity<Template<Types...>>
    {};
    
    template<template<class Value,Value...>class Template,class Value,Value...value>
    struct _impl<Template<Value,value...>>
      :type_identity<Template<Value,value...>>
    {};
    
    template<template<class...>class Template,class...P,class...O>
    struct _impl<Template<P...>,Template<O...>>
      :type_identity<Template<P...,O...>>
    {};
    
    template<template<class Value,Value...>class Template,class Value,Value...P,Value...O>
    struct _impl<Template<Value,P...>,Template<Value,O...>>
      :type_identity<Template<Value,P...,O...>>
    {};
    
    template<class T,class U,class...Other>
    struct _impl<T,U,Other...>
      :_impl<typename _impl<T,U>::type,Other...>
    {};
  public:
    using type=typename _impl<Head,Rest...>::type;
  };
  
  template<class...Lists>
  using merge=_merge<remove_cvref_t<Lists>...>;
  
  template<class...Lists>
  using merge_t=typename merge<Lists...>::type;
}

//cxx20 common_type
namespace common
{
  //common_type cxx20 form
  template<class...>
  struct common_type
  {};
  
  template<class T>
  struct common_type<T>
    :type_identity<T>
  {};
  
  template<class T1,class T2>
  struct common_type<T1,T2>
  {
  private:
    
    template<class L,class R>
    using condition_type=std::decay_t<decltype(false?std::declval<L>():std::declval<R>())>;
    
    template<class L,class R,class=void>
    struct Impl_base
    {};
    
    template<class L,class R>
    struct Impl_base<L,R,std::void_t<condition_type<L,R>>>
    {
      using type =condition_type<L,R>;
    };
    
    template<class L,class R,class=void>
    struct Impl
      :Impl_base<L,R>
    {};
    
    template<class L,class R>
    struct Impl<L,R,std::void_t<condition_type<const L&,const R&>>>
      :type_identity<condition_type<const L&,const R&>>
    {};
  public:
    using type=typename Impl<T1,T2>::type;
  };
  
  template<class T1,class T2,class...Rest>
  struct common_type<T1,T2,Rest...>
    :common_type<typename common_type<T1,T2>::type,Rest...>
  {};
  
  template<class...Types>
  using common_type_t=typename common_type<Types...>::type;
}

//filter
namespace common
{
  enum struct Category
  {
    S_unique,
    S_take,
    S_sub,
    S_equal_range,
    S_insert,
    S_insert_if
  };
  
  template<Category c,class T,template<class,class...>class Pred,class...>
  struct _filter
    :type_identity<undefined>
  {};
  
  struct _common_pred
  {
    template<ptrdiff_t Idx,template<class,class...>class,class,class...>
    struct _first_pos
      :ssize_constant<-1>
    {};
    
    template<ptrdiff_t Idx,template<class,class...>class T,template<class...>class L,class H,class...R,class...Other>
    struct _first_pos<Idx,T,L<H,R...>,Other...>
      :std::conditional_t<T<H,Other...>::value,ssize_constant<Idx>,
       _first_pos<Idx+1,T,L<R...>,Other...>>
    {};
    
    template<class T,class TL>
    struct _first_index
      :_first_pos<0,is_same,TL,T>
    {};
    
    template<class T,class TL>
    struct _first
      :bool_constant<_first_pos<0,is_same,TL,T>::value!=-1>
    {};
    
    template<template<class V,V...>class,class,class>
    struct _traits
    {};
    
    template<template<class V,V...>class Sequence,class V,V...v>
    struct _traits<Sequence,V,_aux<std::integral_constant<V,v>...>>
    :type_identity<Sequence<V,v...>>
    {};
    
    template<template<class V,V...>class S,class V,class TL>
    using _traits_t=typename _traits<S,V,TL>::type;
  };
}

//unique
namespace common
{
  template<template<class...>class T,class H,class...R,class...O,template<class,class>class Traits,class Bool>
  struct _filter<Category::S_unique,T<H,R...>,Traits,T<O...>,Bool>
    :std::conditional_t<Traits<H,T<O...>>::value,
      _filter<Category::S_unique,T<R...>,Traits,T<O...>,std::false_type>,
      _filter<Category::S_unique,T<R...>,Traits,T<O...,H>,std::true_type>>
  {};
  
  template<class T,class O,template<class,class>class Traits,class Bool>
  struct _filter<Category::S_unique,T,Traits,O,Bool>
    :Bool,_type_identity<O>
  {};
  
  template<class TL,class=std::enable_if_t<is_template<TL>::value>>
  struct _unique
    :private _common_pred
  {
  private:
    using _base=_filter<Category::S_unique,TL,_first,make_empty_t<TL>,std::true_type>;
  public:
    static constexpr bool value=_base::value;
    using type=typename _base::_type;
  };
  
  template<template<class V,V...v>class TL,class V,V...v>
  struct _unique<TL<V,v...>>
    :private _common_pred
  {
  private:
    using _sequence=_aux<std::integral_constant<V,v>...>;
    using _base=_filter<Category::S_unique,remove_cvref_t<_sequence>,_first,_aux<>,std::true_type>;
    using _type=copy_cvref_t<_sequence,typename _base::_type>;
  public:
    static constexpr bool value=_base::value;
    using type=_traits_t<TL,V,_type>;
  };
}
//reverse
namespace common
{
  template<class List,bool=is_template_v<List>>
  class _reverse
  {
  private:
    template<class T,class E>
    struct _impl
      :type_identity<E>
    {};
    
    template<template<class...> class Template,class...O,class Head,class...Rest>
    struct _impl<Template<Head,Rest...>,Template<O...>>
      :_impl<Template<Rest...>,Template<Head,O...>>
    {};
    template<template<class Value,Value...> class Template,class Value,Value Head,Value...Rest,Value...O>
    struct _impl<Template<Value,Head,Rest...>,Template<Value,O...>>
      :_impl<Template<Value,Rest...>,Template<Value,Head,O...>>
    {};
  private:
    using _type=typename _impl<remove_cvref_t<List>,make_empty_t<List>>::type;
  public:
    using type=copy_cvref_t<List,_type>;
  };
  
  template<class Template>
  using reverse_t=typename _reverse<remove_cvref_t<Template>>::type;
  
  template<class Template>
  using reverse = _reverse<remove_cvref_t<Template>>;
}
//get_n
namespace common
{
  namespace _details
  {
    template<class,ptrdiff_t _index,class E , class = void>
    struct _get
      :type_identity<E>
    {};
    
    template<template<class...>class TL,class A,class...Rest,class E>
    struct _get<TL<A,Rest...>,0,E>
      :type_identity<A>
    {};
    
    template<template<class...>class TL,class A,class B,class...Rest,class E>
    struct _get<TL<A,B,Rest...>,1,E>
      :type_identity<B>
    {};
    
    template<template<class...>class TL,class A,class B,class C,class...Rest,class E>
    struct _get<TL<A,B,C,Rest...>,2,E>
      :type_identity<C>
    {};
    
    template<ptrdiff_t _index,template<class...>class TL,class A,class B,class C,class D,class...Rest,class E>
    struct _get<TL<A,B,C,D,Rest...>,_index,E,std::enable_if_t<(_index>2)>>
      :_get<TL<D,Rest...>,_index-3,E>
    {};
    
    template<template<class VT,VT...>class TL,class VT,VT..._v,ptrdiff_t _index,class E>
    struct _get<TL<VT,_v...>,_index,E>
      :_get<_aux<std::integral_constant<VT,_v>...>,_index,E>
    {};
  }
  
  template<class TL,ptrdiff_t _index,class E=undefined>
  using get_n = _details::_get<TL,_details::_actual_index<_index,TL>::value,E>;
  
  template<class TL,ptrdiff_t _index,class E=undefined>
  using get_n_t=typename get_n<TL,_index,E>::type;
  
  template<class Template>
  using front_t = get_n_t<Template,0>;
  
  template<class Template>
  using back_t = get_n_t<Template,-1>;
  
  template<class TL,ptrdiff_t _index,class E=error_constant>
  INLINE constexpr auto get_n_v=get_n_t<TL,_index,E>::value;
  
  template<class Template,class ErrorType=error_constant>
  INLINE constexpr auto front_v=get_n_v<Template,0>;
  
  template<class Template,class ErrorType=error_constant>
  INLINE constexpr auto back_v=front_v<reverse_t<Template>>;
}
//take
namespace common
{

  template<template<class...>class TL,class H,class...R,
    template<class,class>class Traits,class Num,class Init,class O>
  struct _filter<Category::S_take,TL<H,R...>,Traits,Num,Init,O>
    :std::conditional_t<Traits<Num,Init>::value,
      type_identity<O>,
      _filter<Category::S_take,TL<R...>,Traits,Num,_next_t<Init>,merge_t<O,TL<H>>>>
  {};
  
  template<class TL,template<class,class>class Traits,class Num,class Init,class O>
  struct _filter<Category::S_take,TL,Traits,Num,Init,O>
    :type_identity<O>
  {};
  
  template<class T,size_t r>
  struct _take
    :_filter<Category::S_take,T,is_same,index_constant<r>,index_constant<0>,make_empty_t<T>>
  {};

  template<template<class VT,VT...>class Sequence,class ValueType,ValueType...value,size_t Idx>
  struct _take<Sequence<ValueType,value...>,Idx>
    :_common_pred
  {
    using type=_traits_t<Sequence,ValueType,typename _take<_aux<std::integral_constant<ValueType,value>...>,Idx>::type>;
  };
}
//equal_range
//count
//count_if
namespace common
{
  //统计出现的位置以及次数
  template<template<class...>class List,class Head,class...Rest,/*TypeList*/
    template<class,class...>class Pred,/*Pred*/
    class...Parameters,/*Other*/
    class Sequence,/*equal_range*/
    class Init,/*loop*/
    class Count/*Count*/>
  struct _filter<Category::S_equal_range,List<Head,Rest...>,Pred,Sequence,Init,Count,Parameters...>
    :std::conditional_t<Pred<Head,Parameters...>::value,
      _filter<Category::S_equal_range,
        List<Rest...>,Pred,merge_t<Sequence,index_sequence<Init::value>>,_next_t<Init>,_next_t<Count>,Parameters...>,
      _filter<Category::S_equal_range,
        List<Rest...>,Pred,Sequence,_next_t<Init>,Count,Parameters...>
    >
  {};
  
  template<class List,
    template<class,class...>class Pred,class Count,
    class...Parameters,class Sequence,class Init>
  struct _filter<Category::S_equal_range,List,Pred,Sequence,Init,Count,Parameters...>
  {
    using sequence=Sequence;
    using count=Count;
  };
  
  template<class T,template<class,class...>class Traits,class...P>
  struct _equal_range
  {
  private:
    using _base=_filter<Category::S_equal_range,T,Traits,index_sequence<>,index_constant<0>,index_constant<0>,P...>;
    using _count=typename _base::count;
    using _sequnce=typename _base::sequence;
  public:
    static constexpr auto count=_count::value;
    using type=_sequnce;
  };
  
  template<template<class V,V...>class T,class V,V...v,
    template<class,class...>class Traits,class...P>
  struct _equal_range<T<V,v...>,Traits,P...>
    :_equal_range<_aux<std::integral_constant<V,v>...>,Traits,P...>
{};
}

/* erase_if
 * insert_if
 * remove
*/
namespace common
{
  //insert_if erase_if
  template<template<class...>class List,
    template<class,class...>class Pred,
    class Bool,
    class Head,class...Rest,
    class...Para,
    class...Increments>
  struct _filter<Category::S_insert_if,List<Head,Rest...>,Pred,List<Increments...>,Bool,Para...>
    :std::conditional_t<Pred<Head,Para...>::value,
      merge<std::conditional_t<Bool::value,List<Increments...,Head>,List<>>,
        typename _filter<Category::S_insert_if,List<Rest...>,Pred,List<Increments...>,Bool,Para...>::type>,
      merge<List<Head>,
        typename _filter<Category::S_insert_if,List<Rest...>,Pred,List<Increments...>,Bool,Para...>::type>
    >
  {};
  
  template<class List,template<class,class...>class Pred,class...Para,class Add,class Bool>
  struct _filter<Category::S_insert_if,List,Pred,Add,Bool,Para...>
    :make_empty<List>
  {};
  
  //insert_if
  template<class,template<class,class...>class,class,class...>
  struct _insert_or_erase_if
    :type_identity<undefined>
  {};
  
  //insert_if
  template<template<class...>class List,
    template<class,class...>class Pred,
    class Bool,
    class...Add,class...Types,class...Other>
  struct _insert_or_erase_if<List<Types...>,Pred,List<Add...>,Bool,Other...>
    : _filter<Category::S_insert_if,List<Types...>,Pred,List<Add...>,Bool,Other...>
  {};
  
  //insert_if_c
  template<template<class V,V...>class List,
    template<class,class...>class Pred,
    class Bool,
    class V,V...Add,V...v,class...Para>
  struct _insert_or_erase_if<List<V,v...>,Pred,List<V,Add...>,Bool,Para...>
    :_common_pred
  {
  private:
    using _sequence=_aux<std::integral_constant<V,v>...>;
    using _add=_aux<std::integral_constant<V,Add>...>;
    using _base=typename _filter<Category::S_insert_if,_sequence,Pred,_add,Bool,Para...>::type;
  public:
    using type=_traits_t<List,V,_base>;
  };
  
  template<class Template,template<class,class...>class Predicate,
    class AddList,class...Para>
  using _insert_if_t=typename _insert_or_erase_if<Template,Predicate,AddList,std::true_type,Para...>::type;
  
  //erase_if_t base
  template<class Template,template<class,class...>class Pred,class...Other>
  using  _erase_if_t=typename _insert_or_erase_if<Template,Pred,make_empty_t<Template>,std::false_type,Other...>::type;
  
  //移除相同的元素
  template<class Template,class T>
  using _remove_t=_erase_if_t<Template,is_same,T>;
}

/* erase insert*/
namespace common
{
  //插入(Idx)
  template<template<class...>class List,class Head,class...Rest,
    template<class,class...>class Pred,
    class AddList,
    class Bool,
    class Index,class Zero,
    class Out>
  struct _filter<Category::S_insert,List<Head,Rest...>,Pred,Out,AddList,Zero,Index,Bool>
    :std::conditional_t<Pred<Zero,Index>::value,
      std::conditional_t<Bool::value,merge<Out,AddList,List<Head,Rest...>>,merge<Out,List<Rest...>>>,
      _filter<Category::S_insert,List<Rest...>,Pred,merge_t<Out,List<Head>>,AddList,_next_t<Zero>,Index,Bool>
    >//conditional_t
  {};
  
  template<class List,
    template<class,class...>class Pred,
    class AddList,
    class Bool,
    class Index,class Zero,
    class Out>
  struct _filter<Category::S_insert,List,Pred,Out,AddList,Zero,Index,Bool>
    :type_identity<Out>
  {};
  
  //erase
  template<class Template,ptrdiff_t Idx,class AddList,class Bool>
  struct _erase_or_insert
    :type_identity<undefined>
  {};
  
  template<template<class...>class Template/*Type List*/,
    ptrdiff_t Idx/*insert pos*/,
    class Bool,
    class...Add,class...Types>
  struct _erase_or_insert<Template<Types...>,Idx,Template<Add...>,Bool>
  {
  private:
    using _self=Template<Types...>;
    using _add=Template<Add...>;
    static constexpr auto S_out_of_range=_details::_out_of_range<_self,Idx>::value;
    static constexpr auto S_index_v=_details::_actual_index<Idx,_self>::value;
    
    using _type=std::conditional_t<(S_index_v>=0),
      Template<Types...,Add...>,
      Template<Add...,Types...>>;
    
    using _base=_filter<Category::S_insert,_self,is_same,make_empty_t<_self>,_add,
      ssize_constant<0>,ssize_constant<_details::_actual_index<Idx,_self>::value>,Bool>;
  public:
    using type=std::conditional_t<S_out_of_range,_type,typename _base::type>;
  };
  //erase
  template<template<class V,V...>class Template,ptrdiff_t Idx,class V,V...v,V...add,class Bool>
  struct _erase_or_insert<Template<V,v...>,Idx,Template<V,add...>,Bool>
    :_common_pred
  {
  private:
    using _sequence=_aux<std::integral_constant<V,v>...>;
    using _base=typename _erase_or_insert<_sequence,Idx,_aux<std::integral_constant<V,add>...>,Bool>::type;
  public:
    using type=_traits_t<Template,V,_base>;
  };
  
  //insert
  template<class/*Type List*/,
    ptrdiff_t/*insert pos*/,
    class...>
  struct _insert
    :type_identity<undefined>
  {};
  
  //erase
  template<class/*Type List*/,
    ptrdiff_t>
  struct _erase
    :type_identity<undefined>
  {};
  
  template<template<class...>class Template/*Type List*/,
    ptrdiff_t Idx/*insert pos*/,
    class...Add,class...Types>
  struct _insert<Template<Types...>,Idx,Add...>
    :_erase_or_insert<Template<Types...>,Idx,Template<Add...>,std::true_type>
  {};
  
  template<template<class V,V...>class TL/*Type List*/,
    ptrdiff_t Idx/*insert pos*/,
    class V,V...v,
    V...add>
  struct _insert<TL<V,v...>,Idx,TL<V,add...>>
    :_common_pred
  {
  private:
    using _sequence=_aux<std::integral_constant<V,v>...>;
    using _base=typename _insert<_sequence,Idx,std::integral_constant<V,add>...>::type;
  public:
    using type=_traits_t<TL,V,_base>;
  };
  
  template<class TL/*Type List*/,
    ptrdiff_t Idx/*insert pos*/,
    class...Add>
  using _insert_t=typename _insert<TL,Idx,Add...>::type;
  
  template<template<class...>class Template/*Type List*/,
    ptrdiff_t Idx/*insert pos*/,class...Types>
  struct _erase<Template<Types...>,Idx>
    :_erase_or_insert<Template<Types...>,Idx,Template<>,std::false_type>
  {};
  
  template<template<class V,V...>class TL/*Type List*/,
    ptrdiff_t Idx/*insert pos*/,
    class V,V...v>
  struct _erase<TL<V,v...>,Idx>
    :_common_pred
  {
  private:
    using _sequence=_aux<std::integral_constant<V,v>...>;
    using _base=typename _erase<_sequence,Idx>::type;
  public:
    using type=_traits_t<TL,V,_base>;
  };
  
  //erase_t
  template<class Template,ptrdiff_t Idx>
  using  _erase_t=typename _erase<Template,Idx>::type;
}
/*rotate*/
namespace common
{
  template<class TL,ptrdiff_t I>
  struct _rotate
  {
  private:
    static constexpr auto S_index_v=extent<TL>::value!=0?I%ptrdiff_t(extent<TL>::value):0;
    
    template<class L,ptrdiff_t Index,bool v=S_index_v>=0>
    struct Impl
      :type_identity<L>
    {};
    
    template<template<class...>class L,class H,class...R,ptrdiff_t Index>
    struct Impl<L<H,R...>,Index,false>
      :std::conditional_t<Index==S_index_v,
        type_identity<L<H,R...>>,
        Impl<L<R...,H>,Index-1>>
    {};
    
    template<class,class...>struct prepend{};
    
    template<template<class...>class L,class...T,class...O>
    struct prepend<L<T...>,O...>
      :type_identity<L<O...,T...>>
    {};
    
    template<class T,class...O>
    using prepend_t=typename prepend<T,O...>::type;
    
    template<template<class...>class L,class H,class...R,ptrdiff_t Index>
    struct Impl<L<H,R...>,Index,true>
      :std::conditional_t<Index==S_index_v,
        type_identity<L<H,R...>>,
        Impl<prepend_t<_erase_t<L<H,R...>,-1>,get_n_t<L<H,R...>,-1>>,Index+1>>
    {};
  
  public:
    using type=typename Impl<TL,0>::type;
  };
}
/*sort*/
namespace common
{
  
  template<template<class,class>class Traits,class L,class R>
  using _equal_pred=bool_constant<!Traits<L,R>::value&&!Traits<R,L>::value>;
  
  template<class TL,template<class...>class BinaryPred>
  struct _insertion_sort
    :make_empty<TL>
  {};
  
  template<template<class...>class TL,
    template<class,class>class B,
    class Head,class...Rest>
  struct _insertion_sort<TL<Head,Rest...>,B>
    :_common_pred
  {
  private:
    template<class L,class R>
    static constexpr auto S_comparison_v=B<L,R>::value||_equal_pred<B,L,R>::value;
    
    template<class L,template<class,class...>class Traits,class...Other>
    static constexpr auto _first_pos_v=_first_pos<0,Traits,L,Other...>::value;
    
    template<class L,class R>
    using _not=B<R,L>;
    
    template<class,class Output,class Back>
    struct Impl
      :type_identity<Output>
    {};
    
    template<class Output,class Back,template<class...>class List,class H,class...R>
    struct Impl<List<H,R...>,Output,Back>
      :std::conditional_t<S_comparison_v<Back,H>,
        Impl<List<R...>,merge_t<Output,List<H>>,H>,
        Impl<List<R...>,_insert_t<Output,_first_pos_v<Output,_not,H>,H>,back_t<Output>>>
    {};
  public:
    using type=typename Impl<TL<Head,Rest...>,TL<>,Head>::type;
  };
  //1,2,3,4,5,2-->less<l,r>==l<r
  
  template<template<class V,V...>class TL,
    template<class,class>class B,
    class V,V...v>
  struct _insertion_sort<TL<V,v...>,B>
    :_common_pred
  {
  private:
    using _sequence=_aux<std::integral_constant<V,v>...>;
  public:
    using type=_traits_t<TL,V,typename _insertion_sort<_sequence,B>::type>;
  };
  
  template<class L,class R>
  struct _less
    :bool_constant<sizeof(L)<sizeof(R)>
  {};
  
  template<class L,class R>
  struct _greater
    :bool_constant<(sizeof(L)> sizeof(R))>
  {};
  
  template<class V,V L,V R>
  struct _less<std::integral_constant<V,L>,std::integral_constant<V,R>>
  :bool_constant<L<R>
  {};

  template<class V,V L,V R>
  struct _greater<std::integral_constant<V,L>,std::integral_constant<V,R>>
  :bool_constant<(L>R)>
  {};

  template<class L,template<class,class>class B>
  using _insertion_sort_t=typename _insertion_sort<L,B>::type;
  
  namespace _details
  {
    template<class VT,class T>
    struct _make_sequence
    {
      using type = std::integer_sequence<VT>;
    };
    
    template<class VT,VT..._v>
    struct _make_sequence<VT,_aux<std::integral_constant<VT,_v>...>>
    {
      using type = std::integer_sequence<VT,_v...>;
    };
    
    template<class VT,class T>
    using _make_sequence_t = typename _make_sequence<VT,T>::type;
  }
  
  namespace _merge_sort
  {
    template<template<class,class>class Binary,class>
    struct _min_sort;
    
    template<template<class,class>class Binary,class A,class B>
    struct _min_sort<Binary,_aux<A,B>>
      :std::conditional_t<Binary<A,B>::value,type_identity<_aux<A,B>>,type_identity<_aux<B,A>>>
    {};
    
    template<template<class,class>class Binary,class T>
    using _min_sort_t = typename _min_sort<Binary,T>::type;
    
    template<template<class,class>class Binary,
      class  S1,class S2,class O>
    struct _merge_two_sequence
      :std::conditional_t<bool(extent<S1>::value>0),
      merge<O,S1>,
      merge<O,S2>>
    {};
    
    template<template<class,class>class Binary,
      template<class...>class TL,
      class H1,class H2,class...A,class...B,class...C>
    struct _merge_two_sequence<Binary,TL<H1,A...>,TL<H2,B...>,TL<C...>>
      :std::conditional_t<Binary<H1,H2>::value,
      _merge_two_sequence<Binary,TL<A...>,TL<H2,B...>,TL<C...,H1>>,
        _merge_two_sequence<Binary,TL<H1,A...>,TL<B...>,TL<C...,H2>>>
    {};
    
    template<template<class,class>class Binary,
      template<class VT,VT...>class TL,
      class VT,VT H1,VT H2,
      VT...A,VT...B,VT...C>
    struct _merge_two_sequence<Binary,TL<VT,H1,A...>,TL<VT,H2,B...>,TL<VT,C...>>
      :std::conditional_t<Binary<std::integral_constant<VT,H1>,std::integral_constant<VT,H2>>::value,
        _merge_two_sequence<Binary,TL<VT,A...>,TL<VT,H2,B...>,TL<VT,C...,H1>>,
        _merge_two_sequence<Binary,TL<VT,H1,A...>,TL<VT,B...>,TL<VT,C...,H2>>>
    {};
    
    template<template<class,class>class Binary,class S1,class S2>
    using _merge_two_sequence_t = typename _merge_sort::_merge_two_sequence<Binary,S1,S2,make_empty_t<S1>>::type;
    
    template<class TL,class...Add>
    using _append=_insert<TL,extent_v<TL>,Add...>;
    
    template<template<class,class>class,class T>
    struct _split
    {
      using type = T;
    };
    
    template<template<class,class>class Binary,class T>
    struct _split<Binary,_aux<T>>
      :type_identity<_aux<_aux<T>>>
    {};
    
    template<template<class,class>class Binary,class T>
    using  _split_t = typename _split<Binary,T>::type;
    
    template<template<class,class>class Binary,class A,class B>
    struct _split<Binary,_aux<A,B>>
    {
      using type = _aux<_min_sort_t<Binary,_aux<A,B>>>;
    };
    
    template<template<class,class>class Binary,class A,class B,class C>
    struct _split<Binary,_aux<A,B,C>>
    {
      //_aux<_aux<A,B>,_aux<C>>
      using type = typename _append<_split_t<Binary,_aux<A,B>>,_aux<C>>::type;
    };
    
    template<template<class,class>class Binary,class A,class B,class C,class D>
    struct _split<Binary,_aux<A,B,C,D>>
    {
      using type = typename _append<_split_t<Binary,_aux<A,B>>,_min_sort_t<Binary,_aux<C,D>>>::type;
    };
    
    template<template<class,class>class Binary,class A,class B,class C,class D,class E,class...R>
    struct _split<Binary,_aux<A,B,C,D,E,R...>>
      :common::merge<_split_t<Binary,_aux<A,B,C,D>>,_split_t<Binary,_aux<E,R...>>>
    {};
    
    template<class T>
    struct _is_aux
      :std::false_type
    {};
    
    template<class ...T>
    struct _is_aux<_aux<T...>>
      :std::true_type
    {};
    
    template<template<class ,class>class B,template<class VT,VT..._v>class TL,class VT,VT..._v>
    struct _split<B,TL<VT,_v...>>
    {
    private:
      using _base= typename _split<B,_aux<std::integral_constant<VT,_v>...>>::type;
      
      template<class>
      struct _traits
      {};
      
      template<class...D>
      struct _traits<_aux<D...>>
      {
        using type = _aux<_details::_make_sequence_t<VT,D>...>;
      };
      
      template<>
      struct _traits<_aux<>>
        :type_identity<_aux<std::integer_sequence<VT>>>
      {};
      
      template<class T>
      using _traits_t = typename _traits<T>::type;
      
    public:
      using type = _traits_t<_base>;
    };
    
    template<template<class,class>class B,class T>
    struct _merge
      :type_identity<T>
    {};
    
    template<template<class,class>class B,class T>
    using _merge_t = typename _merge<B,T>::type;
    
    template<template<class,class>class T,class A>
    struct _merge<T,_aux<A>>
    {
      using type = A;
    };
    
    template<template<class,class>class T,class A,class B>
    struct _merge<T,_aux<A,B>>
    {
      using type = _merge_two_sequence_t<T,A,B>;
    };
    
    template<template<class,class>class T,class A,class B,class C>
    struct _merge<T,_aux<A,B,C>>
    {
      using type = _merge_two_sequence_t<T,_merge_two_sequence_t<T,A,B>,C>;
    };
    
    template<template<class,class>class T,class A,class B,class C,class D>
    struct _merge<T,_aux<A,B,C,D>>
    {
      using type = _merge_two_sequence_t<T,_merge_two_sequence_t<T,A,B>,_merge_two_sequence_t<T,C,D>>;
    };
    
    template<template<class,class>class T,class A,class B,class C,class D,class E,class...R>
    struct _merge<T,_aux<A,B,C,D,E,R...>>
    {
      using type = _merge_two_sequence_t<T,_merge_t<T,_aux<A,B,C,D>>,_merge_t<T,_aux<E,R...>>>;
    };
    
    template<template<class,class>class B,class T>
    struct _merge_sort
    {};
    
    template<template<class,class>class B,template<class VT,VT...>class TL,class VT,VT..._v>
    struct _merge_sort<B,TL<VT,_v...>>
      :_merge<B,_split_t<B,TL<VT,_v...>>>
    {};
    
    template<template<class,class>class B,class T>
    using _merge_sort_t = typename _merge_sort<B,T>::type;
  }
  
  template<size_t _num,size_t _left,size_t _right,size_t _parent,class W>
  struct _huffman_node
  {};

  template<class,size_t,size_t>
  struct _change_node
  {
  
  };
  
  
  template<class T>
  struct _make_huffman_tree
  {};
  
  template<class T,T..._v>
  struct _make_huffman_tree<std::integer_sequence<T,_v...>>
  {
    using _num_list = std::make_index_sequence<sizeof...(_v)>;
    
    template<class,class>
    struct _make_node_list{};
    
    template<size_t..._index,T..._value>
    struct _make_node_list<std::index_sequence<_index...>,std::integer_sequence<T,_value...>>
    {
      using type = _aux<_huffman_node<_index,0,0,0,std::integral_constant<T,_value>>...>;
    };
    
    using _node_list = typename _make_node_list<_num_list,std::integer_sequence<T,_v...>>::type;
  };

}

namespace common
{
  template<class TL>
  using unique=copy_cvref_t<TL,_unique<remove_cvref_t<TL>>>;
  
  template<class TL>
  using unique_t=typename unique<TL>::type;
  
  template<class T,size_t r>
  using take=copy_cvref<T,typename _take<remove_cvref_t<T>,r>::type>;
  
  template<class T,size_t r>
  using take_t=typename take<T,r>::type;
  
  template<class T,template<class,class...>class Traits,class...P>
  using equal_range
    =copy_cvref<T,typename _equal_range<remove_cvref_t<T>,Traits,P...>::type>;
  
  template<class T,template<class,class...>class Traits,class...P>
  using equal_range_t=typename equal_range<T,Traits,P...>::type;
  
  template<class TL,ptrdiff_t Idx,class...Add>
  using insert
    =copy_cvref<TL,typename _insert<remove_cvref_t<TL>,Idx,Add...>::type>;
  
  template<class TL,ptrdiff_t Idx,class...Add>
  using insert_t=typename insert<TL,Idx,Add...>::type;
  
  template<class TL,class...Add>
  using prepend_t=typename insert<TL,0,Add...>::type;
  
  template<class TL,class...Add>
  using append_t=typename insert<TL,extent_v<TL>,Add...>::type;
  
  template<class TL,class T>
  using remove_t=copy_cvref_t<TL,_remove_t<remove_cvref_t<TL>,T>>;
  
  template<class TL,class T>
  using remove=type_identity<remove_t<TL,T>>;
  
  template<class TL,ptrdiff_t Idx>
  using erase_t=copy_cvref_t<TL,_erase_t<remove_cvref_t<TL>,Idx>>;
  
  template<class TL,ptrdiff_t Idx>
  using erase=type_identity<erase_t<TL,Idx>>;
  
  template<class TL,template<class,class...>class Pred,class...Other>
  using erase_if_t=copy_cvref_t<TL,_erase_if_t<remove_cvref_t<TL>,Pred,Other...>>;
  
  template<class TL,template<class,class...>class Pred,class...Other>
  using erase_if=type_identity<erase_if_t<TL,Pred,Other...>>;
  
  template<class TL,ptrdiff_t Idx>
  using rotate=copy_cvref<TL,typename _rotate<remove_cvref_t<TL>,Idx>::type>;
  
  template<class TL,ptrdiff_t Idx>
  using rotate_t=typename rotate<TL,Idx>::type;
  
  template<class L,template<class,class>class B>
  using insertion_sort_t=copy_cvref_t<L,_insertion_sort_t<L,B>>;
  
  template<class L,template<class,class>class B>
  using insertion_sort=copy_cvref<L,_insertion_sort_t<L,B>>;
  
  template<template<class,class>class B,class T>
  using merge_sort_t = typename _merge_sort::_merge_sort<B,T>::type;
  
  template<class List,template<class,class...>class Traits,class...P>
  using count_if = ssize_constant<_equal_range<remove_cvref_t<List>,Traits,P...>::count>;
  
  template<class List,class Type>
  using count = count_if<List,is_same,Type>;
  
  template<class List,template<class,class...>class Traits,class...P>
  INLINE constexpr size_t count_if_v=_equal_range<remove_cvref_t<List>,Traits,P...>::count;
  
  template<class List,class Type>
  INLINE constexpr size_t count_v=_equal_range<remove_cvref_t<List>,is_same,Type>::count;
  
  template<class TL>
  INLINE constexpr bool is_unique_v=unique<TL>::value;
}

namespace common
{
  namespace ext
  {

  }
}

/*test*/
namespace common
{
  #if 0
  template<class...>class variant{};
  template<class...>class tuple{};
  
  template<class...>class TypeList{};
  
  static_assert(is_same_v<typename _rotate<_aux<int,double>,2>::type,
                _aux<int,double>>);
  static_assert(is_same_v<typename _rotate<_aux<int,double,char>,-2>::type,
                _erase_t<_aux<double,char,int,double>,0>>);
  
  static_assert(is_same_v<typename _rotate<_aux<int,double,char>,-4>::type,
                _aux<double,char,int>>);
  
  static_assert(is_same_v<_erase_t<_aux<int>,0>,_aux<>>);
  static_assert(is_same_v<_erase_t<_aux<int,double,char,int,float,int,double>,1>,_aux<int,char,int,float,int,double>>);
  static_assert(is_same_v<get_n_t<variant<>,0>,common::undefined>);
  static_assert(is_same_v<get_n_t<variant<int>,-100>,undefined>);
  static_assert(is_same_v<front_t<variant<>>,undefined>);
  static_assert(is_same_v<get_n_t<variant<int>,22>,undefined>);
  static_assert(is_same_v<get_n_t<variant<int>,0>,int>);
  static_assert(is_same_v<front_t<variant<int>>,int>);
  static_assert(is_same_v<get_n_t<variant<int,double,char>,1>,double>);
  static_assert(is_same_v<get_n_t<variant<int,double,char>,2>,char>);
  static_assert(is_same_v<front_t<variant<int,double,char>>,int>);
  static_assert(is_same_v<back_t<variant<int,double,char>>,char>);
  static_assert(is_same_v<back_t<variant<char>>,char>);
  static_assert(is_same_v<back_t<variant<>>,undefined>);
  
  static_assert(is_same_v<get_n_t<tuple<int,double,int*,int&,char&,int*>,3>,int&>);
  static_assert(is_same_v<get_n_t<tuple<int,double,int*,int&,char&,int*>,-3>,int&>);
  static_assert(is_same_v<get_n_t<tuple<int,double,int*,int&,char&,int*>,-2>,char&>);
  static_assert(is_same_v<get_n_t<tuple<int,double,int*,int&,char&,int*>,-1>,::std::add_pointer_t<int>>);
  static_assert(is_same_v<get_n_t<tuple<int,double,int*,int&,char&,int*>,-4>,int*>);
  static_assert(is_same_v<get_n_t<tuple<int,double,int*,int&,char&,int*>,-100>,undefined>);
  
  static_assert(get_n_v<char_sequence<'1','2','3'>,0> =='1');
  static_assert(get_n_v<char_sequence<'1','2','3'>,1> =='2');
  static_assert(get_n_v<char_sequence<'1','2','3'>,4> ==-1);
  static_assert(get_n_v<char_sequence<'1','2','3'>,3> ==-1);
  static_assert(get_n_v<char_sequence<>,-11> ==-1);
  static_assert(get_n_v<char_sequence<'1','2','3'>,2> =='3');
  static_assert(get_n_v<char_sequence<'1','2','3'>,-2> =='2');
  
  static_assert(get_n_v<bool_sequence<true,false,true>,2> == true);
  static_assert(get_n_v<bool_sequence<true,false,true>,-2> == false);
  static_assert(get_n_v<range_sequence<-3,-2,-1,0,1,2,3,4>,2> == -1);
  static_assert(get_n_v<range_sequence<-3,-2,-1,0>,-4> ==-3);
  static_assert(get_n_v<range_sequence<-3,-2,-1,0>,-1> ==0);
  static_assert(get_n_v<index_sequence<1,2,3,4,5>,4> ==5);
  static_assert(is_same_v<remove_cvref_t<tuple<int&>>,tuple<int&>>);
  
  static_assert(is_same_v<merge_t<int_sequence<1,4>,int_sequence<55>>,int_sequence<1,4,55>>);
  static_assert(is_same_v<merge_t<_aux<>,_aux<void>>,_aux<void>>);
  
  static_assert(is_same_v<unique_t<TypeList<>>,TypeList<>>);
  static_assert(is_same_v<unique_t<TypeList<int>>,TypeList<int>>);
  static_assert(is_same_v<unique_t<TypeList<int>>,TypeList<int>>);
  static_assert(is_same_v<unique_t<TypeList<int,void,void,int,char,float,float,char>>,TypeList<int,void,char,float>>);
  
  static_assert(is_unique_v<TypeList<index_constant<1>,index_constant<5>>>);
  static_assert(is_unique_v<TypeList<int>>);
  static_assert(is_unique_v<TypeList<int,void,int>> == false);
  static_assert(is_unique_v<TypeList<int,void,char>> );
  
  static_assert(is_unique_v<bool_sequence<>>);
  
  static_assert(is_unique_v<int_sequence<>>);
  static_assert(!is_unique_v<int_sequence<0,1,2,3,4,0>>);
  static_assert(is_unique_v<int_sequence<0,1,2,3,4>>);
  static_assert(is_unique_v<int_sequence<0,1,2,3,4>>);
  static_assert(!is_unique_v<int_sequence<1,2,3,4,0,0>>);
  
  static_assert(is_same_v<int_sequence<1,2,3,4,5>,
                reverse_t<int_sequence<5,4,3,2,1>>>);
  
  static_assert(is_same_v<int_sequence<1>,
                reverse_t<int_sequence<1>>>);
  
  static_assert(::std::is_same_v<reverse_t<variant<>>,variant<>>);
  static_assert(::std::is_same_v<reverse_t<const variant<int,char*>>,variant<char*,int>>);
  
  static_assert(is_same_v<variant<int,double>,append_t<variant<int>,double>>);
  static_assert(is_same_v<variant<double>,append_t<variant<>,double>>);
  static_assert(is_same_v<variant<double>,insert_t<variant<>,444,double>>);
  
  static_assert(is_same_v<insert_t<variant<int,double,char>,1,void>,
                variant<int,void,double,char>
                >);
  static_assert(is_same_v<insert_t<variant<int,double,char>,0,void>,
                variant<void,int,double,char>
                >);
  static_assert(is_same_v<insert_t<variant<int,double,char>,2,void>,
                variant<int,double,void,char>
                >);
  
  static_assert(is_same_v<insert_t<variant<int,double,char>,3,void>,
                variant<int,double,char,void>
                >);
  
  static_assert(is_same_v<insert_t<variant<int,double,char>,5,void>,
                variant<int,double,char,void>
                >);
  
  static_assert(is_same_v<insert_t<variant<>,5555,void,int>,
                variant<void,int>
                >);
  
  static_assert(is_same_v<insert_t<variant<double*>,5555,void,int>,
                variant<double*,void,int>
                >);
  
  static_assert(is_same_v<prepend_t<variant<int>,double>,variant<double,int>>);
  
  static_assert(is_same_v<variant<int,double,double>,
                insert_t<variant<int,double>,-1,double>>);
  
  static_assert(is_same_v<insert_t<variant<int,double,char>,1,void>,
                variant<int,void,double,char>
                >);
  static_assert(is_same_v<insert_t<variant<int,double,char>,0,void>,
                variant<void,int,double,char>
                >);
  static_assert(is_same_v<insert_t<variant<int,double,char>,2,void>,
                variant<int,double,void,char>
                >);
  
  static_assert(is_same_v<insert_t<variant<int,double,char>,3,void>,
                variant<int,double,char,void>
                >);
  
  static_assert(is_same_v<insert_t<variant<int,double,char>,111,void>,
                variant<int,double,char,void>
                >);
  
  static_assert(is_same_v<insert_t<variant<>,5555,void,int>,
                variant<void,int>
                >);
  
  static_assert(is_same_v<insert_t<variant<double*>,5555,void,int>,
                variant<double*,void,int>
                >);
  
  static_assert(is_same_v<prepend_t<variant<int>,double>,variant<double,int>>);
  
  
  static_assert(is_same_v<insert_t<variant<>,-1,char*>,variant<char*>>);
  
  static_assert(is_same_v<insert_t<variant<int,char>,-11,char*>,variant<char*,int,char>>);
  static_assert(is_same_v<insert_t<variant<int,char>,-1,char*>,variant<int,char*,char>>);
  static_assert(is_same_v<insert_t<variant<int,char>,-2,char*>,variant<char*,int,char>>);
  
  static_assert(is_same_v<insert_t<index_sequence<1,2,3,4,5>,0,index_sequence<>>,
                index_sequence<1,2,3,4,5>>);
  
  static_assert(is_same_v<
                insert_t<index_sequence<>,0,index_sequence<1,2,3,4,5>>,
                index_sequence<1,2,3,4,5>,
                insert_t<index_sequence<1,3,4,5>,1,index_sequence<2>>,
                insert_t<index_sequence<1,2,4,5>,2,index_sequence<3>>,
                insert_t<index_sequence<1,2,3,5>,3,index_sequence<4>>,
                insert_t<index_sequence<1,2,3,4>,4,index_sequence<5>>,
                insert_t<index_sequence<1,2,3,4>,5,index_sequence<5>>
                >);
  
  static_assert(is_same_v<
                insert_t<index_sequence<>,-1,index_sequence<2>>,
                insert_t<index_sequence<>,-2,index_sequence<2>>,
                index_sequence<2>
                >);
  
  static_assert(is_same_v<
                insert_t<index_sequence<1>,-1,index_sequence<2>>,
                insert_t<index_sequence<1>,-2,index_sequence<2>>,
                index_sequence<2,1>
                >);
  static_assert(common::is_same_v<_erase_t<index_sequence<1,2,3,4,5,6,7,8>,1000>,
    _erase_t<index_sequence<1,2,3,4,5,6,7,8>,8>,index_sequence<1,2,3,4,5,6,7,8>>);
  static_assert(::std::is_same_v<_erase_t<index_sequence<1,2,3,4,5,6,7,8>,1>,index_sequence<1,3,4,5,6,7,8>>);
  static_assert(::std::is_same_v<_erase_t<index_sequence<1,2,3,4,5,6,7,8>,0>,index_sequence<2,3,4,5,6,7,8>>);
  static_assert(::std::is_same_v<_erase_t<index_sequence<1,2,3,4,5,6,7,8>,1>,index_sequence<1,3,4,5,6,7,8>>);
  static_assert(::std::is_same_v<_erase_t<index_sequence<1,2,3,4,5,6,7,8>,7>,index_sequence<1,2,3,4,5,6,7>>);
  static_assert(::std::is_same_v<_erase_t<index_sequence<1,2,3,4,5,6,7,8>,-1>,index_sequence<1,2,3,4,5,6,7>>);
  static_assert(::std::is_same_v<_erase_t<index_sequence<1,2,3,4,5,6,7,8>,-2>,index_sequence<1,2,3,4,5,6,8>>);
  static_assert(::std::is_same_v<_erase_t<index_sequence<1,2,3,4,5,6,7,8>,-2>,index_sequence<1,2,3,4,5,6,8>>);
  static_assert(::std::is_same_v<_erase_t<index_sequence<1,2,3,4,5,6,7,8>,-3>,index_sequence<1,2,3,4,5,7,8>>);
  static_assert(::std::is_same_v<_erase_t<index_sequence<1,2,3,4,5,6,7,8>,-4>,index_sequence<1,2,3,4,6,7,8>>);
  static_assert(::std::is_same_v<_erase_t<index_sequence<1,2,3,4,5,6,7,8>,-5>,index_sequence<1,2,3,5,6,7,8>>);
  static_assert(::std::is_same_v<_erase_t<index_sequence<1,2,3,4,5,6,7,8>,-6>,index_sequence<1,2,4,5,6,7,8>>);
  static_assert(::std::is_same_v<_erase_t<index_sequence<1,2,3,4,5,6,7,8>,-7>,index_sequence<1,3,4,5,6,7,8>>);
  static_assert(::std::is_same_v<_erase_t<index_sequence<1,2,3,4,5,6,7,8>,-8>,index_sequence<2,3,4,5,6,7,8>>);
  static_assert(::std::is_same_v<_erase_t<index_sequence<1,2,3,4,5,6,7,8>,-88>,index_sequence<1,2,3,4,5,6,7,8>>);
  static_assert(is_same_v<_erase_t<variant<>,1>,variant<>,_erase_t<variant<>,-1>,_erase_t<variant<int>,0>>);
  static_assert(is_same_v<_erase_t<variant<int>,1>,variant<int>>);
  static_assert(is_same_v<_erase_t<variant<int,double>,1>,variant<int>,_erase_t<variant<int,double>,-1>>);
  static_assert(is_same_v<variant<>,_remove_t<variant<int>,int>,_remove_t<variant<>,int>>);
  static_assert(is_same_v<index_sequence<2,3,4,3,5,6,7>,_remove_t<index_sequence<1,2,1,3,4,3,5,6,7>,index_constant<1>>>);
  static_assert(count_v<_aux<int,double,char,float,int>,int> ==2);
  
  static_assert(count_v<index_sequence<8,1,1,1,2,3,4,1>,index_constant<1>> ==4);
  static_assert(count_if_v<index_sequence<1,2,3,4,5,6,7>,_less,index_constant<3>> ==2);
  static_assert(is_same_v<take_t<_aux<int,double,char>,1>,_aux<int>>);
  static_assert(is_same_v<take_t<_aux<int,double,char>,77>,_aux<int,double,char>>);
  static_assert(is_same_v<take_t<index_sequence<>,77>,index_sequence<>>);
  static_assert(is_same_v<take_t<index_sequence<111>,0>,index_sequence<>>);
  static_assert(is_same_v<take_t<index_sequence<1,11,77>,2>,index_sequence<1,11>>);
  
  static_assert(is_same_v<_aux<>,
                _insertion_sort_t<_aux<>,_less>>);
  
  static_assert(is_same_v<_aux<char>,
                _insertion_sort_t<_aux<char>,_less>>);
  
  static_assert(is_same_v<_aux<char,int,double>,
                _insertion_sort_t<_aux<int,double,char>,_less>>);
  
  static_assert(is_same_v<_aux<char,int,float,double>,
                _insertion_sort_t<_aux<int,double,char,float>,_less>>);
  
  static_assert(is_same_v<_aux<>,
                _insertion_sort_t<_aux<>,_greater>>);
  
  static_assert(is_same_v<_aux<char>,
                _insertion_sort_t<_aux<char>,_greater>>);
  
  static_assert(is_same_v<_aux<double,int,char>,
                _insertion_sort_t<_aux<int,double,char>,_greater>>);
  
  static_assert(is_same_v<_aux<double,int,float,char>,
                _insertion_sort_t<_aux<int,double,char,float>,_greater>>);
  
  static_assert(is_same_v<index_sequence<1>,_insertion_sort_t<index_sequence<1>,_less>>);
  static_assert(is_same_v<index_sequence<>,_insertion_sort_t<index_sequence<>,_less>>);
  
  static_assert(is_same_v<index_sequence<1,2,3,4,5,6,7,8,9>,
                _insertion_sort_t<index_sequence<9,8,6,7,5,4,2,3,1>,_less>>);
  
  static_assert(is_same_v<reverse_t<index_sequence<1,2,3,4,5,6,7,8,9>>,
                _insertion_sort_t<index_sequence<9,8,6,7,5,4,2,3,1>,_greater>>);
  
  #endif
}

#undef NON_STL_CXX_14
#undef NON_STL_CXX_17
#undef NON_STL_CXX_20
#undef NON_STL_17
#undef NON_STL_14
#undef NON_STL_20
#undef NON_STL_HAS_CXX
#undef INLINE
#endif //_META_HPP_
