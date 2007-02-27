// André Ribes EDF R&D - 2006
// 
#ifndef _CONNECTION_MANAGER_I_HXX_
#define _CONNECTION_MANAGER_I_HXX_

#include <iostream>
#include <map>
#include "DSC_Engines.hh"
#include <pthread.h>

/*! \class ConnectionManager_i
 *  \brief This class implements the interface Engines::ConnectionManager.
 */
class ConnectionManager_i :
  public virtual POA_Engines::ConnectionManager
{
  public :
    ConnectionManager_i(CORBA::ORB_ptr orb);
    ~ConnectionManager_i();

    /*!
     * \see Engines::ConnectionManager::connect
     */
    Engines::ConnectionManager::connectionId connect(Engines::DSC_ptr uses_component, 
						     const char* uses_port_name, 
						     Engines::DSC_ptr provides_component, 
						     const char* provides_port_name)
      throw (Engines::DSC::PortNotDefined,
	     Engines::DSC::BadPortType,
	     Engines::DSC::NilPort);

    /*!
     * \see Engines::ConnectionManager::disconnect
     */
    void disconnect(const Engines::ConnectionManager::connectionId id,
		    Engines::DSC::Message message)
      throw (Engines::ConnectionManager::BadId);

  private :

    struct connection_infos {
      Engines::DSC_ptr uses_component; 
      std::string uses_port_name;
      Engines::DSC_ptr provides_component;
      std::string provides_port_name;
      Ports::Port_ptr provides_port;
    };

    typedef std::map<Engines::ConnectionManager::connectionId, 
	    connection_infos *> ids_type;
    typedef std::map<Engines::ConnectionManager::connectionId, 
	    connection_infos *>::iterator ids_it_type;

    ids_type ids;
    ids_it_type ids_it;

    int current_id;
    pthread_mutex_t mutex;
};

#endif
