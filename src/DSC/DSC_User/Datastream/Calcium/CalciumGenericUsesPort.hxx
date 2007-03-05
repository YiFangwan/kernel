// Eric Fayolle - EDF R&D
// Modified by : $LastChangedBy$
// Date        : $LastChangedDate: 2007-02-28 15:26:32 +0100 (mer, 28 f√©v 2007) $
// Id          : $Id$

#ifndef _CALCIUM_GENERIC_USES_PORT_HXX_
#define _CALCIUM_GENERIC_USES_PORT_HXX_

#include "GenericUsesPort.hxx"
#include "calcium_uses_port.hxx"

template <typename DataManipulator, typename CorbaPortType, char * repositoryName > 
class CalciumGenericUsesPort : public GenericUsesPort<DataManipulator,CorbaPortType, repositoryName,
					       calcium_uses_port >
{
public :
  virtual ~CalciumGenericUsesPort() {};
  void disconnect(bool provideLastGivenValue);

};


template <typename DataManipulator,typename CorbaPortType, char * repositoryName > 
void
CalciumGenericUsesPort< DataManipulator,CorbaPortType, repositoryName >::disconnect(bool provideLastGivenValue ) {

  typedef typename CorbaPortType::_ptr_type CorbaPortTypePtr;

  if (!_my_ports)
    throw DSC_Exception(LOC("There is no connected provides port to communicate with."));

  for(int i = 0; i < _my_ports->length(); i++) {
    CorbaPortTypePtr port = CorbaPortType::_narrow((*_my_ports)[i]);
    try {
      std::cerr << "-------- CalciumGenericUsesPort<>::disconnect"<< std::endl;

      port->disconnect(provideLastGivenValue);
    } catch(const CORBA::SystemException& ex){
      throw DSC_Exception(LOC(OSS() << "Impossible d'invoquer la mÈthode disconnect sur le port provide n∞"
			      << i << "( i>=  0)"));
    }
  }
  
}


#endif
