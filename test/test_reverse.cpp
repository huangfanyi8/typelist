#include"../reverse.hpp"
#include<variant>
#include<tuple>


int main()
{
  using namespace meta_base;
  
  static_assert(meta_base::is_same_v<meta_base::int_sequence<1,2,3,4,5>,
          meta_base::reverse_t<meta_base::int_sequence<5,4,3,2,1>>>);

  static_assert(meta_base::is_same_v<meta_base::int_sequence<1>,
          meta_base::reverse_t<meta_base::int_sequence<1>>>);
  
  static_assert(std::is_same_v<reverse_t<std::variant<>>,std::variant<>>);
  static_assert(std::is_same_v<reverse_t<std::variant<int,char*>>,std::variant<char*,int>>);
}