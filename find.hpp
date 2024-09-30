/*查找算法以及过滤算法*/

#ifndef META_FIND_HPP
#define META_FIND_HPP

#include "merge.hpp"
#include"erase.hpp"
#include"insert.hpp"
#include"get_n.hpp"

namespace meta_base
{
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
  
  template<class Template,
    template<class,class...>class Predicate,class...Parameters>
  class find_if
  {
  private:
    using Base=Find_base<Template,Predicate,Parameters...>;
  
  public:
    using type=typename Base::out;
  };
  
  template<class Template,
    template<class,class...>class Predicate,class...Parameters>
  class filter_if
  {
  private:
    using Base=Find_base<Template,Predicate,Parameters...>;
  
  public:
    using type=typename Base::rest;
  };
  
  template<class Template,template<class,class...>class Predicate,class...Parameters>
  using find_if_t=typename
  find_if<Template,Predicate,Parameters...>::type ;
  
  template<class Template,template<class,class...>class Predicate,class...Parameters>
  using filter_if_t=typename
  filter_if<Template,Predicate,Parameters...>::type ;
  
  template<class Template,class Type>
  using find_t=typename
  find_if_t<Template,is_same,Type>::type ;
  
  template<class Template,class Type>
  using filter_t=typename
  filter_if_t<Template,is_same,Type>::type ;
  
  template<class Template,template<class,class...>class Predicate,class...Parameters>
  inline constexpr size_t count_if_v=Find_base<Template,Predicate,Parameters...>::count;
  
  template<class Template,class T>
  inline constexpr size_t count_v=Find_base<Template,is_same,T>::count;
}

namespace meta_base
{
    template<class Template,
            template<traits_value_t<Template>,traits_value_t<Template>...>class Predicate,
            traits_value_t<Template>...Parameters>
    class Find_base_c
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
                template<class Value_type,Value_type...>class List,
                class Value_type,
                class Out,class Index_sequence,Value_type Head,Value_type...Rest>
        struct Impl<Index,std::enable_if_t<(Index<S_extent_v)>,List<Value_type,Head,Rest...>,Out,Index_sequence>
                :binary_t
                         <Predicate<Head,Parameters...>::value,
                         Impl<Index+1,void,List<Value_type,Rest...>,merge_t<Out,List<Value_type,Head>>,append_constant_t<Index_sequence,Index>>,
                         Impl<Index+1,void,List<Value_type,Rest...,Head>,Out,Index_sequence>
                         >
        {};

    public:
        using rest=typename Impl<0,void,Template,Empty_list,normal_sequence<>>::rest;
        using out=typename Impl<0,void,Template,Empty_list,normal_sequence<>>::out;
        using result_sequence=typename Impl<0,void,Template,Empty_list,normal_sequence<>>::result_sequence;
        static constexpr auto count=Impl<0,void,Template,empty_t<Template>,normal_sequence<>>::S_size;
    };

    template<class Template,traits_value_t<Template> arg>
    class equal_range_c
    {
    private:
        using value_type=traits_value_t<Template>;

        template<value_type  Arg1,value_type Arg2>
        using Is_same=is_same_c<value_type ,Arg1,Arg2>;
    public:
        using type=typename Find_base_c<Template,Is_same,arg>::result_sequence ;
    };

    template<class Template,template<traits_value_t<Template>,traits_value_t<Template>...>class Predicate,
            traits_value_t<Template>...Parameters>
    using equal_range_if_c_t
        =typename Find_base_c<Template,Predicate,Parameters...>::result_sequence ;

    template<class Template,traits_value_t<Template> Value>
    using equal_range_c_t=typename equal_range_c<Template,Value>::type ;

    template<class Template,traits_value_t<Template> Parameter>
    constexpr auto is_contain_v()
    {return extent_v<equal_range_c_t<Template,Parameter>>;}
    
  template<class Template,
    template<auto,auto...>class Predicate,auto...Parameters>
  class find_if_c
  {
  private:
    using Base=Find_base_c<Template,Predicate,Parameters...>;
  
  public:
    using type=typename Base::out;
  };
  
  template<class Template,
    template<auto,auto...>class Predicate,auto...Parameters>
  class filter_if_c
  {
  private:
    using Base=Find_base_c<Template,Predicate,Parameters...>;
  
  public:
    using type=typename Base::rest;
  };
  
  template<class Template,template<auto,auto...>class Predicate,auto...Parameters>
  using find_if_c_t=typename
  find_if_c<Template,Predicate,Parameters...>::type ;
  
  template<class Template,template<auto,auto...>class Predicate,auto...Parameters>
  using filter_if_c_t=typename
  filter_if_c<Template,Predicate,Parameters...>::type ;
  
  template<class Template,traits_value_t<Template> v>
  using find_c_t=typename
  find_if_c_t<Template,equal_Arg,v>::type ;
  
  template<class Template,traits_value_t<Template> v>
  using filter_c_t=typename
  filter_if_c_t<Template,equal_Arg,v>::type;
  
  template<class Template,template<auto,auto...>class Predicate,auto...Parameters>
  inline constexpr size_t count_if_c_v=Find_base_c<Template,Predicate,Parameters...>::count;
  
  template<class Template,traits_value_t<Template> v>
  inline constexpr size_t count_c_v=Find_base_c<Template,equal_Arg,v>::count;
    
}

#endif //META_FIND_HPP
