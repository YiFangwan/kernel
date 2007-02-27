// André Ribes EDF R&D - 2006
// 
#include "ConnectionManager_i.hxx"
#include "SALOME_NamingService.hxx"

ConnectionManager_i::ConnectionManager_i(CORBA::ORB_ptr orb) {
  SALOME_NamingService * ns = new SALOME_NamingService(orb);
  const char * ConnectionManagerNameInNS = "/ConnectionManager";
  ns->Register(_this(), ConnectionManagerNameInNS);

  current_id = 0;
  pthread_mutex_init(&mutex, NULL);
}

ConnectionManager_i::~ConnectionManager_i() {}

/* Cette méthode connecte deux ports ensembles.
 * Elle permet de rendre plus simple l'utilisation du modèle de programmation
 * à base de ports.
 */
Engines::ConnectionManager::connectionId
ConnectionManager_i::connect(Engines::DSC_ptr uses_component, 
			     const char* uses_port_name, 
			     Engines::DSC_ptr provides_component, 
			     const char* provides_port_name) 
throw (Engines::DSC::PortNotDefined,
       Engines::DSC::BadPortType,
       Engines::DSC::NilPort) 
{

  Ports::Port_ptr p_port = provides_component->get_provides_port(provides_port_name, false);
  uses_component->connect_uses_port(uses_port_name, p_port);
  provides_component->connect_provides_port(provides_port_name);

  // Ajout de l'id dans la map comme tout s'est bien passé
  // 
  // Protection du current_id pour récupérer un id unique
  pthread_mutex_lock(&mutex);
  Engines::ConnectionManager::connectionId rtn_id = current_id;
  current_id += 1;
  pthread_mutex_unlock(&mutex);

  // Creation de la structure et ajout dans la map
  connection_infos * infos = new connection_infos();
  infos->uses_component = Engines::DSC::_duplicate(uses_component);
  infos->uses_port_name = uses_port_name;
  infos->provides_component = Engines::DSC::_duplicate(provides_component);
  infos->provides_port_name = provides_port_name;
  infos->provides_port = Ports::Port::_duplicate(p_port);

  // Ajout dans la map
  ids[rtn_id] = infos;

  return rtn_id;
}

void
ConnectionManager_i::disconnect(const Engines::ConnectionManager::connectionId id,
				Engines::DSC::Message message)
throw (Engines::ConnectionManager::BadId)
{
  // Verification que l'id existe
  ids_it = ids.find(id);
  if (ids_it == ids.end())
    throw Engines::ConnectionManager::BadId();

  // Recuperation des infos
  connection_infos * infos = ids[id];
  infos->provides_component->disconnect_provides_port(infos->provides_port_name.c_str(),
						      message);
  infos->uses_component->disconnect_uses_port(infos->uses_port_name.c_str(),
					      Ports::Port::_duplicate(infos->provides_port),
					      message);
  // Nettoyage
  delete infos;
  ids.erase(id);
}
