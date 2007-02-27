// André Ribes EDF R&D - 2007
// 
#ifndef _PORTPROPERTIES_I_HXX_
#define _PORTPROPERTIES_I_HXX_

#include "SALOME_Ports.hh"

class PortProperties_i:
  public virtual POA_Ports::PortProperties
{
  public:
    PortProperties_i();
    virtual ~PortProperties_i();

    virtual void set_property(const char * name, const CORBA::Any& value)
      throw (Ports::NotDefined, Ports::BadType);
    virtual CORBA::Any* get_property(const char* name)
      throw (Ports::NotDefined);
};

#endif


