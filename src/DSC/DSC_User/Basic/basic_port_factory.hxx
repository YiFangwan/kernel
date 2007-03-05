// André Ribes - EDF R&D 2006
//

#ifndef _BASIC_PORT_FACTORY_HXX_
#define _BASIC_PORT_FACTORY_HXX_

#include "data_short_port_provides.hxx"
#include "data_short_port_uses.hxx"

using namespace std;

class basic_port_factory
{
  public:
    basic_port_factory();
    virtual ~basic_port_factory();

    virtual provides_port * create_data_servant(string type); 
    virtual uses_port * create_data_proxy(string type); 
};

#endif

