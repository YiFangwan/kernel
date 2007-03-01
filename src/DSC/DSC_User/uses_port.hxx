// André Ribes - EDF R&D - 2006
//

// Tous les proxy hérites de cette classe
// Elle permet d'identifier un proxy, de gérer 
// la référence réelle de l'objet distant ainsi qu'un support 
// aux ports multiples

#ifndef _USES_PORT_HXX_
#define _USES_PORT_HXX_

#include "base_port.hxx"
#include "SALOME_Ports.hh"
#include "DSC_Engines.hh"
#include "PortProperties_i.hxx"

class uses_port : public base_port
{
  public :
    uses_port();
    virtual ~uses_port();

    virtual const char * get_repository_id() = 0;
    virtual bool set_port(Ports::Port_ptr port) = 0;

    virtual void uses_port_changed(Engines::DSC::uses_port * new_uses_port,
				   const Engines::DSC::Message message) = 0;
    virtual Ports::PortProperties_ptr get_port_properties();

  private :
    PortProperties_i * default_properties;
};

#endif

