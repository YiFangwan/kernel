// Eric Fayolle - EDF R&D
// Modified by : $LastChangedBy$
// Date        : $LastChangedDate: 2007-01-08 19:01:14 +0100 (lun, 08 jan 2007) $
// Id          : $Id$

#ifndef __DISPLAY_PAIR__
#define __DISPLAY_PAIR__

#include <iostream>

template <typename T, typename U> std::ostream & operator <<(std::ostream & os, const std::pair<T,U> & p)
{ os << "(" << p.first << "," << p.second << ")";
  return os;
};


#endif
