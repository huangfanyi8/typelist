#include<utility>
#include"../Meta_traits.hpp"

namespace meta
{
  
  #ifdef DIRECT_TEST
  static_assert(std::is_same_v<std::index_sequence<1,2,3>,
    typename meta_base::constant_sequence<bool>::template rebind<std::integer_sequence,size_t,1,2,3>>);
  #endif
}