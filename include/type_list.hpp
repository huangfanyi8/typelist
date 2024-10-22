#include<utility>
#include"../get_n.hpp"
#include"../erase.hpp"
#include"../insert.hpp"
#include <bits/stl_construct.h>

namespace meta
{
  #ifdef DIRECT_TEST
  static_assert(std::is_same_v<std::index_sequence<1,2,3>,
    typename meta_base::constant_sequence<bool>::template rebind<std::integer_sequence,size_t,1,2,3>>);
  #endif
  
  template<class T>
  struct type_identity
    :meta_base::type_identity<T>
  {};
  
  template<class T>
  struct CPO
  {};
}

//reverse
namespace meta
{
  template<class Template>
  struct reverse
    :meta_base::reverse<Template>
  {};
  
  template<class Template>
  using reverse_t=typename reverse<Template>::type;
}

//get nth element
namespace meta
{
  template<class T,ptrdiff_t Idx,
    bool=meta_base::is_correct_v<T>>
  struct get_n
    :meta_base::get_n_element<T,Idx>
  {};
  
  template<class T,ptrdiff_t Idx>
  struct get_n<T,Idx,false>
  {static_assert(false,"T must be a template!");};
  
  template<class Template,ptrdiff_t Idx,
    class=std::enable_if_t<meta_base::is_normal_v<Template>>
      >
  using get_n_t=typename get_n<Template,Idx>::type;
  
  template<class Template,ptrdiff_t Idx,
    class=std::enable_if_t<!meta_base::is_normal_v<Template>>>
  inline constexpr auto get_n_v=get_n<Template,Idx>::value;
  
  template<class Default,class Template>
  struct front
    :std::conditional_t<meta_base::is_same_v<meta_base::front_t<Template>,meta_base::undefined>,
    type_identity<Default>,
    type_identity<meta_base::front_t<Template>>>
  {};
  
  template<class Template,class Default>
  using front_t=typename front<Default,Template>::type;
  
  template<class Template,class Default>
  using back_t=front_t<Default,reverse_t<Template>>;
  
  template<class Template>
  inline constexpr auto front_v=get_n_v<Template,0>;
  
  template<class Template>
  inline constexpr auto back_v=front_v<reverse_t<Template>>;
}

//merge
namespace meta
{
  template<class Head,class...K>
  struct merge
    :meta_base::merge<Head,K...>
  {
    static_assert(meta_base::and_v<meta_base::is_similar_v<Head,K>...>);
  };
  
  template<class...Types>
  using merge_t=typename merge<Types...>::type;
}

//erase
namespace meta
{
  template<class Template,ptrdiff_t Idx,
    class=std::enable_if_t<meta_base::is_correct_v<Template>>>
  struct erase
    :type_identity<meta_base::erase_t<Template,Idx>>
  {};
  
  template<class Template,ptrdiff_t Idx>
  using erase_t=typename erase<Template,Idx>::type;
}

