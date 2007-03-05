// Eric Fayolle - EDF R&D
// Modified by : $LastChangedBy$
// Date        : $LastChangedDate: 2007-01-08 19:01:14 +0100 (lun, 08 jan 2007) $
// Id          : $Id$

#ifndef _ITERATOR_TRAITS_HXX_
#define _ITERATOR_TRAITS_HXX_

// Trait permettant d'accéder au type de donnée stockée 
// dans une map
template < typename MapIterator >
struct iterator_t {
  typedef typename MapIterator::value_type value_type1;
  typedef typename value_type1::second_type value_type;
};

#endif
