/*c++ 14 c++17 c++20实现的轻量级元编程库，
 * 包含对标准库的扩展以及type_list相关的多种算法
 * */

#ifndef DONG_DONG_COMMON_HPP
#define DONG_DONG_COMMON_HPP

#include<type_traits>
#include<cstddef>

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

//Constant
namespace common
{
  enum class Error
  {S_error};
  
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
  
  class undefined{};
  
  template<class...>class _aux
  {};
  
  template<class  Type>
  struct type_identity
  {using type=Type;};
  
  template<class ValueType,ValueType...v>
  struct constant_sequence
  {
    using value_type = ValueType;
    using self=constant_sequence;
    static constexpr size_t value = sizeof...(v);
    static constexpr size_t sequence = true;
  };
  
  template<class Value, Value v>
  struct constant
  {
    using value_type = Value;
    using self=constant;
    static constexpr value_type value = v;
    static constexpr size_t sequence = false;
    constexpr value_type operator()() const { return value; }
    constexpr  explicit operator value_type() const { return value; }
  };
  
  template<class Constant,ptrdiff_t Dis>
  struct _advance
  {};
  
  template<class Type,Type v,ptrdiff_t Dis>
  struct _advance<constant<Type,v>,Dis>
    :type_identity<constant<Type,v+static_cast<Type>(Dis)>>
  {};
  
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
  using range_constant=constant<ptrdiff_t ,v>;
  
  template<size_t v>
  using index_constant=constant<size_t,v>;
  
  using error_constant=constant<Error,Error::S_error>;
  
  template<size_t...v>
  using index_sequence=constant_sequence<size_t,v...>;
  
  template<ptrdiff_t...v>
  using range_sequence=constant_sequence<ptrdiff_t,v...>;
  
  template<bool...v>
  using bool_sequence=constant_sequence<bool,v...>;
  
  template<char...v>
  using char_sequence=constant_sequence<char,v...>;
  
  template<int...v>
  using int_sequence=constant_sequence<int,v...>;
  
  template<bool v>
  using bool_constant=constant<bool,v>;
  
  using true_type=bool_constant<true>;
  using false_type=bool_constant<false>;
}
//map_t , remove_cvref_t
namespace common
{
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
  
  template<class T>
  struct remove_cvref
    :map<T,std::remove_reference,std::remove_cv>
  {};
  
  template<class Type,template<class >class...Pred>
  using map_t=typename map<Type,Pred...>::type;
  
  template<class Type>
  using remove_cvref_t=typename remove_cvref<Type>::type;
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
//switch
namespace common
{
  template<class Void,std::ptrdiff_t index,class...Types>
  struct _switch
  {
  private:
    static constexpr int S_index_v=index>=0?index:sizeof...(Types)+index;
  private:
    template<class,std::ptrdiff_t c,class...>
    struct Impl
      :type_identity<undefined>
    {};
    
    template<class Head,class...Rest>
    struct Impl<void,0,Head,Rest...>
      :type_identity<Head>
    {};
    
    template<std::ptrdiff_t c,class Head,class Second,class...Rest>
    struct Impl<std::enable_if_t<(c>0)>,c,Head,Second,Rest...>
      :Impl<void,c-1,Second,Rest...>
    {};
  public:
    using type=typename Impl<void,S_index_v,Types...>::type;
  };
  template<int Idx,class...Types>
  using switch_t=typename _switch<void,Idx,Types...>::type;
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
  
  template<class Value, Value v>
  PRIVATE_INLINE constexpr bool _is_template_v<constant<Value,v>> = false;
  
  template<class Value, Value v>
  PRIVATE_INLINE constexpr bool _is_template_v<std::integral_constant<Value,v>> = false;
  
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
  
  template<class T>
  struct meta_traits
    : _meta_traits<remove_cvref_t<T>>
  {};
  
  template<class List>
  using make_empty_t = typename meta_traits<remove_cvref_t<List>>::type;
  
  template<class List>
  using make_empty = type_identity<make_empty_t<List>>;
  
  template<class List>
  INLINE constexpr bool is_empty_v = !meta_traits<List>::size;
  
  template<class List>
  INLINE constexpr bool is_normal_v = meta_traits<List>::S_normal;
  
  template<class Template>
  using traits_value_t = typename meta_traits<Template>::value_type;
  
  template<class Template>
  INLINE constexpr auto extent_v = meta_traits<Template>::size;
  
  template<class T>
  INLINE constexpr bool is_template_v = _is_template_v<remove_cvref_t<T>>;
}
//Predicate
namespace common
{
  template<class,class...>
  struct is_same
    :true_type
  {};
  
  template<class L,class R>
  struct is_same<L,R>
    :std::is_same<L,R>
  {};
  
  template<class A,class B,class...Rest>
  struct is_same<A,B,Rest...>
    :bool_constant<is_same<A,Rest...>::value&&is_same<B,Rest...>::value>
  {};
  
  //0:and 1:or 2:not
  template<int type,class...>
  struct _logic_pred
    :std::conditional_t<type==1,false_type,true_type>
  {};
  
  template<int type,class H,class...R>
  struct _logic_pred<type,H,R...>
    :switch_t<type,
    std::conditional_t<H::value,_logic_pred<type,R...>,false_type>,
    std::conditional_t<H::value,true_type,_logic_pred<type,R...>>,
    std::conditional_t<H::value,false_type,_logic_pred<type,R...>>>
  {};
  
  template<template<class...>class Template,class...>
  struct is_specialization
    :false_type
  {};
  
  template<template<class...>class Template, class...Types>
  struct is_specialization<Template,Template<Types...>>
    :true_type
  {};
  
  template<class List,ptrdiff_t Idx,ptrdiff_t Extent=ptrdiff_t(extent_v<List>)>
  struct _out_of_range
    :bool_constant<(Idx>=0)?(Idx>=Extent):(Idx<-Extent)>
  {};

  template<class H,class...R>
  using conjunction=_logic_pred<0,H,R...>;
  template<class H,class...R>
  using disjunction=_logic_pred<1,H,R...>;
  template<class H,class...R>
  using negation=_logic_pred<2,H,R...>;
  
  //判断是否具有一样的cv属性，不包括指针
  template<class L,class R,bool v=(_reference_cast<L> ==_reference_cast<R>)>
  using is_similar =bool_constant<(std::is_const<L>::value==std::is_const<R>::value)
                                  &&(std::is_volatile<L>::value==std::is_volatile<R>::value)
                                  &&v
                                  &&is_normal_v<L> ==is_normal_v<R>>;
  
  template<class L,class R>
  INLINE constexpr bool is_similar_v=is_similar<L,R>::value;
  
  template<class H,class...R>
  INLINE constexpr bool conjunction_v = conjunction<H,R...>::value;
  
  template<class H,class...R>
  INLINE constexpr bool disjunction_v = disjunction<H,R...>::value;
  
  template<class H,class...R>
  INLINE constexpr bool negation_v= negation<H,R...>::value;
  
  template<template<class...>class Template, class...Types>
  INLINE constexpr bool is_specialization_v = is_specialization<Template, Types...>::value;
  
  template<ptrdiff_t Idx,class T>
  INLINE constexpr auto _conversion_v=Idx>=0?Idx:Idx+ptrdiff_t(extent_v<T>);
  
  template<class T,ptrdiff_t Idx>
  INLINE constexpr bool _is_out_of_range=_out_of_range<T,Idx>::value;
  
  template<class H,class...Rest>
  INLINE constexpr bool is_same_v=is_same<H,Rest...>::value;
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
//Common Reference
namespace common
{
  //CPO class
  template<class T,class U,
    template<class> class TQ, template<class> class UQ>
  struct basic_common_reference
  { };
  
  template<class T>
  INLINE constexpr bool S_exists_v=!std::is_same<remove_cvref<T>,undefined>::value;
  
  namespace common_reference_detail
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
          =conjunction_v<std::is_reference<_type>,bool_constant<S_exists_v<_type>>>;
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
          conjunction_v<std::is_convertible<L,C>,
            std::is_convertible<R,C>>,C,undefined>;
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
          conjunction_v<std::is_reference<L>,
            std::is_reference<R>>>>
        {
        private:
          using S=typename SimpleCommonReference<L,R>::type;
        public:
          using type=std::conditional_t<
            conjunction_v<std::is_convertible<map_t<L,std::add_pointer>,
              map_t<S,std::add_pointer>>,
              std::is_convertible<map_t<L,std::add_pointer>,map_t<S,std::add_pointer>>>,
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
  
  template<class...Types>
  struct common_reference
    :common::common_reference_detail::common_reference<Types...>
  {};
  
  template<class...Types>
  using common_reference_t=typename common_reference<Types...>::type;
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
  private:
    template<class...>
    struct Impl
      :type_identity<undefined>
    {};
    
    template<template<class...>class Template,class...Types>
    struct Impl<Template<Types...>>
      :type_identity<Template<Types...>>
    {};
    
    template<template<class Value,Value...>class Template,class Value,Value...value>
    struct Impl<Template<Value,value...>>
      :type_identity<Template<Value,value...>>
    {};
    
    template<template<class...>class Template,class...P,class...O>
    struct Impl<Template<P...>,Template<O...>>
      :type_identity<Template<P...,O...>>
    {};
    
    template<template<class Value,Value...>class Template,class Value,Value...P,Value...O>
    struct Impl<Template<Value,P...>,Template<Value,O...>>
      :type_identity<Template<Value,P...,O...>>
    {};
    
    template<class T,class U,class...Other>
    struct Impl<T,U,Other...>
      :Impl<typename Impl<T,U>::type,Other...>
    {};
  public:
    using type=typename Impl<Head,Rest...>::type;
  };
  
  template<class...Lists>
  using merge=_merge<remove_cvref_t<Lists>...>;
  
  template<class...Lists>
  using merge_t=typename merge<Lists...>::type;
}
//filter
namespace common
{
  enum struct Category
  {
    S_satisfied,
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
  
  template<class T,template<class,class...>class Traits>
  struct _filter<Category::S_satisfied,T,Traits,std::enable_if_t<!is_template_v<T>>>
    :false_type
  {};
  
  template<class T,template<class,class>class Traits>
  struct _filter<Category::S_satisfied,T,Traits,std::enable_if_t<(extent_v<T> <= 1)>>
    :true_type
  {};
  
  template<template<class...>class T,
    template<class,class>class Traits,
    class H,class S,class...R>
  struct _filter<Category::S_satisfied,T<H,S,R...>,Traits,void>
    :std::conditional_t<conjunction_v<Traits<H,S>,Traits<H,R>...>,
    _filter<Category::S_satisfied,T<S,R...>,Traits,void>,false_type>
  {};
  
  template<template<class,class>class BinaryPred,class...Types>
  struct is_all
    :_filter<Category::S_satisfied,_aux<Types...>,BinaryPred>
  {};
  
  struct _common_pred
  {
    template<ptrdiff_t Idx,template<class,class...>class,class,class...>
    struct _first_pos
      :constant<ptrdiff_t,-1>
    {};
    
    template<ptrdiff_t Idx,template<class,class...>class T,template<class...>class L,class H,class...R,class...Other>
    struct _first_pos<Idx,T,L<H,R...>,Other...>
      :std::conditional_t<T<H,Other...>::value,constant<ptrdiff_t,Idx>,
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
    struct _traits<Sequence,V,_aux<constant<V,v>...>>
      :type_identity<Sequence<V,v...>>
    {};
    
    template<template<class V,V...>class S,class V,class TL>
    using _traits_t=typename _traits<S,V,TL>::type;
  };
}
//unique
namespace common
{
  template<template<class...>class T,class H,class...R,class O,template<class,class>class Traits,class Bool>
  struct _filter<Category::S_unique,T<H,R...>,Traits,O,Bool>
    :std::conditional_t<Traits<H,O>::value,
    _filter<Category::S_unique,T<R...>,Traits,O,false_type>,
      _filter<Category::S_unique,T<R...>,Traits,merge_t<O,T<H>>,true_type>>
  {};
  
  template<class T,class O,template<class,class>class Traits,class Bool>
  struct _filter<Category::S_unique,T,Traits,O,Bool>
    :Bool,type_identity<O>
  {};
  
  template<class TL,class=std::enable_if_t<is_template_v<TL>>>
  struct _unique
    :private _common_pred
  {
  private:
    using _base=_filter<Category::S_unique,TL,_first,make_empty_t<TL>,true_type>;
  public:
    static constexpr bool value=_base::value;
    using type=typename _base::type;
  };
  
  template<template<class V,V...v>class TL,class V,V...v>
  struct _unique<TL<V,v...>>
    :private _common_pred
  {
  private:
    using _sequence=_aux<constant<V,v>...>;
    using _base=_filter<Category::S_unique,remove_cvref_t<_sequence>,_first,_aux<>,true_type>;
    using _type=copy_cvref_t<_sequence,typename _base::type>;
  public:
    static constexpr bool value=_base::value;
    using type=_traits_t<TL,V,_type>;
  };
}
///reverse
namespace common
{
  template<class List,bool=is_template_v<List>>
  class reverse
  {
  private:
    template<class T,class E>
    struct Impl
      :type_identity<E>
    {};
    
    template<template<class...> class Template,class E,class Head,class...Rest>
    struct Impl<Template<Head,Rest...>,E>
      :Impl<Template<Rest...>,merge_t<Template<Head>,E>>
    {};
    template<template<class Value,Value...> class Template,class Empty,class Value,Value Head,Value...Rest>
    struct Impl<Template<Value,Head,Rest...>,Empty>
      :Impl<Template<Value,Rest...>,merge_t<Template<Value,Head>,Empty>>
    {};
  private:
    using _type=typename Impl<remove_cvref_t<List>,make_empty_t<List>>::type;
  public:
    using type=copy_cvref_t<List,_type>;
  };
  
  template<class Template>
  using reverse_t=typename reverse<Template>::type;
}
//get_n
namespace common
{
  template<class T,/*TypeList*/
    std::ptrdiff_t Idx,/*index sub*/
    class ErrorType/*error type*/
  >
  #if NON_STL_20
  requires (is_template_v<T>)
  #endif
  class _get_n
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
    using type=typename Impl<S_idx,T>::type;
  };
  
  template<class T,/*TypeList*/
    std::ptrdiff_t Idx,/*index sub*/
    class ErrorType/*error type*/
  >
  class get_n
    :public _get_n<remove_cvref_t<T>,Idx,ErrorType>
  {};
  
  template<class T,/*TypeList*/
    std::ptrdiff_t Idx,/*index sub*/
    class ErrorType=undefined/*error type*/
  >
  using get_n_t=typename get_n<T,Idx,ErrorType>::type;
  
  template<class T,/*TypeList*/
    std::ptrdiff_t Idx,/*index sub*/
    class ErrorType=error_constant, /*error type*/
    class=std::enable_if_t<!is_normal_v<T>>
  >
  INLINE constexpr auto get_n_v=get_n<T,Idx,ErrorType>::type::value;
  
  template<class Template>
  using front_t = get_n_t<Template,0>;
  
  template<class Template>
  using back_t = get_n_t<Template,-1>;
  
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
    using type=_traits_t<Sequence,ValueType,typename _take<_aux<constant<ValueType,value>...>,Idx>::type>;
  };
}
//splice
namespace common
{
  struct in_place_t{explicit in_place_t()=default;};
  template<size_t>struct in_place_index_t{explicit in_place_index_t()=default;};
  template<class>struct in_place_type_t{explicit in_place_type_t()=default;};
  
  INLINE constexpr auto in_place=in_place_t{};
  template<class T>INLINE constexpr auto in_place_type=in_place_type_t<T>{};
  template<size_t Idx>INLINE constexpr auto in_place_index=in_place_index_t<Idx>{};
  
  template<class >
  struct list
  {};
  
  template<template<class VT,VT...>class Sequence,class ValueType,ValueType...value>
  struct list<Sequence<ValueType,value...>>
  {
    constexpr auto splice(int,int)
    {}
    
    constexpr auto splice(in_place_t,int)
    {}
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
    :_equal_range<_aux<constant<V,v>...>,Traits,P...>
  {};
}
/* erase_if
 * insert_if
 * remove
 * */
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
    using _sequence=_aux<constant<V,v>...>;
    using _add=_aux<constant<V,Add>...>;
    using _base=typename _filter<Category::S_insert_if,_sequence,Pred,_add,Bool,Para...>::type;
  public:
    using type=_traits_t<List,V,_base>;
  };
  
  template<class Template,template<class,class...>class Predicate,
    class AddList,class...Para>
  using _insert_if_t=typename _insert_or_erase_if<Template,Predicate,AddList,true_type,Para...>::type;
  
  //erase_if_t base
  template<class Template,template<class,class...>class Pred,class...Other>
  using  _erase_if_t=typename _insert_or_erase_if<Template,Pred,make_empty_t<Template>,false_type,Other...>::type;
  
  //移除相同的元素
  template<class Template,class T>
  using _remove_t=_erase_if_t<Template,is_same,T>;
}
/* erase
 * insert
 *
 * */
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
    static constexpr auto S_out_of_range=_is_out_of_range<_self,Idx>;
    static constexpr auto S_index_v=_conversion_v<Idx,_self>;
    
    using _type=std::conditional_t<(S_index_v>=0),
      Template<Types...,Add...>,
      Template<Add...,Types...>>;
    
    using _base=_filter<Category::S_insert,_self,is_same,make_empty_t<_self>,_add,
      constant<ptrdiff_t,0>,constant<ptrdiff_t,_conversion_v<Idx,_self>>,Bool>;
  public:
    using type=std::conditional_t<S_out_of_range,_type,typename _base::type>;
  };
  //erase
  template<template<class V,V...>class Template,ptrdiff_t Idx,class V,V...v,V...add,class Bool>
  struct _erase_or_insert<Template<V,v...>,Idx,Template<V,add...>,Bool>
    :_common_pred
  {
  private:
    using _sequence=_aux<constant<V,v>...>;
    using _base=typename _erase_or_insert<_sequence,Idx,_aux<constant<V,add>...>,Bool>::type;
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
    :_erase_or_insert<Template<Types...>,Idx,Template<Add...>,true_type>
  {};
  
  template<template<class V,V...>class TL/*Type List*/,
    ptrdiff_t Idx/*insert pos*/,
    class V,V...v,
    V...add>
  struct _insert<TL<V,v...>,Idx,TL<V,add...>>
    :_common_pred
  {
  private:
    using _sequence=_aux<constant<V,v>...>;
    using _base=typename _insert<_sequence,Idx,constant<V,add>...>::type;
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
    :_erase_or_insert<Template<Types...>,Idx,Template<>,false_type>
  {};
  
  template<template<class V,V...>class TL/*Type List*/,
    ptrdiff_t Idx/*insert pos*/,
    class V,V...v>
  struct _erase<TL<V,v...>,Idx>
    :_common_pred
  {
  private:
    using _sequence=_aux<constant<V,v>...>;
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
    static constexpr auto S_index_v=extent_v<TL>!=0
      ?I%ptrdiff_t(extent_v<TL>)
      :0;
    
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
  
  template<template<class,class>class Traits,class L,class R>
  INLINE constexpr auto _three_way_comparison_v=Traits<L,R>::value||_equal_pred<Traits,L,R>::value
  ?true
  :false;
  
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
    static constexpr auto S_comparison_v=_three_way_comparison_v<B,L,R>;
    
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
    using _sequence=_aux<constant<V,v>...>;
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
  struct _less<constant<V,L>,constant<V,R>>
    :bool_constant<L<R>
  {};
  
  template<class V,V L,V R>
  struct _greater<constant<V,L>,constant<V,R>>
    :bool_constant<(L>R)>
  {};

  template<class L,template<class,class>class B>
  using _insertion_sort_t=typename _insertion_sort<L,B>::type;
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
  
  template<class List,template<class,class...>class Traits,class...P>
  INLINE constexpr auto count_if_v=equal_range<List,Traits,P...>::count;
  
  template<class List,class Type>
  INLINE constexpr auto count_v=equal_range<List,is_same,Type>::count;
  template<class TL>
  INLINE constexpr bool is_unique_v=unique<TL>::value;
}
/*test*/
namespace common
{
  #if NON_STL_17
  #include<variant>
  using std::variant;
  #else
  template<class...>class variant{};
  #endif
  #include<tuple>
  using std::tuple;
  template<class...>class TypeList{};
  
  #if 0
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
  static_assert(get_n_v<char_sequence<'1','2','3'>,4> ==Error::S_error);
  static_assert(get_n_v<char_sequence<'1','2','3'>,3> ==Error::S_error);
  static_assert(get_n_v<char_sequence<>,-11> ==Error::S_error);
  static_assert(get_n_v<char_sequence<'1','2','3'>,2> =='3');
  static_assert(get_n_v<char_sequence<'1','2','3'>,-2> =='2');
  
  static_assert(get_n_v<bool_sequence<true,false,true>,2> == true);
  static_assert(get_n_v<bool_sequence<true,false,true>,-2> == false);
  static_assert(get_n_v<range_sequence<-3,-2,-1,0,1,2,3,4>,2> == -1);
  static_assert(get_n_v<range_sequence<-3,-2,-1,0>,-4> ==-3);
  static_assert(get_n_v<range_sequence<-3,-2,-1,0>,-1> ==0);
  static_assert(get_n_v<std::index_sequence<1,2,3,4,5>,4> ==5);
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
  
  static_assert(::std::is_same_v<reverse_t<std::variant<>&>,std::variant<>&>);
  static_assert(::std::is_same_v<reverse_t<const std::variant<int,char*>>,const std::variant<char*,int>>);
  
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
  
  template<class A,class B>using _less=bool_constant<(A::value<B::value)>;
  static_assert(count_v<index_sequence<8,1,1,1,2,3,4,1>,index_constant<1>> ==4);
  static_assert(count_if_v<index_sequence<1,2,3,4,5,6,7>,_less,constant<size_t,3>> ==2);
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

#endif



