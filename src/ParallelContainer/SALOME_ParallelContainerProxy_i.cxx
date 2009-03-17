//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//  SALOME_ParallelContainerProxy : implementation of container and engine for Parallel Kernel
//  File   : SALOME_ParallelContainerProxy_i.cxx
//  Author : André RIBES, EDF

#include "SALOME_ParallelContainerProxy_i.hxx"

Container_proxy_impl_final::Container_proxy_impl_final(CORBA::ORB_ptr orb, 
						       paco_fabrique_thread * fab_thread, 
						       PortableServer::POA_ptr poa,
						       std::string containerName,
						       bool is_a_return_proxy) :
  Engines::PACO_Container_proxy_impl(orb, fab_thread, is_a_return_proxy),
  Engines::Container_proxy_impl(orb, fab_thread, is_a_return_proxy),
  InterfaceManager_impl(orb, fab_thread, is_a_return_proxy)
{
  _numInstance = 0;
  _containerName = containerName;
  _poa = PortableServer::POA::_duplicate(poa);

  // Add CORBA object to the poa
  _id = _poa->activate_object(this);
  this->_remove_ref();

  // Init SALOME Naming Service
  _NS = new SALOME_NamingService();
  _NS->init_orb(_orb);
}

Container_proxy_impl_final:: ~Container_proxy_impl_final() {
  if (_id)
    delete _id;
  if (_NS)
    delete _NS;
}

void
Container_proxy_impl_final::Shutdown()
{
  // We call shutdown in each node
  for (CORBA::ULong i = 0; i < _infos.nodes.length(); i++)
  {
    MESSAGE("Shutdown work node : " << i);
    CORBA::Object_var object = _orb->string_to_object(_infos.nodes[i]);
    Engines::Container_var node = Engines::Container::_narrow(object);
    if (!CORBA::is_nil(node))
    {
      try 
      {
	node->Shutdown();
	MESSAGE("Shutdown done node : " << i);
      }
      catch (...)
      {
	INFOS("Exception catch during Shutdown of node : " << i);
      }
    }
    else
    {
      INFOS("Cannot shutdown node " << i << " ref is nil !");
    }
  }

  INFOS("Shutdown Parallel Proxy");
  if(!CORBA::is_nil(_orb))
    _orb->shutdown(0);
}

// On intercepte cette méthode pour pouvoir ensuite
// déterminer si on doit créer une instance sequentielle
// ou parallèle d'un composant dans la méthode create_component_instance
CORBA::Boolean 
Container_proxy_impl_final::load_component_Library(const char* componentName)
{
  MESSAGE("Begin of load_component_Library on proxy : " << componentName)
  std::string aCompName = componentName;

  if (_libtype_map.count(aCompName) == 0)
  {
    _numInstanceMutex.lock(); // lock to be alone

    // Default lib is seq
    _libtype_map[aCompName] = "seq";

    // --- try dlopen C++ component
    // If is not a C++ or failed then is maybe 
    // a seq component...

    MESSAGE("Try to load C++ component");
#ifndef WIN32
    std::string impl_name = string ("lib") + aCompName + string("Engine.so");
#else
    std::string impl_name = aCompName + string("Engine.dll");
#endif
    void* handle;
#ifndef WIN32
    handle = dlopen( impl_name.c_str() , RTLD_LAZY ) ;
#else
    handle = dlopen( impl_name.c_str() , 0 ) ;
#endif
    if ( handle )
    {
      _library_map[impl_name] = handle;
      MESSAGE("Library " << impl_name << " loaded");

      //Test if lib could contain a parallel component

      std::string paco_test_fct_signature = aCompName + std::string("_isAPACO_Component");
      INFOS("SIG is : " << paco_test_fct_signature);
      PACO_TEST_FUNCTION paco_test_fct = NULL;
#ifndef WIN32
      paco_test_fct = (PACO_TEST_FUNCTION)dlsym(handle, paco_test_fct_signature.c_str());
#else
      paco_test_fct = (PACO_TEST_FUNCTION)GetProcAddress((HINSTANCE)handle, paco_test_fct_signature.c_str());
#endif
      if (paco_test_fct)
      {
	// PaCO Component found
	MESSAGE("PACO LIB FOUND");
	_libtype_map[aCompName] = "par";
      }
      else
      {
	MESSAGE("SEQ LIB FOUND");
#ifndef WIN32
	MESSAGE("dlerror() result is : " << dlerror());
#endif
      }
    }
    _numInstanceMutex.unlock();
  }

  // Call load_component_Library in each node
  CORBA::Boolean ret = true;
  for (CORBA::ULong i = 0; i < _infos.nodes.length(); i++)
  {
    MESSAGE("Call load_component_Library work node : " << i);
    CORBA::Object_var object = _orb->string_to_object(_infos.nodes[i]);
    Engines::Container_var node = Engines::Container::_narrow(object);
    if (!CORBA::is_nil(node))
    {
      try 
      {
	node->load_component_Library(componentName);
	MESSAGE("Call load_component_Library done node : " << i);
      }
      catch (...)
      {
	INFOS("Exception catch during load_component_Library of node : " << i);
	ret = false;
      }
    }
    else
    {
      INFOS("Cannot call load_component_Library node " << i << " ref is nil !");
      ret = false;
    }
  }

  // If ret is false -> lib is not loaded !
  if (!ret)
    _libtype_map.erase(aCompName);
  return ret;
}

// Il y a deux cas :
// Composant sequentiel -> on le créer sur le noeud 0 (on pourrait faire une répartition de charge)
// Composant parallèle -> création du proxy ici puis appel de la création de chaque objet participant
// au composant parallèle
Engines::Component_ptr 
Container_proxy_impl_final::create_component_instance(const char* componentName, ::CORBA::Long studyId)
{
  std::string aCompName = componentName;
  if (_libtype_map.count(aCompName) == 0)
  {
    // Component is not loaded !
    INFOS("Proxy: component is not loaded ! : " << aCompName);
    return Engines::Component::_nil();
  }

  // If it is a sequential component
  if (_libtype_map[aCompName] == "seq")
    return Engines::Container_proxy_impl::create_component_instance(componentName, studyId);

  // Parallel Component !
  Engines::Component_var component_proxy = Engines::Component::_nil();

  // On commence par créer le proxy
#ifndef WIN32
  std::string impl_name = string ("lib") + aCompName + string("Engine.so");
#else
  std::string impl_name = aCompName + string("Engine.dll");
#endif
  void* handle = _library_map[impl_name];
  std::string factory_name = aCompName + std::string("EngineProxy_factory");

  MESSAGE("Creating component proxy : " << factory_name);
  FACTORY_FUNCTION component_proxy_factory = (FACTORY_FUNCTION) dlsym(handle, factory_name.c_str());

  if (!component_proxy_factory)
  {
    INFOS("Can't resolve symbol: " + factory_name);
#ifndef WIN32
    INFOS("dlerror() result is : " << dlerror());
#endif
    return Engines::Component::_nil() ;
  }
  try {
    _numInstanceMutex.lock() ; // lock on the instance number
    _numInstance++ ;
    int numInstance = _numInstance ;
    _numInstanceMutex.unlock() ;

    char aNumI[12];
    sprintf( aNumI , "%d" , numInstance ) ;
    string instanceName = aCompName + "_inst_" + aNumI ;
    string component_registerName = _containerName + "/" + instanceName;

    // --- Instanciate required CORBA object
    PortableServer::ObjectId *id ; //not owner, do not delete (nore use var)
    id = (component_proxy_factory) (_orb, 
				    new paco_omni_fabrique(), 
				    _poa, 
				    _id, 
				    instanceName.c_str(), 
				    _parallel_object_topology.total) ;

    // --- get reference & servant from id
    CORBA::Object_var obj = _poa->id_to_reference(*id);
    component_proxy = Engines::Component::_narrow(obj) ;

    if (!CORBA::is_nil(component_proxy))
    {
      _cntInstances_map[impl_name] += 1;

      // --- register the engine under the name
      //     containerName(.dir)/instanceName(.object)
      _NS->Register(component_proxy , component_registerName.c_str()) ;
      MESSAGE(component_registerName.c_str() << " bound" ) ;
    }
    else
    {
      INFOS("The factory returns a nil object !");
      return Engines::Component::_nil();
    }
      
  }
  catch (...)
  {
    INFOS( "Exception catched in Proxy creation" );
    return Engines::Component::_nil();
  }

  // Create on each node a work node
  for (CORBA::ULong i = 0; i < _infos.nodes.length(); i++)
  {
    MESSAGE("Call create_paco_component_node_instance on work node : " << i);
    CORBA::Object_var object = _orb->string_to_object(_infos.nodes[i]);
    Engines::PACO_Container_var node = Engines::PACO_Container::_narrow(object);
    if (!CORBA::is_nil(node))
    {
      try 
      {
	node->create_paco_component_node_instance(componentName, studyId);
	MESSAGE("Call create_paco_component_node_instance done on node : " << i);
      }
      catch (SALOME::SALOME_Exception & ex)
      {
	INFOS("SALOME_EXCEPTION : " << ex.details.text);
	return Engines::Component::_nil();
      }
      catch (...)
      {
	INFOS("Unknown Exception catch during create_paco_component_node_instance on node : " << i);
	return Engines::Component::_nil();
      }
    }
    else
    {
      INFOS("Cannot call create_paco_component_node_instance on node " << i << " ref is nil !");
      return Engines::Component::_nil();
    }
  }

  // Start Parallel object
  PaCO::InterfaceManager_var paco_proxy = PaCO::InterfaceManager::_narrow(component_proxy);
  paco_proxy->start();

  return component_proxy;
}
