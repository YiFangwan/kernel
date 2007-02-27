// André Ribes EDF R&D - 2006
// 

#include "DSC_i.hxx"

Engines_DSC_i::
Engines_DSC_i(CORBA::ORB_ptr orb,
			PortableServer::POA_ptr poa,
			PortableServer::ObjectId * contId,
			const char *instanceName,
			const char *interfaceName,
			bool notif) : Engines_Component_i(orb, 
							  poa, 
							  contId, 
							  instanceName, 
							  interfaceName) {}

Engines_DSC_i::~Engines_DSC_i() {}

