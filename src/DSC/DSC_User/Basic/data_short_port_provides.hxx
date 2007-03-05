// André Ribes EDF R&D - 2006
// 

#ifndef _DATA_SHORT_PORT_PROVIDES_HXX_
#define _DATA_SHORT_PORT_PROVIDES_HXX_

#include <iostream>
#include "SALOME_Ports.hh"
#include "provides_port.hxx"

class data_short_port_provides :
  public virtual POA_Ports::Data_Short_Port,
  public virtual provides_port
{
  public :
    data_short_port_provides();
    virtual ~data_short_port_provides();

    virtual void put(CORBA::Short data);
    virtual CORBA::Short get();
    virtual CORBA::Short get_local();

    virtual Ports::Port_ptr get_port_ref();

  private :
    CORBA::Short _val;
};

#endif
