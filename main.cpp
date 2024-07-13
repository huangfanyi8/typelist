

#include"Meta_traits.hpp"

int main()
{
    static_assert(std::is_same_v<meta_base::switch_t<0,int>,int>);
}
