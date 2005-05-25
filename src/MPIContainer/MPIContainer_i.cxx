//  SALOME MPIContainer : implemenation of container based on MPI libraries
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
//  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 
//
//
//
//  File   : MPIContainer_i.cxx
//  Module : SALOME

#include <iostream.h>
#include <dlfcn.h>
#include <stdio.h>
#include "MPIContainer_i.hxx"
#include "SALOME_NamingService.hxx"
#include "Utils_SINGLETON.hxx"
#include "OpUtil.hxx"
#include "utilities.h"
using namespace std;

// L'appel au registry SALOME ne se fait que pour le process 0
Engines_MPIContainer_i::Engines_MPIContainer_i(int nbproc, int numproc,
					       CORBA::ORB_ptr orb, 
					       PortableServer::POA_ptr poa,
					       char * containerName,
					       int argc, char *argv[]) 
  : Engines_Container_i(orb,poa,containerName,argc,argv,false), MPIObject_i(nbproc,numproc)
{
  MESSAGE("[" << numproc << "] activate object");
  _id = _poa->activate_object(this);
//   this->_add_ref();

  if(numproc==0){

    _NS = new SALOME_NamingService();
//     _NS = SINGLETON_<SALOME_NamingService>::Instance() ;
//     ASSERT(SINGLETON_<SALOME_NamingService>::IsAlreadyExisting()) ;
    _NS->init_orb( CORBA::ORB::_duplicate(_orb) ) ;

//     Engines::Container_ptr pCont 
//       = Engines::Container::_narrow(POA_Engines::MPIContainer::_this());
    CORBA::Object_var obj=_poa->id_to_reference(*_id);
    Engines::Container_var pCont = Engines::Container::_narrow(obj);
    string hostname = GetHostname();
    _containerName = _NS->BuildContainerNameForNS(containerName,hostname.c_str());
    SCRUTE(_containerName);
    _NS->Register(pCont, _containerName.c_str());
  }

  // Root recupere les ior des container des autre process
  Engines::MPIObject_var pobj = POA_Engines::MPIContainer::_this();
  BCastIOR(_orb,pobj,true);
}

Engines_MPIContainer_i::Engines_MPIContainer_i(int nbproc, int numproc) 
  : Engines_Container_i(), MPIObject_i(nbproc,numproc)
{
}

Engines_MPIContainer_i::~Engines_MPIContainer_i(void)
{
  MESSAGE("[" << _numproc << "] Engines_MPIContainer_i::~Engines_MPIContainer_i()");
  if( !handle_map.empty() ){
    MESSAGE("[" << _numproc << "] Engines_MPIContainer_i::~Engines_MPIContainer_i: warning destroy a not empty container");
  }
}

// Load component
Engines::Component_ptr Engines_MPIContainer_i::load_impl(const char* nameToRegister,
						 const char* componentName)
{
  int ip;

  if( _numproc == 0 ){
    // Invocation du chargement du composant dans les autres process
    for(ip= 1;ip<_nbproc;ip++)
      (Engines::MPIContainer::_narrow((*_tior)[ip]))->Asload_impl(nameToRegister,
								componentName);
  }

  return Lload_impl(nameToRegister,componentName);

}

// Load component
void Engines_MPIContainer_i::Asload_impl(const char* nameToRegister,
					 const char* componentName)
{
  Lload_impl(nameToRegister,componentName);
}

Engines::Component_ptr Engines_MPIContainer_i::Lload_impl(
				   const char* nameToRegister,
				   const char* componentName)
{
  Engines::Component_var iobject;
  Engines::MPIObject_var pobj;
  char cproc[4];

  sprintf(cproc,"_%d",_numproc);

  BEGIN_OF("[" << _numproc << "] MPIContainer_i::Lload_impl");

  _numInstanceMutex.lock() ; // lock on the instance number
  _numInstance++ ;
  char _aNumI[12];
  sprintf(_aNumI,"%d",_numInstance) ;

  string _impl_name = componentName;
  string _nameToRegister = nameToRegister;
  string instanceName = _nameToRegister + "_inst_" + _aNumI + cproc;
  MESSAGE("[" << _numproc << "] instanceName=" << instanceName);

  string absolute_impl_name(_impl_name);
  MESSAGE("[" << _numproc << "] absolute_impl_name=" << absolute_impl_name);
  void * handle = dlopen(absolute_impl_name.c_str(), RTLD_LAZY);
  if(!handle){
    INFOS("[" << _numproc << "] Can't load shared library : " << absolute_impl_name);
    INFOS("[" << _numproc << "] error dlopen: " << dlerror());
    return Engines::Component::_nil() ;
  }

  string factory_name = _nameToRegister + string("Engine_factory");
  MESSAGE("[" << _numproc << "] factory_name=" << factory_name) ;

  PortableServer::ObjectId * (*MPIComponent_factory) (int,int,
						  CORBA::ORB_ptr,
						  PortableServer::POA_ptr,
						  PortableServer::ObjectId *,
						  const char *,
						  const char *) =
    (PortableServer::ObjectId * (*) (int,int,
				     CORBA::ORB_ptr,
				     PortableServer::POA_ptr, 
				     PortableServer::ObjectId *, 
				     const char *, 
				     const char *)) 
    dlsym(handle, factory_name.c_str());

  char *error ;
  if ((error = dlerror()) != NULL){
    // Try to load a sequential component
    MESSAGE("[" << _numproc << "] Try to load a sequential component");
    _numInstanceMutex.unlock() ;
    iobject = Engines_Container_i::load_impl(nameToRegister,componentName);
    if( CORBA::is_nil(iobject) ) return Engines::Component::_duplicate(iobject);
  }
  else{
    // Instanciation du composant parallele
    MESSAGE("[" << _numproc << "] Try to load a parallel component");
    PortableServer::ObjectId * id = (MPIComponent_factory)
      (_nbproc,_numproc,_orb, _poa, _id, instanceName.c_str(), _nameToRegister.c_str());
    // get reference from id
    CORBA::Object_var o = _poa->id_to_reference(*id);
    pobj = Engines::MPIObject::_narrow(o) ;
    iobject = Engines::Component::_narrow(o) ;
  }

  if( _numproc == 0 ){
    // utiliser + tard le registry ici :
    // register the engine under the name containerName.dir/nameToRegister.object
    string component_registerName = _containerName + "/" + _nameToRegister;
    _NS->Register(iobject, component_registerName.c_str()) ;
  }

  handle_map[instanceName] = handle;
  _numInstanceMutex.unlock() ;

  // Root recupere les ior des composants des autre process
  BCastIOR(_orb,pobj,false);

  END_OF("[" <<_numproc << "] MPIContainer_i::Lload_impl");
  return Engines::Component::_duplicate(iobject);

}

void Engines_MPIContainer_i::remove_impl(Engines::Component_ptr component_i)
{
  int ip;
  Engines::Component_ptr cptr;
  Engines::MPIObject_ptr pcptr;
  Engines::MPIObject_ptr spcptr;

  ASSERT(! CORBA::is_nil(component_i));

  if( _numproc == 0 ){
    // Invocation de la destruction du composant dans les autres process
    pcptr = (Engines::MPIObject_ptr)component_i;
    for(ip= 1;ip<_nbproc;ip++){
      spcptr = Engines::MPIObject::_narrow((*(pcptr->tior()))[ip]);
      cptr = (Engines::Component_ptr)spcptr;
      (Engines::MPIContainer::_narrow((*_tior)[ip]))->Asremove_impl(cptr);
    }
  }

  Lremove_impl(component_i);
}

void Engines_MPIContainer_i::Asremove_impl(Engines::Component_ptr component_i)
{
  Lremove_impl(component_i);
}

void Engines_MPIContainer_i::Lremove_impl(Engines::Component_ptr component_i)
{
  BEGIN_OF("[" << _numproc << "] MPIContainer_i::Lremove_impl");

  ASSERT(! CORBA::is_nil(component_i));

  string instanceName = component_i->instanceName() ;
  MESSAGE("[" << _numproc << "] unload component " << instanceName);
  component_i->destroy() ;
  MESSAGE("[" << _numproc << "] test key handle_map");
  _numInstanceMutex.lock() ; // lock on the remove on handle_map
  if (handle_map[instanceName]) // if key does not exist, created & initialized null
    {
      remove_map[instanceName] = handle_map[instanceName] ;
    }
  else MESSAGE("[" << _numproc << "] no key handle_map");
  handle_map.erase(instanceName) ;   
  _numInstanceMutex.unlock() ;
  MESSAGE("[" << _numproc << "] list handle_map");
  map<string, void *>::iterator im ;
  for (im = handle_map.begin() ; im != handle_map.end() ; im ++)
    {
      MESSAGE("[" << _numproc << "] stay " << (*im).first);
    }

  END_OF("[" << _numproc << "] MPIContainer_i::Lremove_impl");

}

void Engines_MPIContainer_i::finalize_removal()
{
  int ip;

  if( _numproc == 0 ){
    // Invocation de la destruction du composant dans les autres process
    for(ip= 1;ip<_nbproc;ip++)
      (Engines::MPIContainer::_narrow((*_tior)[ip]))->Asfinalize_removal();
  }

  Lfinalize_removal();
}

void Engines_MPIContainer_i::Asfinalize_removal()
{
  Lfinalize_removal();
}

void Engines_MPIContainer_i::Lfinalize_removal()
{
  BEGIN_OF("[" << _numproc << "] MPIContainer_i::Lfinalize_removal");

  map<string, void *>::iterator im ;
  // lock on the explore remove_map & dlclose
  _numInstanceMutex.lock() ; 
  for (im = remove_map.begin() ; im != remove_map.end() ; im ++)
    {
      void * handle = (*im).second ;
      MESSAGE("[" << _numproc << "] dlclose " << (*im).first);
      dlclose(handle) ;
    }
  MESSAGE("[" << _numproc << "] remove_map.clear()");
  remove_map.clear() ;  
  _numInstanceMutex.unlock() ;

  END_OF("[" << _numproc << "] MPIContainer_i::Lfinalize_removal");
}

// Load component
void Engines_MPIContainer_i::Shutdown()
{
  int ip;
  MESSAGE("[" << _numproc << "] shutdown of MPI Corba Server");
  if( _numproc == 0 ){
    _NS->Destroy_FullDirectory(_containerName.c_str());
    for(ip= 1;ip<_nbproc;ip++)
      (Engines::MPIContainer::_narrow((*_tior)[ip]))->Shutdown();
  }
  _orb->shutdown(0);

}

