#include"../erase.hpp"

template<class...>class TypeList{};
template<class...>class TL{};

int main()
{
    using namespace meta_base;

    using LongList=TypeList<int,void*,double,char*,float,int&,long&>;
    using LongSequence=meta_base::char_sequence<'1','2','3','4','5','6','7','8'>;
    
    static_assert(is_same_v<TL<>,erase_t<TL<>,-1>>);
  
    //static_assert(is_same_v<TL<>,erase_t<TL<int>,-3>>);
    
    static_assert(is_same_v<erase_t<TypeList<>,98>,erase_t<TypeList<>,0>,erase_t<TypeList<>,9>,TypeList<>,erase_t<TypeList<>,-1>>);
    static_assert(is_same_v<erase_t<TypeList<int>,0>,TypeList<>>);
    static_assert(is_same_v<erase_t<TypeList<int,double>,1>,TypeList<int>>);
    static_assert(is_same_v<erase_t<LongList,1>,TypeList<int,double,char*,float,int&,long&>>);
    static_assert(is_same_v<erase_t<LongList,2>,TypeList<int,void*,char*,float,int&,long&>>);
    static_assert(is_same_v<erase_t<LongList,0>,TypeList<void*,double,char*,float,int&,long&>>);
    static_assert(is_same_v<erase_t<LongList,5>,TypeList<int,void*,double,char*,float,long&>>);
    static_assert(is_same_v<erase_t<LongList,extent_v<LongList>-1>,TypeList<int,void*,double,char*,float,int&>>);
  
    static_assert(is_same_v<erase_t<LongList,-1>,erase_t<LongList,6>>);
  
    static_assert(is_same_v<erase_t<LongList,-2>,erase_t<LongList,5>>);
  
    static_assert(is_same_v<erase_t<LongList,-3>,erase_t<LongList,4>>);
  
    static_assert(is_same_v<erase_t<LongList,-4>,erase_t<LongList,3>>);
  
    static_assert(is_same_v<erase_t<LongList,-5>,erase_t<LongList,2>>);
  
    static_assert(is_same_v<erase_t<LongList,-6>,erase_t<LongList,extent_v<LongList>-6>>);

    static_assert(is_same_v<TypeList<int>,pop_back_t<TypeList<int,double>>>);

    static_assert(is_same_v<erase_t<char_sequence<>,55>,char_sequence<>>);
    static_assert(is_same_v<erase_t<LongSequence ,5>,meta_base::char_sequence<'1','2','3','4','5','7','8'>>);
    static_assert(is_same_v<erase_t<LongSequence ,1>,meta_base::char_sequence<'1','3','4','5','6','7','8'>>);
    static_assert(is_same_v<erase_t<LongSequence ,0>,meta_base::char_sequence<'2','3','4','5','6','7','8'>>);
    static_assert(is_same_v<erase_t<LongSequence ,7>,meta_base::char_sequence<'1','2','3','4','5','6','7'>>);
    static_assert(is_same_v<pop_back_t<LongSequence >,meta_base::char_sequence<'1','2','3','4','5','6','7'>>);
    static_assert(is_same_v<pop_front_t<LongSequence >,meta_base::char_sequence<'2','3','4','5','6','7','8'>>);
    static_assert(is_same_v<pop_back_t<char_sequence<>>,char_sequence<>>) ;
    static_assert(is_same_v<pop_back_t<char_sequence<'l'>>,char_sequence<>>) ;
    static_assert(is_same_v<pop_front_t<char_sequence<'9','7'>>,char_sequence<'7'>>) ;
    static_assert(is_same_v<pop_front_t<char_sequence<>>,char_sequence<>>) ;
    static_assert(is_same_v<pop_front_t<char_sequence<'l'>>,char_sequence<>>) ;
    static_assert(is_same_v<pop_back_t<int_sequence<-1,-2,-3,4,5,6,7>>,int_sequence<-1,-2,-3,4,5,6>>) ;
    //static_assert(is_same_v<erase_t<int_sequence<5> ,-5>,
   //         int_sequence<>>);
}