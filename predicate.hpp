/*
 *一些常见的谓词
 */

#ifndef META_PREDICATE_HPP
#define META_PREDICATE_HPP

#include"Meta_traits.hpp"

namespace meta_base
{
    template<class L,class R>
    using greater=bool_constant<(sizeof(L)>sizeof(R))>;

    template<class L,class R>
    using less=bool_constant<sizeof(L)<sizeof(R)>;

    template<class L,class R>
    using greater_or_equal=bool_constant<sizeof (L)>=sizeof(R)>;

    template<class L,class R>
    using less_or_equal=bool_constant<sizeof (L)<=sizeof(R)>;
    
    template<bool A,bool B>
    struct equal_pred
      :bool_constant<A&&B>
    {};
    
  template<auto v1,auto v2>
  using equal_Arg=bool_constant<v1==v2>;
}

#endif //META_PREDICATE_HPP
