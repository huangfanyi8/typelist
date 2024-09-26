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
    

}

#endif //META_PREDICATE_HPP
