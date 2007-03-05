// Eric Fayolle - EDF R&D
// Modified by : $LastChangedBy$
// Date        : $LastChangedDate: 2007-02-28 15:26:32 +0100 (mer, 28 fév 2007) $
// Id          : $Id$

#ifndef _CALCIUM_COMPLEX_PORT_USES_HXX_
#define _CALCIUM_COMPLEX_PORT_USES_HXX_

#include "Calcium_Ports.hh"
#include "GenericUsesPort.hxx"

extern char _repository_Calcium_Complex_Port_name[];

class calcium_complex_port_uses :
  public virtual CalciumGenericUsesPort< seq_u_manipulation<Ports::Calcium_Ports::seq_float,
						     CORBA::Float >, 
				  Ports::Calcium_Ports::Calcium_Complex_Port, 
				  _repository_Calcium_Complex_Port_name 
				  >
{
  public :

  virtual ~calcium_complex_port_uses();

};

#endif
