// André Ribes - EDF R&D - 2006
//

#ifndef _PROVIDES_PORT_HXX_
#define _PROVIDES_PORT_HXX_

#include "base_port.hxx"
#include "SALOME_Ports.hh"
#include "DSC_Engines.hh"
#include "PortProperties_i.hxx"

class provides_port : public base_port
{
  public :
    provides_port();
    virtual ~provides_port();

    virtual Ports::Port_ptr get_port_ref() = 0;
    virtual void provides_port_changed(int connection_nbr,
				       const Engines::DSC::Message message) {}
    virtual Ports::PortProperties_ptr get_port_properties();

  private :
    PortProperties_i * default_properties;
};

#endif

