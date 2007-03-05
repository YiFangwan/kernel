// Eric Fayolle - EDF R&D
// Modified by : $LastChangedBy$
// Date        : $LastChangedDate: 2007-01-08 19:01:14 +0100 (lun, 08 jan 2007) $
// Id          : $Id$

#ifndef _PALM_DATA_SEQ_SHORT_PORT_PROVIDES_HXX_
#define _PALM_DATA_SEQ_SHORT_PORT_PROVIDES_HXX_

#include "Palm_Ports.hh"
#include "provides_port.hxx"
#include "GenericPort.hxx"
#include "PalmCouplingPolicy.hxx"

class palm_data_seq_short_port_provides :
  public virtual POA_Ports::Palm_Ports::Palm_Data_Seq_Short_Port,
  public virtual provides_port,
  public virtual GenericPort< seq_u_manipulation<Ports::Palm_Ports::seq_short, CORBA::Short>,
  			      PalmCouplingPolicy >
{
  typedef Ports::Palm_Ports::seq_short                       CorbaDataType;
  typedef seq_u_manipulation<CorbaDataType, CORBA::Short>    DataManipulator;
  typedef GenericPort< DataManipulator, PalmCouplingPolicy > Port;
  
  public :
    palm_data_seq_short_port_provides() {}
    virtual ~palm_data_seq_short_port_provides() {}

  void put(DataManipulator::CorbaInType data, CORBA::Long time, CORBA::Long tag) {
    Port::put(data, time, tag);
  }

  Ports::Port_ptr get_port_ref() {
    return _this();
  }
};

#endif
