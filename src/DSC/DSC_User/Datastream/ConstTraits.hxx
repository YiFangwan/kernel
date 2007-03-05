// Eric Fayolle - EDF R&D
// Modified by : $LastChangedBy$
// Date        : $LastChangedDate: 2007-01-08 19:01:14 +0100 (lun, 08 jan 2007) $
// Id          : $Id$

#ifndef _CONST_TRAITS_HXX_
#define _CONST_TRAITS_HXX_

template < typename T > struct ConstTrait { typedef  T NonConstType; };
template < typename T > struct ConstTrait < const T > {  typedef  T NonConstType; };

#endif
