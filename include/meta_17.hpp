#ifndef META_META_17_HPP
#define META_META_17_HPP

#include<utility>
#include <cstddef>

#if defined(_MSVC_LANG)
#define STL_LANG _MSVC_LANG
#else
#define STL_LANG __cplusplus
#endif

#define HAS_CXX(VERSION) (STL_LANG >= VERSION)

#define STL_17 HAS_CXX(201703L)
#define STL_20 HAS_CXX(202002L)

#define  SOME_NAMESPACE _DETAILS(nmae) namespace some##name
#if STL_17

namespace cxx17
{
    using signed_index_t = ptrdiff_t;
    using unsigned_index_t = size_t;
    using signed_size_type = ptrdiff_t;
    using unsigned_size_type = size_t;
    
    template<signed_index_t _value>
    using signed_index_constant = std::integral_constant<signed_index_t ,_value>;
    
    template<unsigned_index_t _value>
    using unsigned_index_constant = std::integral_constant<unsigned_index_t ,_value>;
}

namespace cxx17
{template<class...>struct  list{};}

namespace cxx17
{
    //未定义类型，确保每种状态下都有值
    struct undefined_t
    {
        constexpr explicit undefined_t() = default;
    };
    
    //未定义常量，确保每种状态下都有值
    inline constexpr undefined_t undefined{};
    
    struct error_constant{static constexpr undefined_t value = undefined;};
}

namespace cxx17
{
    template<class T>
    struct identity
    {using type =T;};
}

namespace cxx17
{
    template<class>struct template_traits;
    
    template<class...T>
    struct template_traits<list<T...>>
    {
        using type = list<>;
        static constexpr unsigned_size_type extent = sizeof...(T);
        using value_type = void;
    };

    template<class T,T ... _value>
    struct template_traits<std::integer_sequence<T,_value...>>
    {
        using type = std::integer_sequence<T>;
        static constexpr unsigned_size_type extent = sizeof...(_value);
        using value_type = T;
    };

    template<class T,T _value>
    struct template_traits<std::integral_constant<T,_value>>
    {
        static constexpr unsigned_size_type extent = 1;
        using value_type = T;
    };
    
    template<class T>
    using empty_t = typename template_traits<T>::type;
    
    template<class T>
    using traits_value_t = typename template_traits<T>::value_type;
    
    template<class T>
    inline constexpr unsigned_size_type extent_v = template_traits<T>::extent;
}

namespace cxx17
{
    template<class,class>struct to_custom;
    template<class T,class = traits_value_t<T>>struct list_or_sequence;
    
    template<template<class...>class Custom,class...Other,class...List>
    struct to_custom<Custom<Other...>,list<List...>>
        :identity<Custom<List...>>
    {};
    
    template<template<class DataType,DataType...>class Custom,class DataType,DataType..._other,DataType..._list>
    struct to_custom<Custom<DataType,_other...>,std::integer_sequence<DataType,_list...>>
        :identity<Custom<DataType,_list...>>
    {};
    
    template<class DataType,DataType..._value>
    struct list_or_sequence<std::integer_sequence<DataType,_value...>,void>
        :identity<list<std::integral_constant<DataType,_value>...>>
    {};
    
    template<class DataType,DataType..._value>
    struct list_or_sequence<list<std::integral_constant<DataType,_value>...>,DataType>
        :identity<std::integer_sequence<DataType,_value...>>
    {};
    
    template<class Custom,class List>
    using to_custom_t = typename to_custom<Custom,List>::type;
    
    template<class Template>using list_or_sequence_t = typename list_or_sequence<Template>::type;
    /*
    template<class...>class variant{};
    static_assert(std::is_same_v<variant<int,double>,to_custom_t<variant<>,list<int,double>>>);
    */
 }
 
namespace cxx17::_details
{
    template<auto _index,class T>
    struct _pair
    {
        static constexpr auto  value = _index;
        using type = T;
    };
    
    template<class>struct _make_pair_list;
    
    template<class...List>
    struct _make_pair_list<list<List...>>
    {
    private:
        template<class>
        struct _impl{};
        
        template<size_t..._index>
        struct _impl<std::index_sequence<_index...>>
        {using type = list<_pair<signed_index_t (_index),List>...>;};
        
        using _id_sequence = std::index_sequence_for<List...>;
    public:
        using type = typename _impl<_id_sequence>::type;
    };
    
    template<class T,T..._value>
    struct _make_pair_list<std::integer_sequence<T,_value...>>
    {
    private:
        template<class>
        struct _impl{};
        
        template<size_t..._index>
        struct _impl<std::index_sequence<_index...>>
        {using type = list<_pair<signed_index_t (_index),std::integral_constant<T,_value>>...>;};
        
        using _id_sequence = std::make_index_sequence<sizeof...(_value)>;
    public:
        using type = typename _impl<_id_sequence>::type;
    };
    
    template<class T>using _make_pair_list_t = typename  _make_pair_list<T>::type;
    
    template<class T,template<class...>class,class...>
    struct _transform
    {};

    template<class T,T..._value,template<class...>class Tr,class...Other>
    struct _transform<std::integer_sequence<T,_value...>,Tr,Other...>
    {
    private:
        using _list = list<std::integral_constant<T,_value>...>;
        
        template<class U>
        struct _impl
            :identity<U>
        {};
        
        template<class U,U..._>
        struct _impl<list<std::integral_constant<U,_>...>>
            :identity<std::integer_sequence<U,_...>>
        {};
    public:
        using type = typename _impl<typename Tr<_list,Other...>::type>::type;
    };
    
    template<class,class T,template<class...>class Traits,class...Parameters>
    struct _is_contain_type
        :std::false_type
    {};
    
    template<class T,template<class...>class Traits,class...Parameters>
    struct _is_contain_type<std::void_t<typename Traits<T,Parameters...>::type>,T,Traits,Parameters...>
        :std::true_type
    {};
    
    template<class T,template<class...>class Traits,class...Parameters>
    inline constexpr bool _is_contain_type_v = _is_contain_type<void,T,Traits,Parameters...>::value;
    
    template<class T,template<class...>class Traits,class...Pameters>
    struct _transform<T,Traits, std::false_type,Pameters...>
    {using type = list<Traits<T,Pameters...>>;};
    
    template<class T,template<class...>class Traits,class...Parameters>
    struct _transform<T,Traits, std::true_type,Parameters...>
    {using type = list<typename Traits<T,Parameters...>::type>;};
    
    template<class T,template<class...>class Traits,class...Parameters>
    using  _transform_t = typename _transform<T,Traits,std::bool_constant<_is_contain_type_v<T,Traits,Parameters...>>,Parameters...>::type;
}

namespace cxx17
{
    template<class>struct list_wrapper;
    template<class>struct sequence_wrapper;
}

namespace cxx17
{
    template<class F,class ... T>
    inline constexpr bool is_same_v = (std::is_same_v<F,T>&&...);
    
    namespace _details
    {
        template<signed_index_t _input_index,class TL>
        inline constexpr signed_index_t  _final_index_v =
            _input_index >= 0 ? _input_index : _input_index + signed_size_type(extent_v<TL>);
        
        template<class ,class = void>
        inline constexpr bool _is_undefined_v = false;
        
        template<>
        inline constexpr bool _is_undefined_v<undefined_t> = true;
        
        template<class T>
        inline constexpr bool _is_undefined_v<T,std::void_t<decltype(T::value)>> = true;
    }
}

namespace cxx17
{
    /*Merge Algorithm*/
    template<class...Lists>
    class merge
    {};
    
    template<class Head,class...Rest>
    class merge<Head,Rest...>
    {
    private:
        template<class...>
        struct _impl
        {};
        
        template<template<class...>class Template,class...Types>
        struct _impl<Template<Types...>>
            :identity<Template<Types...>>
        {};
        
        template<template<class Value,Value...>class Template,class Value,Value...value>
        struct _impl<Template<Value,value...>>
            :identity<Template<Value,value...>>
        {};
        
        template<template<class...>class Template,class...P,class...O>
        struct _impl<Template<P...>,Template<O...>>
            :identity<Template<P...,O...>>
        {};
        
        template<template<class Value,Value...>class Template,class Value,Value...P,Value...O>
        struct _impl<Template<Value,P...>,Template<Value,O...>>
            :identity<Template<Value,P...,O...>>
        {};
        
        template<class T,class U,class...Other>
        struct _impl<T,U,Other...>
            :_impl<typename _impl<T,U>::type,Other...>
        {};
    public:
        using type=typename _impl<Head,Rest...>::type;
    };
    
    template<class...Lists>
    using merge_t=typename merge<Lists...>::type;
}

namespace cxx17
{
    template<class TL,signed_index_t..._input_index>
    struct in_range
    {
    public:
        enum  range{less,greater,in};
    private:
        template<class,class = void>
        struct _impl
            :std::false_type
        {};
        
        template<range..._value>
        struct _impl<std::integer_sequence<range,_value...>,std::enable_if_t<((_value==in)&&...)>>
            :std::true_type
        {};
        static constexpr signed_index_t _size = signed_index_t (extent_v<TL>);
    public:
        using type = std::integer_sequence<range,((_size<=_details::_final_index_v<_input_index,TL>)
                                                    ?greater
                                                    :0>_details::_final_index_v<_input_index,TL>
                                                    ?less
                                                    :in)...>;
        
        static constexpr bool value = _impl<type>::value;
    };
    
    template<class TL,signed_index_t..._input_index>
    inline constexpr bool  in_range_v = in_range<TL,_input_index...>::value;
}

namespace cxx17::_details
{
    template<class Template,class Index,class Undefined= undefined_t,class = void>
    struct _get
            :identity<Undefined>
    {};
    
    template<class A,class...Rest,class Undefined>
    struct _get<list<A,Rest...>,signed_index_constant<0>,Undefined>
        :identity<A>
    {};
    
    template<class A,class B,class...R,class Undefined>
    struct _get<list<A,B,R...>,signed_index_constant<1>,Undefined>
    {using type = B;};
    
    template<class A,class B,class C,class...R,class Undefined>
    struct _get<list<A,B,C,R...>,signed_index_constant<2>,Undefined>
    {using type = C;};
    
    template<class A,class B,class C,class D,class...R,class Undefined>
    struct _get<list<A,B,C,D,R...>,signed_index_constant<3>,Undefined>
    {using type = D;};
    
    template<signed_index_t  _index,class A,class B,class C,class D,class E,class...R,class Undefined>
    struct _get<list<A,B,C,D,E,R...>,signed_index_constant<_index>,Undefined,std::enable_if_t<(_index>=4)>>
        :_get<list<E,R...>,signed_index_constant<_index-4>,Undefined>
    {};
    
    template<signed_index_t  _index,class DataType,DataType..._value,class Undefined>
    struct _get<std::integer_sequence<DataType,_value...>,signed_index_constant<_index>,Undefined>
        :_transform<std::integer_sequence<DataType,_value...>,_get,signed_index_constant<_index>,Undefined>
    {};
    
    template<class Template,class Index,class Undefined = undefined_t>
    using _get_t = typename _get<Template,Index,Undefined>::type;
}

namespace cxx17
{
    template<signed_index_t _index,class List,class Undefined = undefined_t>
    using get_t = _details::_get_t<List,signed_index_constant<_details::_final_index_v<_index,List>>,Undefined>;
    
    template<class T>using front_t = get_t<0,T>;
    template<class T>using back_t = get_t<-1,T>;
    
    template<signed_index_t _index,class Sequence,class Error = error_constant, class =void>
    inline  constexpr auto get_v =get_t<_index,Sequence,Error>::value;
}

namespace cxx17
{
    template<class T,class U= empty_t<T>>
    struct reverse
        :identity<U>
    {};
    
    template<class F,class...T,class...U>
    struct reverse<list<F,T...>,list<U...>>
        :reverse<list<T...>,list<F,U...>>
    {};
    
    template<class T,T..._value>
    struct reverse<std::integer_sequence<T,_value...>>
        :_details::_transform<std::integer_sequence<T,_value...>,reverse>
    {};
    
    template<class T,class  U = empty_t<T>,class B= std::true_type>
    struct unique
        :_details::_pair<B::value,U>
    {};
    
    template<class F,class...T,class...U,class B>
    struct unique<list<F,T...>,list<U...>,B>
        :std::conditional_t<(std::is_same_v<F,U>||...),unique<list<T...>,list<U...>,std::false_type >,unique<list<T...>,list<U...,F>,std::true_type >>
    {};
    
    template<class T,T..._value>
    struct unique<std::integer_sequence<T,_value...>>
        :_details::_transform<std::integer_sequence<T,_value...>,unique>
    {};
    
    template<class T>using unique_t = typename unique<T>::type;
    template<class T>using reverse_t = typename reverse<T>::type;
    template<class T>inline constexpr bool is_unique_v = unique_t<T>::value;
    
    //static_assert(is_same_v<unique_t<std::index_sequence<>>,std::index_sequence<>>);
}

namespace cxx17
{
    template<class,class...>struct append;
    template<class,class...>struct prepend;
    template<class>struct pop_front;
    template<class>struct pop_back;

    template<class...T,class...O>
    struct append<list<T...>,O...>
        :identity<list<T...,O...>>
    {};

    template<class...T,class...O>
    struct prepend<list<T...>,O...>
        :identity<list<O...,T...>>
    {};

    template<template<class DataType,DataType...>class Sequence,class DataType,DataType..._values,DataType..._add>
    struct prepend<Sequence<DataType,_values...>,Sequence<DataType,_add...>>
    {};

    template<class T,class... Add>using append_t =typename append<T,Add...>::type;
    template<class T,class... Add>using prepend_t =typename prepend<T,Add...>::type;

    namespace _details
    {
        enum _modifier_type{_inset_after,_insert_before,_replace,_erase};

        template<_modifier_type _v>
        using _modifier_constant = std::integral_constant<_modifier_type,_v>;

        template<class Template,class Index,class,class Type,class Bool = std::bool_constant<in_range_v<Template,Index::value>>>struct _modifier;

        template<class FirstItem,class...RestItems,signed_index_t _index,class...NewItems>
        struct _modifier<list<FirstItem,RestItems...>,signed_index_constant<_index>,list<NewItems...>, _modifier_constant<_insert_before>,std::false_type>
        {};

        template<class Template,template<class...>class Traits,class...Parameters>
        struct _insert_
        {

        };
    }

}

namespace cxx17
{
    inline constexpr size_t _max_value = size_t(-1);
    
    //rotation==0 left subtree
    //rotation==1 right subtree
    //rotation ==-1 root node
    template<size_t _value ,int _id,int _parent,int _rotation>
    struct _huffman_node
    {
        static constexpr size_t value = _value;
        static  constexpr int id =_id;
        static constexpr int parent = _parent;
        static constexpr int rotation = _rotation;
    };
    using _aux_node = _huffman_node<_max_value,-1,-1,-1>;
    
    //选出两个最小的节点合并为一个新的节点  并更新当前节点列表
    template<class Input,class Output,class Copy,class First,class Second,size_t _new_id>
    struct _find_two_min_nodes
    {
        using min_node = First;
        using sec_node  = Second;
        template<class,class>struct _impl{};
        
        //    template<size_t _value ,int _id,int _parent,int _rotation>
        template<class...O,class...C>
        struct _impl<list<O...>,list<C...>>
        {
            using type = list<std::conditional_t<O::id==First::id||O::id==Second::id,
            std::conditional_t<O::id==First::id,First,Second>,C>...>;
        };
        
        using type = append_t<typename _impl<Output,Copy>::type,_huffman_node<First::value+Second::value,_new_id,-1,-1>>;
    };
    
    template<class Node,class...Nodes,class First,class Second,size_t _new_id,class ...O,class Copy>
    struct _find_two_min_nodes<list<Node,Nodes...>,list<O...>,Copy,First,Second,_new_id>
        :std::conditional_t<
            (Node::value<First::value)&&Node::parent==-1,
            _find_two_min_nodes<list<Nodes...>,list<O...,_huffman_node<Node::value,Node::id,_new_id,0>>,Copy,
            _huffman_node<Node::value,Node::id,_new_id,0>,_huffman_node<First::value,First::id,First::parent,1>,_new_id>,
            std::conditional_t<
                (Second::value>Node::value)&&Node::parent==-1,
                _find_two_min_nodes<list<Nodes...>,list<O...,_huffman_node<Node::value,Node::id,_new_id,1>>,Copy,First,_huffman_node<Node::value,Node::id,_new_id,1>,_new_id>,
                _find_two_min_nodes<list<Nodes...>,list<O...,Node>,Copy,First,Second,_new_id>
            >//conditional
        >//conditional
    {};
    
    template<class T>using _huffman_min_node_t = typename _find_two_min_nodes<T,list<>,T,_aux_node,_aux_node,extent_v<T>>::min_node;
    template<class T>using _huffman_sec_node_t = typename _find_two_min_nodes<T,list<>,T,_aux_node,_aux_node,extent_v<T>>::sec_node ;
    template<class T>using _huffman_new_list_t = typename _find_two_min_nodes<T,list<>,T,_aux_node,_aux_node,extent_v<T>>::type ;
    
    //    template<size_t _value ,int _id,int _parent,int _rotation>
    using _test_sequence = list<
        _huffman_node<5,0,-1,-1>,
        _huffman_node<6,1,-1,-1>,
        _huffman_node<8,2,-1,-1>,
        _huffman_node<4,3,-1,-1>,
        _huffman_node<7,4,-1,-1>,
        _huffman_node<3,5,-1,-1>
        >;
    
    static_assert(is_same_v<_huffman_min_node_t<_test_sequence>, _huffman_node<3,5,6,0>>);
    static_assert(is_same_v<_huffman_sec_node_t<_test_sequence>, _huffman_node<4,3,6,1>>);
    static_assert(is_same_v<_huffman_new_list_t<_test_sequence>,
        cxx17::list<cxx17::_huffman_node<5, 0,-1,-1 >,
        _huffman_node<6,1,-1,-1>,
        cxx17::_huffman_node<8, 2, -1, -1>,
        cxx17::_huffman_node<4, 3, 6, 1>,
        cxx17::_huffman_node<7, 4, -1, -1>,
        cxx17::_huffman_node<3, 5, 6, 0>,
        cxx17::_huffman_node<7, 6, -1, -1>>>);
    
    template<class T,size_t _new_id,size_t _end>
    struct _make_huffman_tree
    {using type =T;};
    
    template<class T>
    using _make_huffman_tree_t = typename  _make_huffman_tree<T,extent_v<T>,2*extent_v<T>-2>::type ;
    
    template<class...Nodes,size_t _new_id,size_t _end>
    struct _make_huffman_tree<list<Nodes...>,_new_id,_end>
        :std::conditional_t<(_new_id<=_end),
        _make_huffman_tree<_huffman_new_list_t<list<Nodes...>>,_new_id+1,_end>,
        identity<list<Nodes...>>>
    {};
    
    static_assert(is_same_v<_make_huffman_tree_t<_test_sequence>,
        cxx17::list<
        cxx17::_huffman_node<5, 0, 7, 0>,
        cxx17::_huffman_node<6, 1, 7, 1>,
        cxx17::_huffman_node<8, 2, 9, 0>,
        cxx17::_huffman_node<4, 3, 6, 1>,
        cxx17::_huffman_node<7, 4, 8, 0>,
        cxx17::_huffman_node<3, 5, 6, 0>,
        cxx17::_huffman_node<7, 6, 8, 1>,
        cxx17::_huffman_node<11, 7, 9, 1>,
        cxx17::_huffman_node<14, 8, 10, 0>,
        cxx17::_huffman_node<19, 9, 10, 1>,
        cxx17::_huffman_node<33, 10, -1, -1>>>);
    
    
    template<class ,class,class S,int,class = void>
    struct _make_huffman_code
    {using type =S;};
    //    template<size_t _value ,int _id,int _parent,int _rotation>
    template<class Node,class...Nodes,int..._code,int _end>
    struct _make_huffman_code<Node,list<Nodes...>,std::integer_sequence<int,_code...>,_end,std::enable_if_t<(_end!=-1)>>
        :_make_huffman_code<get_t<Node::parent,list<Nodes...>>,list<Nodes...>,std::integer_sequence<int,Node::rotation,_code...>,Node::parent>
    {};
    
    template<class List,class Node>
    using _make_huffman_code_t = typename _make_huffman_code<Node,List,std::integer_sequence<int>,Node::parent>::type;
}

namespace cxx17
{
    namespace _details
    {
        template<class T,signed_index_t _left,signed_index_t _right,bool = in_range_v<T,_left,_right>>struct _swap;
        
        template<class T,signed_index_t _left,signed_index_t _right>
        struct _swap<T,_left,_right, false>
            :identity<T>
        {};
        
        template<class T,signed_index_t _left,signed_index_t _right>
        struct _swap<T,_left,_right, true>
        {
        private:
            using _list = _details::_make_pair_list_t<T>;
            static constexpr signed_index_t _left_index = _details::_final_index_v<_left,T>;
            static constexpr signed_index_t _right_index = _details::_final_index_v<_right,T>;
            using _pair_l = get_t<_left_index,_list>;
            using _pair_r = get_t<_right_index,_list>;
            
            template<class>struct _impl{};
            template<class...Pair>
            struct _impl<list<Pair...>>
            {
                using type =list<std::conditional_t<(Pair::value == _left_index),
                typename _pair_r ::type,
                std::conditional_t<(Pair::value == _right_index),typename _pair_l ::type,typename Pair::type>>...>;
            };
            
            using _type = typename _impl<_list>::type;
            
            template<class U,bool = std::is_same_v<traits_value_t<T>,void>>struct _impl1;
            
            template<class...U>
            struct _impl1<list<U...>,true>
                :identity<list<U...>>
            {};
            
            template<class...U>
            struct _impl1<list<U...>,false>
                :identity<std::integer_sequence<traits_value_t<T>,U::value...>>
            {};
            
        public:
            using type = typename _impl1<_type>::type;
        };
        
        template<class T,signed_index_t _left,signed_index_t  _right>using _swap_t = typename _swap<T,_left,_right>::type;
    }
    template<class T,size_t _l,size_t _r>
    using swap_t = _details::_swap_t<T,_l,_r>;
}

namespace cxx17
{
    namespace _details
    {
        template<class T,signed_index_t _current,signed_index_t _end,bool = (_current * 2 + 1 <= _end),bool = (_current * 2 + 2 <= _end)>
        struct _three_max
            :std::integral_constant<signed_index_t ,_current>
        {};
        
        template<class T,signed_index_t _current,signed_index_t _end>
        struct _three_max<T,_current,_end,true,true>
        {
            using _sequence = T;
            static constexpr auto _left_child = 2*_current+1;
            static constexpr auto _left_child_value  =   get_v<_left_child,_sequence>;
            static constexpr auto _right_child_value  =   get_v<_left_child+1,_sequence>;
            static constexpr auto _root_value = get_v<_current,_sequence>;
            static constexpr auto value= _left_child_value>_right_child_value?
                                                 (_left_child_value>_root_value?_left_child:_current)
                                                                                     :(_right_child_value>_root_value?_left_child+1:_current);
        };
        
        template<class T,signed_index_t _root,signed_index_t _end>
        struct _three_max<T,_root,_end,true,false>
        {
            using _sequence =T;
            static constexpr auto _left_child_value  =   get_v<2*_root+1,_sequence>;
            static constexpr auto _root_value = get_v<_root,_sequence>;
            static constexpr auto value =  _left_child_value>_root_value?2*_root+1:_root;
        };
        
        template<class T,signed_index_t _current,signed_index_t _end>
        inline constexpr signed_index_t  _three_max_v = _three_max<T,_current,_end>::value;
        
        template<class T,signed_index_t _current,signed_index_t _end,signed_index_t _max = _details::_three_max_v<T,_current,_end>>
        struct _heap_adjust
            :std::conditional_t<_max!=_current,
            _heap_adjust<_swap_t<T,_max,_current>,_max,_end>,
            identity<T>>
        {};
        
        template<class T,signed_index_t _current,signed_index_t _end>
        using  _heap_adjust_t = typename _heap_adjust<T,_current,_end>::type;
        
        template<class T,signed_index_t _index>
        struct _make_heap
            :std::conditional_t<(_index>=0),
            _make_heap<_heap_adjust_t<T,_index, signed_index_t (extent_v<T>) - 1>, _index - 1>,identity<T>>
        {};
        
        template<class T>using _make_heap_t =typename _make_heap<T, signed_index_t(extent_v<T>) / 2 - 1>::type;
        
        template<class T>
        struct _heap_sort
        {
            using _type = _make_heap_t<T>;
            
            template<class U,signed_index_t _index>
            struct _impl
                :_impl<_heap_adjust_t <_swap_t<U,0,_index>,0,_index-1>,_index-1>
            {};
            
            template<class U>
            struct _impl<U,0>
                :identity<U>
            {};
            
            using type =typename _impl<_type, signed_index_t (extent_v<T>) - 1>::type;
        };
        
        template<class T>
        using _heap_sort_t = typename _heap_sort<T>::type;
        
        static_assert(is_same_v<_make_heap_t<std::index_sequence<24,44, 18 ,52 ,92 ,74 ,44 ,15 ,57 ,17>>,
            std::index_sequence<92, 57 ,74, 52 ,44 ,18, 44, 15 ,24 ,17>>);
        
        static_assert(is_same_v<_heap_sort_t<std::index_sequence<24,44, 18 ,52 ,92 ,74 ,44 ,15 ,57 ,17>>,
            std::index_sequence<15, 17, 18 ,24 ,44, 44 ,52 ,57 ,74, 92>>);
        
        static_assert(is_same_v<_heap_sort_t<std::index_sequence<1,2>>,std::index_sequence<1,2>>);
        static_assert(is_same_v<_heap_sort_t<std::index_sequence<1>>,std::index_sequence<1>>);
        static_assert(is_same_v<_make_heap_t<std::index_sequence<1,5>>>);
        static_assert(is_same_v<_make_heap_t<std::index_sequence<1>>>);
        static_assert(is_same_v<_make_heap_t<std::index_sequence<1>>>);
    }
    
    template<class T>
    using heap_sort_t = _details::_heap_sort_t<T>;
    
    template<class T>using make_heap_t =_details::_make_heap_t<T>;
    
    template<class T>
    struct is_max_heap
    {
        static constexpr signed_size_type _extent = signed_size_type (extent_v<T>);
        static constexpr signed_size_type _end = (_extent - 2) / 2;
        
        template<signed_index_t  _begin,bool = _begin * 2 + 1 < _extent,bool = _begin * 2 + 2 < _extent>
        struct _impl
            :std::true_type
        {};
        
        template<signed_index_t _begin>
        struct _impl<_begin,true,false>
            :std::bool_constant<((get_v<_begin,T>)>=get_v<_begin*2+1,T>)>
        {};
        
        template<signed_index_t _begin>
        struct _impl<_begin,true,true>
            :std::bool_constant<((get_v<_begin,T>)>=get_v<_begin*2+1,T>)&&((get_v<_begin,T>)>=get_v<_begin*2+2,T>)>
        {};
        
        template<signed_index_t _begin>
        struct _pimpl
            :std::conditional_t<_begin<=_end,std::conditional_t<_impl<_begin>::value,_pimpl<_begin+1>,std::false_type>,std::true_type >
        {};
        
        static constexpr bool value = _pimpl<0>::value;
    };
    
    static_assert(is_max_heap<make_heap_t<std::index_sequence<1,4,3,6,8,9,4,7,22>>>::value);
}

namespace meta::cxx17
{
    //未定义类型，确保每种状态下都有值
    struct undefined_t
    {constexpr explicit undefined_t() = default;};
    
    //未定义常量，确保每种状态下都有值
    inline constexpr undefined_t undefined{};
    
    //TypeList
    template<class...>
    struct meta_list{};
    
    //cxx20 std::type_identity
    template<class T>
    struct type_identity
    {using type = T;};
    
    //Meta Constant
    enum class meta_state{
        error
    };
    
    template<class>struct template_traits;
    
    template<template<class...>class TypeList,class...T>
    struct template_traits<TypeList<T...>>
    {
        using type = TypeList<>;
        static constexpr size_t extent = sizeof...(T);
        using value_type = void;
    };
    
    template<template<class DataType,DataType...>class Sequence,class DataType,DataType ... _value>
    struct template_traits<Sequence<DataType,_value...>>
    {
        using type = Sequence<DataType>;
        static constexpr size_t extent = sizeof...(_value);
        using value_type = DataType;
    };
    
    //获取空模板类型
    template<class T>
    using empty_t = typename template_traits<T>::type;
    
    //萃取元序列的元素类型
    template<class T>
    using traits_value_t = typename template_traits<T>::value_type;
    
    //获取模板长度
    template<class T>
    inline constexpr size_t extent_v = template_traits<T>::extent;
    //获取模板长度
    template<class T>
    inline constexpr ptrdiff_t signed_extent_v = static_cast<ptrdiff_t>(template_traits<T>::extent);

    template<ptrdiff_t _input_index,class Template>
    inline constexpr ptrdiff_t  _final_index_v =
        _input_index >= 0 ? _input_index : _input_index + signed_extent_v<Template>;
    
    using error_constant = std::integral_constant<meta_state,meta_state::error>;
    
    template<class T,class,class = void>
    struct get
        :std::conditional_t<std::is_same_v<traits_value_t<T>,void>,error_constant,undefined_t>
    {};
    
    template<template<class...>class TypeList,class F,class...Rest>
    struct get<TypeList<F,Rest...>,std::integral_constant<ptrdiff_t,0>>
    {using type = F;};
    
    template<template<class...>class TypeList,class F,class B,class...Rest>
    struct get<TypeList<F,B,Rest...>,std::integral_constant<ptrdiff_t,1>>
    {using type = B;};
    
    template<template<class...>class TypeList,class F,class B,class C,class...Rest>
    struct get<TypeList<F,B,C,Rest...>,std::integral_constant<ptrdiff_t,2>>
    {using type = C;};
    
    template<template<class...>class TypeList,class F,class B,class C,class D,class...Rest>
    struct get<TypeList<F,B,C,D,Rest...>,std::integral_constant<ptrdiff_t,3>>
    {using type = D;};
    
    template<template<class...>class TypeList,ptrdiff_t _index,class F,class B,class C,class D,class...Rest>
    struct get<TypeList<F,B,C,D,Rest...>,std::integral_constant<ptrdiff_t,_index>,std::enable_if_t<(_index>3)>>
        :get<TypeList<Rest...>,std::integral_constant<ptrdiff_t,_index-3>>
    {};
    
    template<class,template<class...>class,class...>struct meta_transform;
    
    template<template<class DataType,DataType...>class Sequence,class  DataType,DataType..._value,
    template<class...>class Traits,class...Other>
    struct meta_transform<Sequence<DataType,_value...>,Traits,Other...>
    {
    private:
        using _list = meta_list<std::integral_constant<DataType,_value>...>;
        using _type = typename Traits<_list,Other...>::type;
        
        template<class T>
        struct _pimpl
            :type_identity<T>
        {};
        
        template<class...Items>
        struct _pimpl<meta_list<Items...>>
            :type_identity<Sequence<DataType,Items::value...>>
        {};
    public:
        using type = typename _pimpl<_type>::type;
    };
    
    template<class Template,template<class...>class Traits,class...P>
    using meta_transform_t = typename meta_transform<Template,Traits,P...>::type;
    
    template<class TypeList,ptrdiff_t _index>
    using get_t = typename get<TypeList,std::integral_constant<ptrdiff_t,_final_index_v<_index,TypeList>>>::type;
    
    template<class...>
    struct merge
        :type_identity<undefined_t>
    {};
    
    template<template<class...>class TypeList,class...Types>
    struct merge<TypeList<Types...>>
        :type_identity<TypeList<Types...>>
    {};
    
    template<template<class...>class TypeList,class...A,class...B>
    struct merge<TypeList<A...>,TypeList<B...>>
        :type_identity<TypeList<A...,B...>>
    {};
    
    template<template<class...>class TypeList,class...A,class...B,class...C>
    struct merge<TypeList<A...>,TypeList<B...>,TypeList<C...>>
        :type_identity<TypeList<A...,B...,C...>>
    {};
    
    template<template<class...>class TypeList,class...A,class...B,class...C,class ... D>
    struct merge<TypeList<A...>,TypeList<B...>,TypeList<C...>,TypeList<D...>>
        :type_identity<TypeList<A...,B...,C...,D...>>
    {};
    
    template<template<class DataType,DataType...>class Sequence,class DataType,DataType..._value>
    struct merge<Sequence<DataType,_value...>>
        :type_identity<Sequence<DataType,_value...>>
    {};
    
    template<template<class DataType,DataType...>class Sequence,class DataType,
        DataType..._value,DataType..._b>
    struct merge<Sequence<DataType,_value...>,Sequence<DataType,_b...>>
        :type_identity<Sequence<DataType,_value...,_b...>>
    {};
    
    template<template<class DataType,DataType...>class Sequence,class DataType,
        DataType..._value,DataType..._b,DataType..._c>
    struct merge<Sequence<DataType,_value...>,Sequence<DataType,_b...>,Sequence<DataType,_c...>>
        :type_identity<Sequence<DataType,_value...,_b...,_c...>>
    {};
    
    template<template<class DataType,DataType...>class Sequence,class DataType,
        DataType..._value,DataType..._b,DataType..._c,DataType..._d>
    struct merge<Sequence<DataType,_value...>,Sequence<DataType,_b...>,Sequence<DataType,_c...>,Sequence<DataType,_d...>>
        :type_identity<Sequence<DataType,_value...,_b...,_c...,_d...>>
    {};
    
    template<class A,class B,class C,class D,class E,class...R>
    struct merge<A,B,C,D,E,R...>
        :merge<typename merge<A,B,C,D>::type,typename merge<E,R...>::type>
    {};
    
    template<class...Templates>
    using merge_t =typename merge<Templates...>::type;
    
    template<class TypeList,class Empty= empty_t<TypeList>>
    struct reverse
        :type_identity<Empty>
    {};
    
    template<template<class...>class TypeList,class First,class...Rest,class...Reverse>
    struct reverse<TypeList<First,Rest...>,TypeList<Reverse...>>
        :reverse<TypeList<Rest...>,TypeList<First,Reverse...>>
    {};
    
    template<template<class DataType,DataType...>class Sequence,class DataType,
        DataType _first,DataType..._rest,DataType..._reverse>
    struct reverse<Sequence<DataType,_first,_rest...>,Sequence<DataType,_reverse...>>
        :meta_transform<Sequence<DataType,_first,_rest...>,reverse>
    {};
    
    template<class T,class  U,class B>
    struct unique
    {
        using type = U;
        static constexpr bool value = B::value;
    };
    
    template<template<class...>class TypeList,class F,class...T,class...U,class B>
    struct unique<TypeList<F,T...>,TypeList<U...>,B>
        :std::conditional_t<(std::is_same_v<F,U>||...),unique<TypeList<T...>,TypeList<U...>,std::false_type >,
        unique<TypeList<T...>,TypeList<U...,F>,std::true_type >>
    {};
    
    template<template<class DataType,DataType...>class Sequence,class DataType,
        DataType _first,DataType..._rest>
    struct unique<Sequence<DataType,_first,_rest...>,meta_list<>,std::true_type >
        :meta_transform<Sequence<DataType,_first,_rest...>,unique,meta_list<>,std::true_type>
    {};
    
    template<class T>using unique_t = typename unique<T,empty_t<T>,std::true_type >::type;
    template<class T>using reverse_t = typename reverse<T>::type;
    template<class T>inline constexpr bool is_unique_v = unique<T,empty_t<T>,std::true_type>::value;


    static_assert(std::is_same_v<unique_t<meta_list<>>,meta_list<>>);
    static_assert(std::is_same_v<unique_t<meta_list<int,int>>,meta_list<int>>);
    static_assert(is_unique_v<std::index_sequence<>>);
    static_assert(is_unique_v<meta_list<>>);
}



#endif//cxx17
#undef MAKE
#undef STL_LANG
#undef STL_17
#undef STL_20
#endif //META_META_17_HPP

