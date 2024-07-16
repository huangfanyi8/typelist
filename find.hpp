#ifndef META_FIND_HPP
#define META_FIND_HPP

#include "merge.hpp"
#include"erase.hpp"
#include"insert.hpp"

namespace meta_base
{
    template<size_t Idx,class Type>
    struct pair
            :type_identity<Type>
    {
        static constexpr auto S_index=Idx;
    };

    template<class Template,template<class,class...>class Predicate,class...Parameters>
    class Find_base
    {
    private:
        using index_sequence=normal_sequence<>;
        using Empty_list=empty_t<Template>;
        static constexpr auto S_extent_v=extent_v<Template>;

        template<size_t Index,class,class List, class Out,class Index_sequence >
        struct Impl
        {
            static constexpr size_t S_size=extent_v<Out>;
            using rest=List;
            using out=Out;
            using result_sequence=Index_sequence;
        };

        template<size_t Index,
                template<class...>class List, class Out,class Index_sequence,class Head,class...Rest>
        struct Impl<Index,std::enable_if_t<(Index<S_extent_v)>,List<Head,Rest...>,Out,Index_sequence>
            :binary_t
                <Predicate<Head,Parameters...>::value,
                    Impl<Index+1,void,List<Rest...>,merge_t<Out,List<Head>>,append_constant_t<Index_sequence,Index>>,
                    Impl<Index+1,void,List<Rest...,Head>,Out,Index_sequence>
        >
        {};

    public:
        using rest=typename Impl<0,void,Template,Empty_list,normal_sequence<>>::rest;
        using out=typename Impl<0,void,Template,Empty_list,normal_sequence<>>::out;
        using result_sequence=typename Impl<0,void,Template,Empty_list,normal_sequence<>>::result_sequence;
        static constexpr auto  count=Impl<0,void,Template,empty_t<Template>,normal_sequence<>>::S_size;

    };

    template<class Template,
            template<class,class...>class Predicate,class...Parameters>
    class equal_range
    {
    private:
        using Base=Find_base<Template,Predicate,Parameters...>;

    public:
        using type=typename Base::result_sequence ;
    };

    template<class Template,template<class,class...>class Predicate,class...Parameters>
    using equal_range_if_t=typename
            equal_range<Template,Predicate,Parameters...>::type ;

    template<class Template,class Type>
    using equal_range_t=equal_range_if_t<Template,is_same,Type> ;

    template<class Template,class Type>
    constexpr auto is_contain_v()
    {return extent_v<equal_range_t<Template,Type>>;}

}

#endif //META_FIND_HPP
