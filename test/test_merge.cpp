#include"merge.hpp"

int main()
{
  using namespace common;
  
  static_assert(is_same_v<merge_t<int_sequence<1,4>,int_sequence<55>>,int_sequence<1,4,55>>);
  static_assert(is_same_v<merge_t<_aux<>,_aux<void>>,_aux<void>>);
}
