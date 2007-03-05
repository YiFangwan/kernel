#ifndef _PALM_DATA_SHORT_PORT_PROVIDES_HXX_
#define _PALM_DATA_SHORT_PORT_PROVIDES_HXX_

#include "Palm_Ports.hh"
#include "provides_port.hxx"
#include "GenericPort.hxx"
#include "PalmCouplingPolicy.hxx"

class palm_data_short_port_provides :
  public virtual POA_Ports::Palm_Ports::Palm_Data_Short_Port,
  public virtual provides_port,
  public virtual GenericPort< atom_manipulation< CORBA::Short >, PalmCouplingPolicy >

{
  typedef CORBA::Short                                 CorbaDataType;
  typedef atom_manipulation < CorbaDataType >          DataManipulator;
  typedef GenericPort< DataManipulator, PalmCouplingPolicy > Port;

  public :
    palm_data_short_port_provides() {}
    virtual ~palm_data_short_port_provides() {}

  void put(DataManipulator::CorbaInType data, CORBA::Long time, CORBA::Long tag) {
    Port::put(data, time, tag);
  }

  virtual Ports::Port_ptr get_port_ref() {
    return _this();
  }
};

#endif
