#include"../swap.hpp"

#include<variant>
#include<tuple>

template<class T>
using is_same_int=std::is_same<T,int>;

int main()
{
    using namespace meta_base;
    static_assert(is_same_v<typename replace_if<std::variant<int,double>,is_same_int,std::tuple<double>>::type,
            std::variant<double,double>
    >);

    static_assert(is_same_v<typename replace_if<std::variant<>,is_same_int,std::tuple<double>>::type,
            std::variant<>
    >);
}