// André Ribes EDF R&D - 2006
//

#include "provides_port.hxx"

provides_port::provides_port() 
{
  default_properties = new PortProperties_i();
}

provides_port::~provides_port() 
{
  delete default_properties;
}

Ports::PortProperties_ptr 
provides_port::get_port_properties() 
{
  return default_properties->_this();
}
