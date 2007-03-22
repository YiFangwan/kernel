//  Copyright (C) 2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS 
// 
//  This library is free software; you can redistribute it and/or 
//  modify it under the terms of the GNU Lesser General Public 
//  License as published by the Free Software Foundation; either 
//  version 2.1 of the License. 
// 
//  This library is distributed in the hope that it will be useful, 
//  but WITHOUT ANY WARRANTY; without even the implied warranty of 
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
//  Lesser General Public License for more details. 
// 
//  You should have received a copy of the GNU Lesser General Public 
//  License along with this library; if not, write to the Free Software 
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA 
// 
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//
//
//  File   : Superv_Component_i.hxx
//  Author : Andr� RIBES (EDF), Eric Fayolle (EDF)
//  Module : KERNEL

#ifndef _SUPERV_COMPONENT_I_HXX_
#define _SUPERV_COMPONENT_I_HXX_

#include "DSC_i.hxx"
#include "base_port.hxx"
#include "uses_port.hxx"
#include "provides_port.hxx"

// Les diff�rentes fabriques de ports
#include "basic_port_factory.hxx"
#include "palm_port_factory.hxx"
#include "calcium_port_factory.hxx"

#include "DSC_Exception.hxx"


using namespace std;

/*! \class Superv_Component_i
 *  \brief This class implements DSC_User component.
 *
 *  This class allows a higher programming level than DSC_Basic. It enables
 *  a programming level for service's developpers who want to use DSC ports.
 *
 *  This class has two level for using and declare ports. The higher level proposes
 *  operations to add ports that are provided by default by Salom� like Calcium ports.
 *  It provides too some methods to add their own DSC_User ports.
 *
 *  \note This class doesn't implement the init_service CORBA operation.
 */
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

  // Exceptions declarations.
  // There are defined on the Superv_Component_i.cxx to avoid problems
  // from dlopen.
  DSC_EXCEPTION(BadFabType);
  DSC_EXCEPTION(BadType);
  DSC_EXCEPTION(BadCast);
  DSC_EXCEPTION(UnexpectedState);
  DSC_EXCEPTION(PortAlreadyDefined);
  DSC_EXCEPTION(PortNotDefined);
  DSC_EXCEPTION(PortNotConnected);
  DSC_EXCEPTION(NilPort);
  DSC_EXCEPTION(BadProperty);
  
  /*!
   * \warning currently disabled.
   */
  virtual provides_port * create_provides_control_port() 
  {return NULL;}

  /*!
   * \warning currently disabled.
   */
  virtual provides_port * create_provides_data_and_control_port(const char* port_type) 
  {return NULL;}

  /*!
   * \warning currently disabled.
   */
  virtual uses_port * create_uses_control_port()
  {return NULL;}

  /*!
   * \warning currently disabled.
   */
  virtual uses_port * create_uses_data_and_control_port(const char* port_type)
  {return NULL;}

  /*!
   * This methode permits to create a provides port provided by the platform.
   * (See documentation of DSC for knoing these ports).
   *   
   *
   * \param port_fab_type type provides port.
   * \return the provides port.
   *
   * \note It's user repsonsability to destroy the provides port.
   */
  virtual provides_port * create_provides_data_port(const char* port_fab_type)
    throw (BadFabType);


  /*!
   * This methode permits to create a uses port provided by the platform.
   * (See documentation of DSC for knoing these ports).
   *   
   *
   * \param port_fab_type type uses port.
   * \return the uses port.
   *
   * \note It's user repsonsability to destroy the uses port.
   */
  virtual uses_port * create_uses_data_port(const char* port_fab_type)
    throw (BadFabType); 

  /*!
   * Adds a port to the component. With this method only Salom�'s provided DSC ports
   * can be added.
   *
   * \param port_fab_type type of the port.
   * \param port_type uses or provides.
   * \param port_name the name of the port in the component.
   */
  virtual void add_port(const char * port_fab_type,
			const char * port_type,
			const char * port_name)
    throw (PortAlreadyDefined, BadFabType, BadType, BadProperty);

  /*!
   * Adds a port to the component. With this method only Salom�'s provided DSC ports
   * can be added.
   *
   * \param port_fab_type type of the port.
   * \param port_type uses or provides.
   * \param port_name the name of the port in the component.
   * \return the created port.   
   */
  template < typename SpecificPortType >  
  SpecificPortType * add_port(const char * port_fab_type,
			      const char * port_type,
			      const char * port_name)
    throw (PortAlreadyDefined, BadFabType, BadType, BadCast, BadProperty);

  /*!
   * Adds a created provides port to the component.
   *
   * \param port the provides port.
   * \param provides_port_name the name of the port in the component.
   */
  virtual void add_port(provides_port * port, 
			const char* provides_port_name)
    throw (PortAlreadyDefined, NilPort, BadProperty);

  /*!
   * Adds a created uses port to the component.
   *
   * \param port the uses port.
   * \param uses_port_name the name of the port in the component.
   */
  virtual void add_port(uses_port * port, 
			const char* uses_port_name)
    throw (PortAlreadyDefined, NilPort, BadProperty);

  /*!
   * Gets the provides port already added in the component.
   *
   * \param port the provides port pointer.
   * \param provides_port_name the name of the port.
   */
  virtual void get_port(provides_port *& port, 
			const char* provides_port_name)
    throw (PortNotDefined, PortNotConnected);
  
  /*!
   * Gets the uses port already added in the component.
   *
   * \param port the uses port pointer.
   * \param uses_port_name the name of the port.
   */
  virtual void get_port(uses_port *& port, 
			const char* uses_port_name)
    throw (PortNotDefined, PortNotConnected);

  /*!
   * Gets the list of the ports of a service.
   * If servicename is not set, all the ports of the component are 
   * returned.
   *
   * \param port_names the ports's list.
   * \param servicename service's name.
   */
  virtual void get_uses_port_names(std::vector<std::string> & port_names,
				   const std::string servicename="") const;

  /*!
   * Gets a port already added in the component.
   *
   * \param provides_port_name the name of the port.
   * \return a port's pointer.
   */
  template <typename SpecificPortType > 
  SpecificPortType * get_port( const char * port_name)
    throw (PortNotDefined, PortNotConnected, BadCast, UnexpectedState);
 
  /*!
   * \see DSC_Callbacks::provides_port_changed
   */
  virtual void provides_port_changed(const char* provides_port_name,
				     int connection_nbr,
				     const Engines::DSC::Message message);

  /*!
   * \see DSC_Callbacks::uses_port_changed
   */
  virtual void uses_port_changed(const char* uses_port_name,
				 Engines::DSC::uses_port * new_uses_port,
				 const Engines::DSC::Message message);


private:    

  // Fabrics
  basic_port_factory * _my_basic_factory;
  palm_port_factory * _my_palm_factory;
  calcium_port_factory *   _my_calcium_factory;

  /*-------------------------------------------------*/
  // A Superv_Component port.
  struct superv_port_t {
    superv_port_t():u_ref(NULL),p_ref(NULL){};
    // For uses ports.
    uses_port * u_ref;
    // For provides ports.
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
      throw BadCast( LOC("La conversion vers le type de port demand� n'est pas possible " ));
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
      throw BadCast( LOC("La conversion vers le type de port demand� n'est pas possible " ));
    }
  }
  else
    throw BadType(LOC(OSS()<< "Le port_type doit �tre soit 'provides' soit 'uses' not "
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
				<< " n'est pas connect�."));
  }
  
  retPort = dynamic_cast<SpecificPortType *>(port);
  if ( retPort == NULL ) {
    delete port; 
    throw BadCast( LOC("La conversion vers le type de port demand� n'est pas possible " ));
  }

  return retPort;
};


#endif
