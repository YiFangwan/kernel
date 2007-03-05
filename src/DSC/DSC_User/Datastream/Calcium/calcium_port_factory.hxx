// Eric Fayolle - EDF R&D
// Modified by : $LastChangedBy$
// Date        : $LastChangedDate: 2007-03-01 13:36:05 +0100 (jeu, 01 mar 2007) $
// Id          : $Id$


#ifndef _CALCIUM_PORT_FACTORY_HXX_
#define _CALCIUM_PORT_FACTORY_HXX_

#include "uses_port.hxx"
#include "provides_port.hxx"
#include <string>

#include "calcium_port_provides.hxx"

#include "calcium_integer_port_uses.hxx"
#include "calcium_real_port_uses.hxx"
#include "calcium_double_port_uses.hxx"
#include "calcium_string_port_uses.hxx"
#include "calcium_logical_port_uses.hxx"
#include "calcium_complex_port_uses.hxx"

using namespace std;

class calcium_port_factory
{
  public:
    calcium_port_factory();
    virtual ~calcium_port_factory();

    virtual provides_port * create_data_servant(string type); 
    virtual uses_port * create_data_proxy(string type); 
};

#endif

