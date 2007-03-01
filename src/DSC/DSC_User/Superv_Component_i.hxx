// André Ribes - EDF R&D 2006
//
// Cette classe implémente le moteur de la plate-forme Salomé
//
// Elle dispose de deux types méthodes :
// Des méthodes pour les ports provides
// Des méthodes pour les ports uses
//
// Les ports provides sont des ports fournis par la plate-forme réalisant
// par exemple le fonctionnement des ports Calcium ou des ports Basiques sur les 
// types CORBA
//
// Les ports uses sont des objets C++ faisant office de proxy pour le code du service afin 
// de réaliser des fonctionnalités avant le transfert des données. Ces fonctionnalités doivent d'ailleurs 
// être écrites par le développeur s'il utilise un port spécifique à son application.
//
//
//
// Pour ces besoins le moteur redéfini la méthode connect et fourni une méthode register
// pour les proxy
//
// On notera que la méthode init n'est pas implémenté. Elle est laissée à l'implémentation 
// du moteur et des services.
//
// Author      : ribes
//
// Modified by : $LastChangedBy: fayolle $
// Date        : $LastChangedDate: 2007-03-01 13:36:05 +0100 (jeu, 01 mar 2007) $
// Id          : $Id$

#ifndef _SUPERV_COMPONENT_I_HXX_
#define _SUPERV_COMPONENT_I_HXX_

#include "parallel_salome_config.h"
#include "DSC_i.hxx"
#include "base_port.hxx"
#include "uses_port.hxx"
#include "provides_port.hxx"

// Les différentes fabriques de ports
#include "basic_port_factory.hxx"
#include "palm_port_factory.hxx"
#include "calcium_port_factory.hxx"

#include "DSC_Exception.hxx"


using namespace std;

class Superv_Component_i :
  public Engines_DSC_i,
  virtual public POA_Engines::Superv_Component
{
public:
  Superv_Component_i(CORBA::ORB_ptr orb,
		     PortableServer::POA_ptr poa,
		     PortableServer::ObjectId * contId,
		     const char *instanceName,
		     const char *interfaceName,
		     bool notif = false);
  virtual ~Superv_Component_i();

  // Déclarations des Exceptions
  // Les définitions sont dans le .cxx
  DSC_EXCEPTION(BadFabType);
  DSC_EXCEPTION(BadType);
  DSC_EXCEPTION(BadCast);
  DSC_EXCEPTION(UnexpectedState);
  DSC_EXCEPTION(PortAlreadyDefined);
  DSC_EXCEPTION(PortNotDefined);
  DSC_EXCEPTION(PortNotConnected);
  DSC_EXCEPTION(NilPort);
  DSC_EXCEPTION(BadProperty);
  
  // C++ methods currently disabled
  virtual provides_port * create_provides_control_port() 
  {return NULL;}
  virtual provides_port * create_provides_data_and_control_port(const char* port_type) 
  {return NULL;}
  virtual uses_port * create_uses_control_port()
  {return NULL;}
  virtual uses_port * create_uses_data_and_control_port(const char* port_type)
  {return NULL;}

  // Fabriques
  // Note: Il manque les méthodes pour ajouter des fabriques
  // Dans ce cas là, les fabriques doivent contenir le constructeur
  // Bref à revoir éventuellement puisque'on peut faire 
  // autrement en surchargement ces méthodes dans le composant
  // utilisateur
  virtual provides_port * create_provides_data_port(const char* port_fab_type)
    throw (BadFabType);
  virtual uses_port * create_uses_data_port(const char* port_fab_type)
    throw (BadFabType); 

  // Ajouts/Récuperations
  virtual void add_port(const char * port_fab_type,
			const char * port_type,
			const char * port_name)
    throw (PortAlreadyDefined, BadFabType, BadType, BadProperty);

  template < typename SpecificPortType >  
  SpecificPortType * add_port(const char * port_fab_type,
			      const char * port_type,
			      const char * port_name)
    throw (PortAlreadyDefined, BadFabType, BadType, BadCast, BadProperty);

  virtual void add_port(provides_port * port, 
			const char* provides_port_name)
    throw (PortAlreadyDefined, NilPort, BadProperty);
  virtual void add_port(uses_port * port, 
			const char* uses_port_name)
    throw (PortAlreadyDefined, NilPort, BadProperty);

  virtual void get_port(provides_port *& port, 
			const char* provides_port_name)
    throw (PortNotDefined, PortNotConnected);
  
  virtual void get_port(uses_port *& port, 
			const char* uses_port_name)
    throw (PortNotDefined, PortNotConnected);

  virtual void get_uses_port_names(std::vector<std::string> & port_names,
				   const std::string servicename="") const;

  template <typename SpecificPortType > 
  SpecificPortType * get_port( const char * provides_port_name)
    throw (PortNotDefined, PortNotConnected, BadCast, UnexpectedState);
  
  // Connexions/Déconnexions
  // Actuellement vide, mais on rajoutera bientôt des 
  // méthodes afin de gérer la connexion dans un même container.

  // Callbacks
  virtual void provides_port_changed(const char* provides_port_name,
				     int connection_nbr,
				     const Engines::DSC::Message message);

  virtual void uses_port_changed(const char* uses_port_name,
				 Engines::DSC::uses_port * new_uses_port,
				 const Engines::DSC::Message message);


private:    

  // Les fabriques
  basic_port_factory * _my_basic_factory;
  palm_port_factory * _my_palm_factory;
  calcium_port_factory *   _my_calcium_factory;

  /*-------------------------------------------------*/
  // Gestion des ports
  struct superv_port_t {
    superv_port_t():u_ref(NULL),p_ref(NULL){};
    // Specifique aux uses port
    uses_port * u_ref;
    // Specifique aux provides port;
    provides_port * p_ref;
  };

  typedef std::map<std::string, superv_port_t *> superv_ports;

  /*-------------------------------------------------*/
  /*-------------------------------------------------*/

  superv_ports my_superv_ports;
  superv_ports::iterator my_superv_ports_it;
};



template < typename SpecificPortType >  SpecificPortType * 
Superv_Component_i::add_port(const char * port_fab_type,
			     const char * port_type,
			     const char * port_name)
  throw (PortAlreadyDefined, BadFabType, BadType, BadCast, BadProperty)
{
  assert(port_fab_type);
  assert(port_type);
  assert(port_name);
  SpecificPortType * retPort; 

  std::cout << "---- Superv_Component_i::add_port :  Mark 0 ----  " << port_name << "----" << std::endl;
    
  std::string s_port_type(port_type);
  if (s_port_type == "provides") {
    provides_port * port = create_provides_data_port(port_fab_type);
    add_port(port, port_name);
    retPort = dynamic_cast<SpecificPortType *>(port);
    if ( retPort == NULL ) { delete port;  
      throw BadCast( LOC("La conversion vers le type de port demandé n'est pas possible " ));
    }
  }
  else if (s_port_type == "uses") {
    uses_port * port = create_uses_data_port(port_fab_type);
    add_port(port, port_name);
    std::cout << "---- Superv_Component_i::add_port :  Mark 1 ----  " << port << "----" << std::endl;
    std::cout << "---- Superv_Component_i::add_port :  Mark 1 ----   get_repository_id()" << port->get_repository_id() << std::endl;
    retPort = dynamic_cast<SpecificPortType *>(port);
    std::cout << "---- Superv_Component_i::add_port :  Mark 2 ----  " << retPort << "----" << std::endl;
    if ( retPort == NULL ) { delete port;  
      throw BadCast( LOC("La conversion vers le type de port demandé n'est pas possible " ));
    }
  }
  else
    throw BadType(LOC(OSS()<< "Le port_type doit être soit 'provides' soit 'uses' not "
		      << port_type));
  
  return retPort;
};


template <typename SpecificPortType > SpecificPortType * 
Superv_Component_i::get_port( const char * port_name)
  throw (PortNotDefined, PortNotConnected, BadCast, UnexpectedState)
{
  assert(port_name);
    
  SpecificPortType * retPort;
  base_port        * port;

  superv_port_t * superv_port =  my_superv_ports[port_name];
  try {
    if ( superv_port->p_ref != NULL ) {
      port = superv_port->p_ref;
      Engines_DSC_interface::get_provides_port(port_name, false); 
    } 
    else if ( superv_port->u_ref != NULL ) {
      port = superv_port->u_ref;
      Engines_DSC_i::get_uses_port(port_name);
    } else {
      throw UnexpectedState( LOC(OSS()<< "Internal Error superv_port struct is inconsistent "));
    
    }
  } catch (const Engines::DSC::PortNotDefined&) {
    throw PortNotDefined( LOC(OSS()<< "Le port "
			      << port_name <<" n'existe pas."));
  } catch (const Engines::DSC::PortNotConnected&) {
    throw PortNotConnected( LOC(OSS()<< "Le port " << port_name 
				<< " n'est pas connecté."));
  }
  
  retPort = dynamic_cast<SpecificPortType *>(port);
  if ( retPort == NULL ) {
    delete port; 
    throw BadCast( LOC("La conversion vers le type de port demandé n'est pas possible " ));
  }

  return retPort;
};


#endif
