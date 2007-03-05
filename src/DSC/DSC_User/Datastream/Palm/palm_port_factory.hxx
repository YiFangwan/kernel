// Eric Fayolle - EDF R&D
// Modified by : $LastChangedBy$
// Date        : $LastChangedDate: 2007-01-08 19:01:14 +0100 (lun, 08 jan 2007) $
// Id          : $Id$


#ifndef _PALM_PORT_FACTORY_HXX_
#define _PALM_PORT_FACTORY_HXX_

#include "uses_port.hxx"
#include "provides_port.hxx"
#include <string>

#include "palm_data_short_port_provides.hxx"
#include "palm_data_seq_short_port_provides.hxx"

using namespace std;

class palm_port_factory
{
  public:
    palm_port_factory();
    virtual ~palm_port_factory();

    virtual provides_port * create_data_servant(string type); 
    virtual uses_port * create_data_proxy(string type); 
};

#endif

