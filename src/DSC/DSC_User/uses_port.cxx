// André Ribes EDF R&D - 2006
//

#include "uses_port.hxx"

uses_port::uses_port() 
{
  default_properties = new PortProperties_i();
}

uses_port::~uses_port() 
{
  delete default_properties;
}

Ports::PortProperties_ptr 
uses_port::get_port_properties() 
{
  return default_properties->_this();
}
