// André Ribes EDF R&D - 2006
//

#ifndef _DATA_SHORT_PORT_USES_HXX_
#define _DATA_SHORT_PORT_USES_HXX_

#include "uses_port.hxx"
#include "SALOME_Ports.hh"

class data_short_port_uses :
  public virtual uses_port
{
  public :
    data_short_port_uses();
    virtual ~data_short_port_uses();

    virtual const char * get_repository_id();
    virtual bool set_port(Ports::Port_ptr port);

    virtual void put(CORBA::Short data);

    virtual void uses_port_changed(Engines::DSC::uses_port * new_uses_port,
				   const Engines::DSC::Message message);

  private :
    Ports::Data_Short_Port_ptr _my_port;
    Engines::DSC::uses_port * _my_ports;
};

#endif

