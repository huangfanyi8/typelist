#include"filter.hpp"
#include<variant>
#include<tuple>

using std::variant;
using std::tuple;
template<class T>
using is_same_int=std::is_same<T,int>;

template<ptrdiff_t a,ptrdiff_t b>
using is_same_c=std::bool_constant<a==b>;

void Test_erase_c()
{
  using namespace common;
  static_assert(common::is_same_v<erase_t<std::index_sequence<1,2,3,4,5,6,7,8>,1000>,
    erase_t<std::index_sequence<1,2,3,4,5,6,7,8>,8>,std::index_sequence<1,2,3,4,5,6,7,8>>);
  static_assert(std::is_same_v<erase_t<std::index_sequence<1,2,3,4,5,6,7,8>,1>,std::index_sequence<1,3,4,5,6,7,8>>);
  static_assert(std::is_same_v<erase_t<std::index_sequence<1,2,3,4,5,6,7,8>,0>,std::index_sequence<2,3,4,5,6,7,8>>);
  static_assert(std::is_same_v<erase_t<std::index_sequence<1,2,3,4,5,6,7,8>,1>,std::index_sequence<1,3,4,5,6,7,8>>);
  static_assert(std::is_same_v<erase_t<std::index_sequence<1,2,3,4,5,6,7,8>,7>,std::index_sequence<1,2,3,4,5,6,7>>);
  static_assert(std::is_same_v<erase_t<std::index_sequence<1,2,3,4,5,6,7,8>,-1>,std::index_sequence<1,2,3,4,5,6,7>>);
  static_assert(std::is_same_v<erase_t<std::index_sequence<1,2,3,4,5,6,7,8>,-2>,std::index_sequence<1,2,3,4,5,6,8>>);
  static_assert(std::is_same_v<erase_t<std::index_sequence<1,2,3,4,5,6,7,8>,-2>,std::index_sequence<1,2,3,4,5,6,8>>);
  static_assert(std::is_same_v<erase_t<std::index_sequence<1,2,3,4,5,6,7,8>,-3>,std::index_sequence<1,2,3,4,5,7,8>>);
  static_assert(std::is_same_v<erase_t<std::index_sequence<1,2,3,4,5,6,7,8>,-4>,std::index_sequence<1,2,3,4,6,7,8>>);
  static_assert(std::is_same_v<erase_t<std::index_sequence<1,2,3,4,5,6,7,8>,-5>,std::index_sequence<1,2,3,5,6,7,8>>);
  static_assert(std::is_same_v<erase_t<std::index_sequence<1,2,3,4,5,6,7,8>,-6>,std::index_sequence<1,2,4,5,6,7,8>>);
  static_assert(std::is_same_v<erase_t<std::index_sequence<1,2,3,4,5,6,7,8>,-7>,std::index_sequence<1,3,4,5,6,7,8>>);
  static_assert(std::is_same_v<erase_t<std::index_sequence<1,2,3,4,5,6,7,8>,-8>,std::index_sequence<2,3,4,5,6,7,8>>);
  static_assert(std::is_same_v<erase_t<std::index_sequence<1,2,3,4,5,6,7,8>,-88>,std::index_sequence<1,2,3,4,5,6,7,8>>);
  static_assert(is_same_v<erase_t<variant<>,1>,variant<>,erase_t<variant<>,-1>,erase_t<variant<int>,0>>);
  static_assert(is_same_v<erase_t<variant<int>,1>,variant<int>>);
  static_assert(is_same_v<erase_t<variant<int,double>,1>,variant<int>,erase_t<variant<int,double>,-1>>);
}

void Test_erase_if()
{
  using namespace common;
  static_assert(is_same_v<std::index_sequence<>,equal_range_t<variant<>,std::is_same,int>>);
  static_assert(is_same_v<std::index_sequence<0>,equal_range_t<variant<int,double>,std::is_same,int>>);
  static_assert(is_same_v<std::index_sequence<>,equal_range_t<variant<char>,std::is_same,int>>);
  static_assert(is_same_v<std::index_sequence<>,equal_range_t<variant<>,std::is_same,int>>);
  static_assert(is_same_v<std::index_sequence<0,1,2,6>,
    equal_range_t<range_sequence<8,8,8,9,0,1,8>,is_same,constant<ptrdiff_t ,8>>>);
  
  static_assert(is_same_v<variant<int,double>,append_t<variant<int>,double>>);
  static_assert(is_same_v<variant<double>,append_t<variant<>,double>,insert_t<variant<>,0,double>>);
}

void Test_insert_if()
{

}

void Test_remove()
{
  using namespace common;
  
  static_assert(is_same_v<variant<>,
    remove_t<variant<int>,int>,
      remove_t<variant<>,int>>);
  
  static_assert(is_same_v<std::index_sequence<2,3,4,3,5,6,7>,
    remove_t<std::index_sequence<1,2,1,3,4,3,5,6,7>,index_constant<1>>>);
}

void Test_insert()
{
  using namespace common;
  static_assert(is_same_v<insert_t<variant<int,double,char>,1,void>,
                variant<int,void,double,char>
                >);
  static_assert(is_same_v<insert_t<variant<int,double,char>,0,void>,
                variant<void,int,double,char>
                >);
  static_assert(is_same_v<insert_t<variant<int,double,char>,2,void>,
                variant<int,double,void,char>
                >);
  
  static_assert(is_same_v<insert_t<variant<int,double,char>,3,void>,
                variant<int,double,char,void>
                >);
  
  static_assert(is_same_v<insert_t<variant<int,double,char>,111,void>,
                variant<int,double,char,void>
                >);
  
  static_assert(is_same_v<insert_t<variant<>,5555,void,int>,
                variant<void,int>
                >);
  
  static_assert(is_same_v<insert_t<variant<double*>,5555,void,int>,
                variant<double*,void,int>
                >);
  
  static_assert(is_same_v<prepend_t<variant<int>,double>,variant<double,int>>);
  
  
  static_assert(is_same_v<insert_t<variant<>,-1,char*>,variant<char*>>);
  
  static_assert(is_same_v<insert_t<variant<int,char>,-11,char*>,variant<char*,int,char>>);
  static_assert(is_same_v<insert_t<variant<int,char>,-1,char*>,variant<int,char*,char>>);
  static_assert(is_same_v<insert_t<variant<int,char>,-2,char*>,variant<char*,int,char>>);
  
  static_assert(is_same_v<insert_t<std::index_sequence<1,2,3,4,5>,0,std::index_sequence<>>,
    std::index_sequence<1,2,3,4,5>>);
  
  static_assert(is_same_v<
    insert_t<std::index_sequence<>,0,std::index_sequence<1,2,3,4,5>>,
    std::index_sequence<1,2,3,4,5>,
    insert_t<std::index_sequence<1,3,4,5>,1,std::index_sequence<2>>,
    insert_t<std::index_sequence<1,2,4,5>,2,std::index_sequence<3>>,
    insert_t<std::index_sequence<1,2,3,5>,3,std::index_sequence<4>>,
    insert_t<std::index_sequence<1,2,3,4>,4,std::index_sequence<5>>,
    insert_t<std::index_sequence<1,2,3,4>,5,std::index_sequence<5>>
    >);
  
  static_assert(is_same_v<
    insert_t<std::index_sequence<>,-1,std::index_sequence<2>>,
    insert_t<std::index_sequence<>,-2,std::index_sequence<2>>,
    std::index_sequence<2>
    >);
  
  static_assert(is_same_v<
    insert_t<std::index_sequence<1>,-1,std::index_sequence<2>>,
    insert_t<std::index_sequence<1>,-2,std::index_sequence<2>>,
    std::index_sequence<2,1>
  >);
}

int main()
{
  using namespace common;
  
  static_assert(is_same_v<variant<char,int>,
                
                insert_if_t<variant<int>,is_same_int,variant<char>>>);
  
  static_assert(is_same_v<variant<>,
                
                insert_if_t<variant<>,is_same_int,variant<char>>>);
  
  static_assert(is_same_v<variant<char,int,char,int>,
                
                insert_if_t<variant<int,int>,is_same_int,variant<char>>>);
  
  static_assert(is_same_v<variant<char,int,char,int,float>,
    
    insert_if_t<variant<int,int,float>,is_same_int,variant<char>>>);


}