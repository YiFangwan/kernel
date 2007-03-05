// Eric Fayolle - EDF R&D
// Modified by : $LastChangedBy$
// Date        : $LastChangedDate: 2007-02-28 15:26:32 +0100 (mer, 28 fév 2007) $
// Id          : $Id$

#ifndef _CALCIUM_USES_PORT_HXX_
#define _CALCIUM_USES_PORT_HXX_

#include "uses_port.hxx"

class calcium_uses_port : public uses_port
{
public :
  calcium_uses_port();
  virtual ~calcium_uses_port();
  virtual void disconnect (bool provideLastGivenValue) {};
};

#endif

