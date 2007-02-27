// André Ribes EDF R&D - 2006
// 

#include "PortProperties_i.hxx"

PortProperties_i::PortProperties_i() {}

PortProperties_i::~PortProperties_i() {}

void
PortProperties_i::set_property(const char * name, const CORBA::Any& value)
  throw (Ports::NotDefined, Ports::BadType)
{
  // Default ...
  throw Ports::NotDefined();
}

CORBA::Any* 
PortProperties_i::get_property(const char* name)
  throw (Ports::NotDefined)
{
  // Default ...
  throw Ports::NotDefined();
}

