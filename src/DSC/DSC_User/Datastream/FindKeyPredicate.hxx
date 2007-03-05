// Eric Fayolle - EDF R&D
// Modified by : $LastChangedBy$
// Date        : $LastChangedDate: 2007-01-08 19:01:14 +0100 (lun, 08 jan 2007) $
// Id          : $Id$

#ifndef __FIND_KEY_PREDICATE__
#define __FIND_KEY_PREDICATE__

#include <functional>
#include <utility>
#include "DisplayPair.hxx"

template < typename T >
struct FindKeyPredicate : public std::unary_function < T, bool > 
{
  T _value;
  FindKeyPredicate(const T& value):_value(value){}
  inline bool operator()(const T &v1, const T& v2) const {
    std::cout << "FindKeyPredicate Generic -> :" << &(v1.first) << std::endl;
    return ( v1 == _value );
  }
};

// Pour les MAPs avec une clef sous forme de pair
// template <typename T1, typename T2, typename T3>
// struct FindKeyPredicate<  std::pair<const std::pair<T1,T2>, T3 > > : 
//   public std::binary_function < std::pair<const std::pair<T1,T2>, T3 >,
// 				std::pair<const std::pair<T1,T2>, T3 >, bool > 
// {
//   std::pair<T1,T2> _value;
//   FindKeyPredicate(const std::pair<T1,T2> & value):_value(value){
//     std::cout << "1-Initializing with value " << _value << std::endl;
//   }
//   bool operator()( const std::pair<const std::pair<T1,T2>, T3 > & v1,  
// 		   const std::pair<const std::pair<T1,T2>, T3 > v2) const {
//     std::cout << "1-> :" << v1 << "," << v2 << " " << std::endl;
//     return (v1.first <= _value ) && (_value < v2.first) ;
//   }
// };

template <typename T1, typename T2>
struct FindKeyPredicate<  std::pair<T1,T2> > : public std::unary_function < std::pair<T1,T2>, bool > 
{
  T1 _value;
  FindKeyPredicate(const T1 & value):_value(value){
    std::cout << "FindKeyPredicate 2-Initializing with value " << _value << std::endl;
  }
  
  inline bool operator()( const std::pair<T1,T2> & v1) const {
    std::cout << "FindKeyPredicate 2-> :" << v1.first << std::endl;
    return v1.first == _value  ;
  }
};

#endif
