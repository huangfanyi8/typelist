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

#if STL_17

namespace cxx17
{
    struct undefined_t
    {
        constexpr explicit undefined_t() = default;
    };
    
    inline constexpr undefined_t undefined{};
}

namespace cxx17
{
    using index_t = ptrdiff_t;
    using signed_size_type = ptrdiff_t;
    
    template<class...O>
    struct  list
    {
    private:
        template<class...>struct _impl{};
        
        template<template<class...>class TL,class...T>
        struct _impl<TL<T...>>
        {using type = TL<O...>;};
    public:
        template<class Other>using type = typename _impl<Other>::type;
    };
    
    template<class T>
    struct identity
    {using type =T;};
    
    template<class>struct template_traits;
    
    template<class...T>
    struct template_traits<list<T...>>
    {
        using type = list<>;
        static constexpr size_t extent = sizeof...(T);
        using value_type = void;
    };
    
    template<class T,T ... _value>
    struct template_traits<std::integer_sequence<T,_value...>>
    {
        using type = std::integer_sequence<T>;
        static constexpr size_t extent = sizeof...(_value);
        using value_type = T;
    };
    
    template<class T>using empty_t = typename template_traits<T>::type;
    template<class T>using traits_value_t = typename template_traits<T>::value_type;
    template<class T>inline constexpr size_t extent_v = template_traits<T>::extent;
}

namespace cxx17::_details
{
    template<auto _index,class T>
    struct _pair
    {
        static constexpr auto  value = _index;
        using type = T;
    };
    
    template<auto _index,class T>
    struct _pair<_index,std::in_place_type_t<T>>
    {
        static constexpr auto  value = _index;
        using type = T;
    };
    
    template<class P>using _pair_t = typename P::type;
    template<class P>inline constexpr auto  _pair_v = P::value;
    
    template<class>struct _make_pair_list;
    
    template<class...List>
    struct _make_pair_list<list<List...>>
    {
        template<class>
        struct _impl{};
        
        template<size_t..._index>
        struct _impl<std::index_sequence<_index...>>
        {using type = list<_pair<index_t (_index),List>...>;};
        
        using _id_sequence = std::index_sequence_for<List...>;
        using type = typename _impl<_id_sequence>::type;
        static constexpr bool value =true;
    };
    
    template<class T,T..._value>
    struct _make_pair_list<std::integer_sequence<T,_value...>>
    {
        template<class>
        struct _impl{};
        
        template<size_t..._index>
        struct _impl<std::index_sequence<_index...>>
        {using type = list<_pair<index_t (_index),std::integral_constant<T,_value>>...>;};
        
        using _id_sequence = std::make_index_sequence<sizeof...(_value)>;
        using type = typename _impl<_id_sequence>::type;
        static constexpr bool value =false;
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
        {};
        
        template<class U,U..._>
        struct _impl<list<std::integral_constant<U,_>...>>
            :identity<std::integer_sequence<U,_...>>
        {};
    public:
        using type = typename _impl<typename Tr<_list,Other...>::type>::type;
    };
}

namespace cxx17
{
    template<class F,class ... T>
    inline constexpr bool is_same_v = (std::is_same_v<F,T>&&...);
    
    namespace _details
    {
        enum class _range{_less,_greater,_no};
        
        template<ptrdiff_t _input_index,class TL>
        inline constexpr ptrdiff_t  _final_index_v = (_input_index>=0)?_input_index:_input_index+ptrdiff_t(extent_v<TL>);
        
        template<ptrdiff_t _input_index,class TL>
        inline constexpr _range _is_out_of_range = extent_v<TL><=_final_index_v<_input_index,TL>
            ?_range::_greater
            :0>_final_index_v<_input_index,TL>
            ?_range::_less:_range::_no;
    }
}

namespace cxx17
{
    template<class,class...>
    struct merge
    {using type = list<>;};
    
    template<class...A>
    struct merge<void,list<A...>>
    {using type=list<A...>;};
    
    template<class...T>using merge_t = typename merge<void,T...>::type;
    
    template<class...A,class...B>
    struct merge<void,list<A...>,list<B...>>
    {using type=list<A...,B...>;};
    
    template<class...A,class...B,class...C>
    struct merge<void,list<A...>,list<B...>,list<C...>>
    {using type=list<A...,B...,C...>;};
    
    template<class...A,class...B,class...C,class...D>
    struct merge<void,list<A...>,list<B...>,list<C...>,list<D...>>
    {using type=list<A...,B...,C...,D...>;};
    
    template<class...A,class...B,class...C,class...D,class...E>
    struct merge<void,list<A...>,list<B...>,list<C...>,list<D...>,list<E...>>
    {using type=list<A...,B...,C...,D...,E...>;};
    
    template<class...A,class...B,class...C,class...D,class...E,class...F>
    struct merge<void,list<A...>,list<B...>,list<C...>,list<D...>,list<E...>,list<F...>>
    {using type=list<A...,B...,C...,D...,E...,F...>;};
    
    template<class...A,class...B,class...C,class...D,class...E,class...F,class...G>
    struct merge<void,list<A...>,list<B...>,list<C...>,list<D...>,list<E...>,list<F...>,list<G...>>
    {using type=list<A...,B...,C...,D...,E...,F...,G...>;};
    
    template<class...A,class...B,class...C,class...D,class...E,class...F,class...G,class...H>
    struct merge<void,list<A...>,list<B...>,list<C...>,list<D...>,list<E...>,list<F...>,list<G...>,list<H...>>
    {using type=list<A...,B...,C...,D...,E...,F...,G...,H...>;};
    
    template<class...A,class...B,class...C,class...D,class...E,class...F,class...G,class...H,class...R>
    struct merge<void,list<A...>,list<B...>,list<C...>,list<D...>,list<E...>,list<F...>,list<G...>,list<H...>,R...>
    {using type=merge_t<list<A...>,list<B...>,list<C...>,list<D...>,list<E...>,list<F...>,list<G...>,list<H...>,merge_t<R...>>;};
}

namespace cxx17
{
    template<class TL,index_t..._input_index>
    struct in_range
    {
    public:
        enum  range{less,greater,in};
    private:
        template<class,class = void>struct _impl:std::false_type {};
        
        template<range..._value>
        struct _impl<std::integer_sequence<range,_value...>,std::enable_if_t<((_value==in)&&...)>>
            :std::true_type
        {};
    public:
        static constexpr index_t size = index_t (extent_v<TL>);
        using type = std::integer_sequence<range,((extent_v<TL><=_details::_final_index_v<_input_index,TL>)
                                                    ?greater
                                                    :0>_details::_final_index_v<_input_index,TL>
                                                    ?less
                                                    :in)...>;
        
        static constexpr bool value = _impl<type>::value;
    };
}

namespace cxx17
{
    template<index_t ,class,class U = undefined_t,class =void >struct get{using type = U;};
    
    template<class A,class...R>struct get<0,list<A,R...>>{using type = A;};
    template<class A,class B,class...R>struct get<1,list<A,B,R...>>{using type = B;};
    template<class A,class B,class C,class...R>struct get<2,list<A,B,C,R...>>{using type =C;};
    template<class A,class B,class C,class D,class...R>struct get<3,list<A,B,C,D,R...>>{using type = D;};
    template<class A,class B,class C,class D,class E,class...R>struct get<4,list<A,B,C,D,E,R...>>{using type = E;};
    template<class A,class B,class C,class D,class E,class F,class...R>struct get<5,list<A,B,C,D,E,F,R...>>{using type = F;};
    template<class A,class B,class C,class D,class E,class F,class G,class...R>struct get<6,list<A,B,C,D,E,F,G,R...>>{using type = G;};
    template<class A,class B,class C,class D,class E,class F,class G,class H,class...R>struct get<7,list<A,B,C,D,E,F,G,H,R...>>{using type = H;};
    
    template<ptrdiff_t _index,class A,class B,class C,class D,class E,class F,class G,class H,class...R,class U>
    struct get<_index,list<A,B,C,D,E,F,G,H,R...>,U,std::enable_if_t<(_index>=8)>>
        :get<_index-8,list<R...>>
    {};
    
    template<ptrdiff_t _index,class T,class U = undefined_t>
    using get_t = typename get<_details::_final_index_v<_index,T>,T,U>::type;
    
    template<ptrdiff_t _index,class T,T..._value,class U>
    struct get<_index,std::integer_sequence<T,_value...>,U>
    {
    private:
        using _type = get_t<_index,list<std::integral_constant<T,_value>...>,U>;
        
        template<class>struct _impl{static constexpr auto value = _type::value;};
        
        template<>struct _impl<undefined_t>{static constexpr undefined_t value = undefined;};
    public:
        static constexpr auto value = _impl<_type>::value;
    };
    
    template<index_t _index,class T,class U =undefined_t, class =void>
    inline  constexpr auto get_v =get<_index,T,U>::value;
    
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
}

namespace cxx17
{
    template<class,ptrdiff_t,class...>struct _insert;
    
    template<class,class...>struct append;
    
    template<class...T,class...O>
    struct append<list<T...>,O...>
        :identity<list<T...,O...>>
    {};
    
    template<class T,class... Add>using append_t =typename append<T,Add...>::type;
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
        template<class T,index_t _left,index_t _right,bool = in_range<T,_left,_right>::value>struct _swap;
        
        template<class T,index_t _left,index_t _right>
        struct _swap<T,_left,_right, false>
            :identity<T>
        {};
        
        template<class T,index_t _left,index_t _right>
        struct _swap<T,_left,_right, true>
        {
        private:
            using _list = _details::_make_pair_list_t<T>;
            static constexpr index_t _left_index = _details::_final_index_v<_left,T>;
            static constexpr index_t _right_index = _details::_final_index_v<_right,T>;
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
        
        template<class T,index_t _left,index_t  _right>using _swap_t = typename _swap<T,_left,_right>::type;
    }
    template<class T,size_t _l,size_t _r>
    using swap_t = _details::_swap_t<T,_l,_r>;
}

namespace cxx17
{
    namespace _details
    {
        template<class T,index_t _current,index_t _end,bool = (_current*2+1<=_end),bool = (_current*2+2<=_end)>
        struct _heap_sort_heapify_helper
            :std::integral_constant<index_t ,_current>
        {};
        
        template<class T,index_t _current,index_t _end>
        struct _heap_sort_heapify_helper<T,_current,_end,true,true>
        {
        private:
            static constexpr index_t left = 2*_current+1;
            static constexpr index_t right = left+1;
            static constexpr auto left_v = get_v<left,T>;
            static constexpr auto right_v = get_v<right,T>;
            static constexpr auto current_v = get_v<_current,T>;;
            static constexpr auto value1 = current_v<left_v?left:_current;
            static constexpr auto value2 = get_v<value1,T><right_v?right:value1;
        public:
            static constexpr auto value = value2;
        };
        
        template<class T,index_t _current,index_t _end>
        struct _heap_sort_heapify_helper<T,_current,_end,true,false>
        {
        private:
            static constexpr index_t left = 2*_current+1;
            static constexpr index_t right = left+1;
            static constexpr auto left_v = get_v<left,T>;
            static constexpr auto right_v = get_v<right,T>;
            static constexpr auto current_v = get_v<_current,T>;
            static constexpr auto value1 = current_v<left_v?left:_current;
        public:
            static constexpr auto value = value1;
        };
        
        template<class T,index_t _current,index_t _end>
        inline constexpr index_t  _heap_sort_heapify_helper_v = _heap_sort_heapify_helper<T,_current,_end>::value;
        
        template<class T,index_t _current,index_t _end,index_t _max = _details::_heap_sort_heapify_helper_v<T,_current,_end>>
        struct _heap_sort_heapify
            :std::conditional_t<_max!=_current,
            _heap_sort_heapify<_swap_t<T,_max,_current>,_max,_end>,
            identity<T>>
        {};
        
        template<class T,index_t _current,index_t _end>
        using  _heap_sort_heapify_t = typename _heap_sort_heapify<T,_current,_end>::type;
        
        template<class T,index_t _index>
        struct _make_heap
            :std::conditional_t<(_index>=0),
            _make_heap<_heap_sort_heapify_t<T,_index,index_t (extent_v<T>)-1>,_index-1>,identity<T>>
        {};
        
        template<class T>using _make_heap_t =typename _make_heap<T,index_t(extent_v<T>)/2-1>::type;
        
        template<class T>
        struct _heap_sort
        {
            using _type = _make_heap_t<T>;
            
            template<class U,index_t _index>
            struct _impl
                :_impl<_heap_sort_heapify_t <_swap_t<U,0,_index>,0,_index-1>,_index-1>
            {};
            
            template<class U>
            struct _impl<U,0>
                :identity<U>
            {};
            
            using type =typename _impl<_type,index_t (extent_v<T>)-1>::type;
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
        
        template<index_t  _begin,bool = _begin*2+1<_extent,bool = _begin*2+2<_extent>
        struct _impl
            :std::true_type
        {};
        
        template<index_t _begin>
        struct _impl<_begin,true,false>
            :std::bool_constant<((get_v<_begin,T>)>=get_v<_begin*2+1,T>)>
        {};
        
        template<index_t _begin>
        struct _impl<_begin,true,true>
            :std::bool_constant<((get_v<_begin,T>)>=get_v<_begin*2+1,T>)&&((get_v<_begin,T>)>=get_v<_begin*2+2,T>)>
        {};
        
        template<index_t _begin>
        struct _pimpl
            :std::conditional_t<_begin<=_end,std::conditional_t<_impl<_begin>::value,_pimpl<_begin+1>,std::false_type>,std::true_type >
        {};
        
        static constexpr bool value = _pimpl<0>::value;
    };
    
    static_assert(is_max_heap<make_heap_t<std::index_sequence<1,4,3,6,8,9,4,7,22>>>::value);
}

#endif//cxx17
#undef MAKE
#undef STL_LANG
#undef STL_17
#undef STL_20
#endif //META_META_17_HPP
