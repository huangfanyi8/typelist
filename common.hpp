/*
 * copy_ref<A,B> 拷贝A的引用属性给B
 * copy_cv<A,B> 拷贝A的cv属性给B
 * switch_t 获取列表中第n个type，下标从0开始
 * similar<A,B>：如果A,B是模板，只有在相等ref与cv属性下返回true type，否则返回false type
 * map<T,Pred> 将Prd属性叠加给T
 * */



#ifndef COMMON_HPP
#define COMMON_HPP

#if __has_include(<cstddef>)
#include<cstddef>
#else
#include<stddef.h>
#endif

#include<utility>

#define NON_STL_CXX_14 201402L
#define NON_STL_CXX_17 201703L
#define NON_STL_CXX_20 202002L

#define NON_STL_HAS_CXX(VERSION)\
__cplusplus >= VERSION || (defined(_MSVC_LANG) && _MSVC_LANG >= VERSION)

#define NON_STL_14 NON_STL_HAS_CXX(NON_STL_CXX_14)
#define NON_STL_17 NON_STL_HAS_CXX(NON_STL_CXX_17)
#define NON_STL_20 NON_STL_HAS_CXX(NON_STL_CXX_20)

#if NON_STL_17
#define INLINE inline
#define PRIVATE_INLINE inline
#define AUTO(...) auto
#else
#define INLINE
#define PRIVATE_INLINE
#define AUTO(...) traits_value_t<__VA_ARGS__>
#endif

#if NON_STL_20
#define CONCEPT concept
#define CONSTEVAL consteval
#else
#define CONCEPT INLINE constexpr bool
#define CONSTEVAL constexpr
#endif

//constant sequence
namespace common
{
  enum class Error
  {
    S_error
  };
  
  //all attribute
  enum class TypeAttribute
  {
    S_lvalue_reference,
    S_rvalue_reference,
    S_pointer,
    S_cv,
    S_cvp,
    S_const,
    S_cp,
    S_volatile,
    S_vp
  };
  
  class undefined{};
  
  template<class  Type>
  struct type_identity
  {
    using type=Type;
  };
  
  template<class Value, Value...value>
  struct constant_sequence
  {
    using value_type = Value;
    using self=constant_sequence;
    static constexpr size_t size = sizeof...(value);

    template<template<class Other,Other...>class Template,class Other,Other...other>
    using rebind=Template<Other,other...>;
  };

  template<class Value, Value v>
  struct constant
  {
    using value_type = Value;
    using self=constant;
    static constexpr value_type value = v;
    constexpr value_type operator()() const { return value; }
    constexpr  explicit operator value_type() const { return value; }
    
    template<class Other,Other _v>
    using rebind=constant<Other,_v>;
  };

  template<bool v>
  using bool_constant=constant<bool,v>;
  
  using true_type=bool_constant<true>;
  using false_type=bool_constant<false>;
  
  template<size_t v>
  using index_constant=constant<size_t,v>;
  
  template<size_t v>
  using index_constant=constant<size_t,v>;

  using error_constant=constant<Error,Error::S_error>;
  
  template<size_t...v>
  using normal_sequence=constant_sequence<size_t,v...>;

  template<ptrdiff_t...v>
  using range_sequence=constant_sequence<ptrdiff_t ,v...>;

  template<bool...v>
  using bool_sequence=constant_sequence<bool,v...>;

  template<char...v>
  using char_sequence=constant_sequence<char,v...>;

  template<int...v>
  using int_sequence=constant_sequence<int,v...>;
}

//map_t ,  remove_cvref_t
namespace common
{
  template<class Type, template<class >class...Pred>
  struct map
    :type_identity<Type>
  {};
  
  template<class Type, template<class >class Pred>
  struct map<Type,Pred>
    :Pred<Type>
  {};
  
  template<class Type,
    template<class >class HeadP,
    template<class >class ...RestP>
  struct map<Type,HeadP,RestP...>
    :map<typename HeadP<Type>::type,RestP...>
  {};
  
  template<template<class...>class Template,
    template<class >class Pred,
    class...Types>
  struct map<Template<Types...>,Pred>
    :type_identity<Template<typename Pred<Types>::type...>>
  {};
  
  template<template<class...>class Template,
    template<class >class HeadP,
    template<class >class ...RestP,class...Types>
  struct map<Template<Types...>,HeadP,RestP...>
    :map<typename map<Template<Types...>,HeadP>::type,RestP...>
  {};
  
  template<class Type, template<class >class...Pred>
  using map_t=typename map<Type,Pred...>::type;
  
  template<class T>
  struct remove_cvref
    :std::remove_cv<std::remove_reference_t<T>>
  {};
  
  template<class Type>
  using remove_cvref_t=typename remove_cvref<Type>::type;
}

//traits
namespace common
{
  template<class T>
  PRIVATE_INLINE constexpr bool _is_template_v = false;
  
  template<template<class...> class T, class...Types>
  PRIVATE_INLINE constexpr bool _is_template_v<T<Types...>> = true;
  
  template<template<class Value,Value...>class T, class Value, Value...v>
  PRIVATE_INLINE constexpr bool _is_template_v<T<Value,v...>> = true;
  
  template<class>
  struct _meta_traits
  {
    static_assert(false,"T must be a variadic template");
  };
  
  template<template<class ...> class Template, class...Types>
  struct _meta_traits<Template<Types...>>
    : type_identity<Template<>>,
      index_constant<sizeof...(Types)>
  {
    static constexpr bool S_normal = true;
  };
  
  template<template<class Value, Value...> class Template, class Value, Value...value>
  struct _meta_traits<Template<Value, value...>>
    : type_identity<Template<Value>>,
      constant<Value,sizeof...(value)>
  {
    static constexpr bool S_normal = false;
  };
  
  template<class T>
  struct meta_traits
    : _meta_traits<remove_cvref_t<T>>
  {};

  template<class List>
  using make_empty_t = typename meta_traits<remove_cvref_t<List>>::type;
  
  template<class List>
  INLINE constexpr bool is_empty_v = !meta_traits<List>::value;
  
  template<class List>
  INLINE constexpr bool is_normal_v = meta_traits<List>::S_normal;
  
  template<class Template>
  using traits_value_t = typename meta_traits<Template>::value_type;
  
  template<class Template>
  INLINE constexpr auto extent_v = meta_traits<Template>::value;
  
  template<class T>
  INLINE constexpr bool is_template_v = _is_template_v<remove_cvref_t<T>>;

}

//switch , get_n
namespace common
{
  template<std::ptrdiff_t index,class...Types>
  struct Switch
  {
  private:
    static constexpr int S_index_v=index>=0?index:sizeof...(Types)+index;
  private:
    template<std::ptrdiff_t c,class,class...>
    struct Impl
      :type_identity<undefined>
    {};
    
    template<class Head,class...Rest>
    struct Impl<0,void,Head,Rest...>
      :type_identity<Head>
    {};
    
    template<class Head,class Second,class...Rest>
    struct Impl<1,void,Head,Second,Rest...>
      :type_identity<Second>
    {};
    
    template<class Head,class Second,class Third,class...Rest>
    struct Impl<2,void,Head,Second,Third,Rest...>
      :type_identity<Third>
    {};
    
    template<class Head,class Second,class Third,class Fourth,class...Rest>
    struct Impl<3,void,Head,Second,Third,Fourth,Rest...>
      :type_identity<Fourth>
    {};
    
    template<std::ptrdiff_t c,template<class...>class List,
      class Head,class Second,class Third,class Fourth,class Five,class...Rest>
    struct Impl<c,std::enable_if_t<(c>=4)>,List<Head,Second,Third,Fourth,Five,Rest...>>
      :Impl<c-4,void,List<Five,Rest...>>
    {};
  public:
    using type=typename Impl<S_index_v,void,Types...>::type;
  };
  
  
  template<std::ptrdiff_t Idx,class...Types>
  using switch_t=typename Switch<Idx,void,Types...>::type;
}

//copy
namespace common
{
  template<class Input, class Output,
    bool Const = std::is_const<Input>::value,
    bool Volatile = std::is_volatile<Input>::value>
  struct copy_cv
          :std::conditional_t<Const,
                  std::conditional_t<Volatile, std::add_cv<Output>, std::add_const<Output>>,
                  std::conditional_t<Volatile, std::add_volatile< Output>, type_identity<Output>>
          >
  {};

  template<class Input,class Output,
    bool R=std::is_rvalue_reference_v<Input>,
    bool L=std::is_lvalue_reference_v<Input>>
  struct copy_ref
          :std::conditional_t<R,std::add_rvalue_reference<Output>,
          std::conditional_t<L,std::add_lvalue_reference<Output>,type_identity<Output>>
          >
  {};

  template<class Input,class Output>
  struct copy_cvref
    :copy_ref<Input,typename copy_cv<std::remove_reference_t<Input>,Output>::type>
  {};

  template<class Input, class Output>
  using copy_cv_t = typename copy_cv<Input, Output>::type;

  template<class Input, class Output>
  using copy_ref_t = typename copy_ref<Input, Output>::type;

  template<class Input, class Output>
  using copy_cvref_t=typename copy_cvref<Input,Output>::type;

}

//is_same
namespace common
{
  template<class,class...>
  struct is_same
          :std::true_type
  {};

  template<class L,class R>
  struct is_same<L,R>
          :std::is_same<L,R>
  {};

  template<class A,class B,class...Rest>
  struct is_same<A,B,Rest...>
          :std::bool_constant<is_same<A,Rest...>::value&&is_same<B,Rest...>::value>
  {};
#if NON_STL_17
  template<class Head,class...Rest>
  INLINE constexpr bool is_same_v=(std::is_same_v<Head,Rest>&&...);
#else
  template<class H,class...Rest>
  INLINE constexpr bool is_same_v=is_same<H,Rest...>::value;
#endif
}

//Logic predicate
namespace common
{
  struct LogicPredicateBase
  {
    enum Type
    {
      logic_and,
      logic_or,
    };
  };
  
  template<bool...r>
  struct LogicPredicate
    :private LogicPredicateBase
  {
  private:
    template<Type type,bool...v>
    struct Impl
      :std::conditional_t<type==logic_and,std::true_type,std::false_type>
    {};
    
    template<Type type,bool head,bool...rest>
    struct Impl<type,head,rest...>
      :std::conditional_t<head,
        std::conditional_t<type==logic_and,Impl<type,rest...>,std::true_type>,
        std::conditional_t<type==logic_and,std::false_type,Impl<type,rest...>>>
    {};
    
    template<bool...>
    struct Not
      :std::true_type
    {};
    template<bool h,bool...rest>
    struct Not<h,rest...>
      :std::conditional_t<h,std::false_type,typename Not<rest...>::type>
    {};
  public:
    using and_t=typename Impl<logic_and,r...>::type;
    using or_t=typename Impl<logic_or,r...>::type;
    using not_t=typename Not<r...>::type;
  };
  
  #if NON_STL_17
  //conjunction_v
  template<bool...Requires>
  INLINE constexpr bool conjunction_v = (Requires&&...&&true);

  //disjunction_v
  template<bool...Requires>
  INLINE constexpr bool disjunction_v = (Requires||...||false);

  //negation_v
  template<bool F,bool...R>
  INLINE constexpr bool negation_v= (!R&&...&&!F);
  
  #else
  //conjunction_v
  template<bool...Requires>
  INLINE constexpr bool conjunction_v = LogicPredicate<Requires...>::and_t::value;
  
  //disjunction_v
  template<bool...Requires>
  INLINE constexpr bool disjunction_v = LogicPredicate<Requires...>::or_t::value;
  
  //negation_v
  template<bool F,bool...R>
  INLINE constexpr bool negation_v= LogicPredicate<F,R...>::not_t::value;
  #endif
}

//similar reference cv attribute
namespace common
{
  enum ReferenceType
  {
    S_lvalue_reference,
    S_rvalue_reference,
    S_none
  };
  
  template<class T>
  INLINE constexpr
  ReferenceType _reference_cast
    =std::is_lvalue_reference<T>::value
     ?S_lvalue_reference
     :std::is_rvalue_reference<T>::value
      ?S_rvalue_reference
      :S_none;
  
  template<class Left,class Right,
    auto l=_reference_cast<Left>,
    auto r=_reference_cast<Right>>
  struct _is_similar
  {
  private:
    using L=std::remove_reference_t<Left>;
    using R=std::remove_reference_t<Right>;
    
    static constexpr bool l_c=
      std::is_const<L>::value;
    
    static constexpr bool r_c=
      std::is_const<R>::value;
    
    static constexpr bool l_v=
      std::is_volatile<L>::value;
    
    static constexpr bool r_v=
      std::is_volatile<R>::value;
    
  public:
    static constexpr bool value=
      (l==r)
      &&(l_c==r_c)
      &&(l_v==r_v);
  };
  
  template<class Left,class Right>
  struct is_similar_helper
    :std::false_type
  {};
  
  template<template<class ...>class Left,
    template<class ...>class Right,
    class...A,class...B>
  struct is_similar_helper<Left<A...>,Right<B...>>
    :std::true_type
  {};
  
  template<template<class A,A...>class Left,
    template<class B,B...>class Right,
    class A,class B,
    A...a,B...b>
  struct is_similar_helper<Left<A,a...>,Right<B,b...>>
    :std::true_type
  {};
  
  template<template<class A,A...>class Left,
    template<class B,B...>class Right,
    class A,class B>
  struct is_similar_helper<Left<A>,Right<B>>
    :std::true_type
  {};
  
  template<class Left,class Right>
  struct is_similar
    :bool_constant<
    is_similar_helper<remove_cvref_t<Left>,remove_cvref_t<Right>>::value
    && _is_similar<Left,Right>::value
    >
  {};
  
  template<class L,class R>
  INLINE constexpr bool is_similar_v=is_similar<L,R>::value;
}

//is_specialization
namespace common
{
  template<template<class...>class Template,class...>
  struct is_specialization
          :std::false_type
  {};

  template<template<class...>class Template, class...Types>
  struct is_specialization<Template,Template<Types...>>
          :std::true_type
  {};

  template<template<class...>class Template, class...Types>
  inline constexpr bool is_specialization_v = is_specialization<Template, Types...>::value;
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
    struct Impl_base<L,R,std::void_t<std::decay_t<decltype(false?std::declval<L>():std::declval<R>())>>>
      :std::decay<decltype(false?std::declval<L>():std::declval<R>())>
    {};
    
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

//cxx23 common_reference_t
namespace common
{
  template<class T,class U,
    template<typename> class TQ, template<typename> class UQ>
  struct basic_common_reference
    #if NON_STL_20
      :std::basic_common_reference<T,U,TQ,UQ>
    #endif
  { };
}

namespace common::common_reference_detail
{
  template<class T>
  INLINE constexpr bool S_exists_v=!std::is_same<remove_cvref<T>,undefined>::value;
  
  template<class T,ReferenceType v=_reference_cast<T>>
  struct xref
  {
    template<class U>
    using type=std::conditional_t<v==S_none,copy_cv_t<T,U>,
    std::conditional_t<v==S_lvalue_reference,
      copy_cv_t<T,U>&,
    copy_cv_t<T,U>&&>>;
  };
  
  template<typename Tp1, typename Tp2>
  using basic_common_ref
    = typename basic_common_reference<remove_cvref_t<Tp1>,
  remove_cvref_t<Tp2>,
  xref<Tp1>::template type,
  xref<Tp2>::template type>::type;
  
  template<class X,class Y,class=void>
  struct cond_res
    :type_identity<undefined>
  {};
  
  template<class X,class Y>
  struct cond_res<X,Y,std::void_t<decltype(false ? std::declval<X(&)()>()() :std::declval<Y(&)()>()())>>
    :type_identity<decltype(false ? std::declval<X(&)()>()() : std::declval<Y(&)()>()())>
  {};
  
  template<class X,class Y>
  using cond_res_t=typename cond_res<X,Y>::type;
  
  template<class X,class Y>
  using cond_res_cvref_t
    = cond_res_t<copy_cv_t<X,Y>&,copy_cv_t<Y,X>&>;
  
  template<class A,class B>
  struct SimpleCommonReference
  {
  private:
    template<class L,class R,
      ReferenceType=_reference_cast<L>,
    ReferenceType=_reference_cast<R>>
    struct Impl
      :type_identity<undefined>
    {};
    
    template<class L,class R>
    struct Impl<L,R,S_lvalue_reference,S_lvalue_reference>
    {
    private:
      using X=map_t<L,std::remove_reference>;
      using Y=map_t<R,std::remove_reference>;
    private:
      using _type=cond_res_cvref_t<X,Y>;
    public:
      constexpr static bool S_exist
        =conjunction_v<std::is_reference<_type>::value,S_exists_v<_type>>;
      using type=std::conditional_t<S_exist,_type,undefined>;
    };
    
    template<class L,class R>
    struct Impl<L,R,S_rvalue_reference,S_rvalue_reference>
    {
    private:
      using X=map_t<L,std::remove_reference>;
      using Y=map_t<R,std::remove_reference>;
    
    private:
      using _type=typename Impl<X&,Y&>::type;
      using C=map_t<_type,std::remove_reference>;
    public:
      using type=std::conditional_t<
        conjunction_v<std::is_convertible<L,C>::value,
        std::is_convertible<R,C>::value>,C,undefined>;
    };
    
    template<class L,class R>
    struct  Impl<L,R,S_rvalue_reference,S_lvalue_reference>
    {
    private:
      using X=map_t<L,std::remove_reference>;
      using Y=map_t<R,std::remove_reference>;
    
    private:
      using D = typename Impl<const X&, Y&>::type;
    public:
      using type=std::conditional_t<std::is_convertible<L,D>::value,D,undefined>;
    };
    
    template<class L,class R>
    struct  Impl<L,R,S_lvalue_reference,S_rvalue_reference>
      :Impl<R,L,S_rvalue_reference,S_lvalue_reference>
    {};
  
  public:
    using type=typename Impl<A,B>::type;
  };
  
  template<class...Types>
  struct common_reference
  {
  private:
    ///sizeof...(T)==0
    template<class...>
    struct Impl
    {};
    
    template<class Type>
    struct Impl<Type>
      :type_identity<Type>
    {};
    
    template<class A,class B>
    struct Impl<A,B>
    {
    private:
      template<class L,class R,int cond=1,class=void>
      struct Two
        :Two<L,R,cond+1>
      {};
      
      template<class L,class R>
      struct Two<L,R,1,std::enable_if_t<
        conjunction_v<std::is_reference<L>::value,
        std::is_reference<R>::value
        >
        >
        >
      {
        private:
        using S=typename SimpleCommonReference<L,R>::type;
        public:
        using type=std::conditional_t<
          conjunction_v<std::is_convertible<map_t<L,std::add_pointer>,
            map_t<S,std::add_pointer>>::value,
        std::is_convertible<map_t<L,std::add_pointer>,map_t<S,std::add_pointer>>::value>,
        S,typename Two<L,R,2>::type>;
        
      };
      
      template<class L,class R>
      struct Two<L,R,2,std::void_t<typename basic_common_reference<remove_cvref_t<L>,
        remove_cvref_t<R>,
        xref<L>::template type,
        xref<R>::template type>::type>>
      :basic_common_reference<remove_cvref_t<L>,
      remove_cvref_t<R>,
      xref<L>::template type,
      xref<R>::template type>
      {};
      
      template<class L,class R>
      struct Two<L,R,3,std::void_t<cond_res_t<L,R>>>
        :cond_res<L,R>
      {};
      
      template<class L,class R>
      struct Two<L,R,4,std::void_t<common_type_t<L,R>>>
        :common_type<L,R>
      {};
      
      template<class L,class R>
      struct Two<L,R,5>
      {};
    public:
      using type=typename Two<A,B>::type;
    };
    
    template<class A,class B,class C,class...Rest>
    struct Impl<A,B,C,Rest...>
      :Impl<typename Impl<A,B>::type,C,Rest...>
    {};
  
  public:
    using type=typename Impl<Types...>::type;
  };
}

namespace common
{
  template<class...Types>
  struct common_reference
    :common::common_reference_detail::common_reference<Types...>
  {};
  
  template<class...Types>
  using common_reference_t=typename common_reference<Types...>::type;
}

namespace common
{
  template<class...>class _aux{};
  
  template<template<class...>class Template,class...T>
  CONSTEVAL auto extent(const Template<T...>&)
  {
    return sizeof...(T);
  }
  
  template<ptrdiff_t Idx,class T>
  #if NON_STL_20
  requires (is_template_v<T>)
  #endif
  INLINE constexpr auto _conversion_v=Idx>=0
    ?Idx
    :Idx+ptrdiff_t(extent_v<T>);
  
  template<class List,ptrdiff_t Idx,ptrdiff_t Extent=ptrdiff_t(extent_v<List>),bool v=(Idx>0)>
  struct _out_of_range
    :bool_constant<(Idx>0)?(Idx>Extent):(Idx<-Extent)>
  {};
  
  template<class T,ptrdiff_t Idx>
  INLINE constexpr bool _is_out_of_range=_out_of_range<T,Idx>::value;
}

namespace common
{
  template<class T, template<class,class>class Traits>
  struct is_satisfied
    :std::true_type
  {};
  
  template<class T, class,template<class,class>class Traits>
  struct _is_satisfied
    :false_type
  {};
  
  template<template<class...>class TL,class T,class...Types,template<class,class>class Traits>
  struct _is_satisfied<TL<Types...>,T,Traits>
    :bool_constant<(Traits<T,Types>::value&&...)>
  {};
  
  template<template<class...>class TL,class A,class...Rest,template<class,class>class Traits>
  struct is_satisfied<TL<A,Rest...>,Traits>
    :std::conditional_t<_is_satisfied<TL<Rest...>,A,Traits>::value,
    is_satisfied<TL<Rest...>,Traits>,
    false_type>
  {};
  
  static_assert(is_satisfied<_aux<int,int,int>,is_same>::value);
}

namespace common
{
  #if NON_STL_20
  
  template<class T>
  concept UnaryPred=requires
  {
    T::value;
    std::convertible_to<decltype(T::value),bool>;
  };
  #endif
  
  template<class,class=void>
  struct _is_constant
    :false_type
  {};
  
  template<template<class V,V>class Constant,
  class V,V v>
  struct _is_constant<Constant<V,v>,std::void_t<typename Constant<V,v>::self>>
    :true_type
  {};
  
  template<class T>
  struct is_constant
    :_is_constant<remove_cvref_t<T>>
  {};
}
#endif
