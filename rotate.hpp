#ifndef META_ROTATE_HPP
#define META_ROTATE_HPP

#include"erase.hpp"
#include"get_n.hpp"
#include"insert.hpp"

namespace meta_base
{
    template<class Template,ptrdiff_t distance,bool=is_normal_v<Template>>
    struct rotate
    {
    private:
        static constexpr ptrdiff_t S_size=static_cast<ptrdiff_t>(extent_v<Template>);
        static constexpr ptrdiff_t S_offset=S_size?distance%S_size:0;

        template<ptrdiff_t x,ptrdiff_t y>
        static constexpr ptrdiff_t S_compare_v=  x>y?0:x==y?1:2;

        template<class T,ptrdiff_t Idx>
        struct Impl
                :switch_t<S_compare_v<Idx,S_offset>,
                        Impl<append_t<pop_front_t<T>,front_t<T>>,Idx-1>,
        type_identity<T>,
        Impl<prepend_t<pop_back_t<T>,back_t<T>>,Idx+1>>
        {};

    public:
        using type=binary_t<is_empty_v<Template>,Template,typename Impl<Template,0>::type>;
    };

    template<class Template,ptrdiff_t distance>
    struct rotate<Template,distance,false>
    {
    private:
        static constexpr ptrdiff_t S_size=static_cast<ptrdiff_t>(extent_v<Template>);
        static constexpr ptrdiff_t S_offset=S_size?distance%S_size:0;

        template<ptrdiff_t x,ptrdiff_t y>
        static constexpr ptrdiff_t S_compare_v=  x>y?0:x==y?1:2;

        template<class T,ptrdiff_t Idx>
        struct Impl
                :switch_t<S_compare_v<Idx,S_offset>,
                        Impl<append_constant_t<pop_front_t<T>,front_v<T>>,Idx-1>,
        type_identity<T>,
        Impl<prepend_constant_t<pop_back_t<T>,back_v<T>>,Idx+1>>
        {};

    public:
        using value_type=traits_value_t<Template>;
        using type=binary_t<is_empty_v<Template>,Template,typename Impl<Template,0>::type>;
        static constexpr ptrdiff_t compare_v=S_compare_v<0,S_offset>;
    };

    template<class Template,ptrdiff_t distance>
    using rotate_t=typename rotate<Template,distance>::type;
}
#endif //META_ROTATE_HPP
