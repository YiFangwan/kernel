//  SALOME_ParallelComponent : implementation of container and engine for Parallel Kernel
//
//  Copyright (C) 2007 OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
//  File   : SALOME_ParallelComponent_i.cxx
//  Author : Andr� RIBES, EDF
//  Author : Paul RASCLE, EDF - MARC TAJCHMAN, CEA

#include "SALOME_ParallelComponent_i.hxx"
#include "SALOME_ParallelContainer_i.hxx"

#include "OpUtil.hxx"
#include <stdio.h>
#ifndef WNT
#include <dlfcn.h>
#endif
#include <cstdlib>
#include "utilities.h"

#ifndef WNT
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#else
#include <sys/timeb.h>
int SIGUSR11 = 1000;
#endif


using namespace std;

extern bool _Sleeping ;
static Engines_Parallel_Component_i * theEngines_Component ;

bool Engines_Parallel_Component_i::_isMultiStudy = true;
bool Engines_Parallel_Component_i::_isMultiInstance = false;

//=============================================================================
/*! 
 *  Default constructor, not for use
 */
//=============================================================================

Engines_Parallel_Component_i::Engines_Parallel_Component_i(CORBA::ORB_ptr orb, char * ior) : 
  InterfaceParallel_impl(orb,ior), Engines::Component_serv(orb,ior), Engines::Parallel_Component_serv(orb,ior)
{
  //ASSERT(0);
  INFOS("Default Constructor...");
}

//=============================================================================
/*! 
 *  Standard Constructor for generic Component, used in derived class
 *  Connection to Registry and Notification
 *  \param orb Object Request broker given by Container
 *  \parap poa Portable Object Adapter from Container (normally root_poa)
 *  \param contId container CORBA id inside the server
 *  \param instanceName unique instance name for this object (see Container_i)
 *  \param interfaceName component class name
 *  \param notif use of notification
 */
//=============================================================================

Engines_Parallel_Component_i::Engines_Parallel_Component_i(CORBA::ORB_ptr orb, char * ior,
					 PortableServer::POA_ptr poa, 
					 PortableServer::ObjectId * contId, 
					 const char *instanceName,
					 const char *interfaceName,
                                         bool notif) :
  InterfaceParallel_impl(orb,ior), 
  Engines::Component_serv(orb,ior),
  Engines::Parallel_Component_serv(orb,ior),
  _instanceName(instanceName),
  _interfaceName(interfaceName),
  _myConnexionToRegistry(0),
  _ThreadId(0) ,
  _ThreadCpuUsed(0) ,
  _Executed(false) ,
  _graphName("") ,
  _nodeName(""),
 _studyId(-1)
{
  MESSAGE("Component constructor with instanceName "<< _instanceName);
  //SCRUTE(pd_refCount);
  _orb = CORBA::ORB::_duplicate(orb);
  _poa = PortableServer::POA::_duplicate(poa);
  _contId = contId ;
  CORBA::Object_var o = _poa->id_to_reference(*contId); // container ior...
  const CORBA::String_var the_ior = _orb->object_to_string(o);
  _myConnexionToRegistry = new RegistryConnexion(0, 0, the_ior,"theSession",
						 _instanceName.c_str());

  _notifSupplier = new NOTIFICATION_Supplier(instanceName, notif);

  deploy_mutex = new pthread_mutex_t();
  pthread_mutex_init(deploy_mutex, NULL);
  _proxy = NULL;
 //SCRUTE(pd_refCount);
}

//=============================================================================
/*! 
 *  Destructor: call Container for decrement of instances count.
 *  When instances count falls to 0, the container tries to remove the
 *  component library (dlclose)
 */
//=============================================================================

Engines_Parallel_Component_i::~Engines_Parallel_Component_i()
{
  MESSAGE("Component destructor");
  Engines_Parallel_Container_i::decInstanceCnt(_interfaceName);
  pthread_mutex_destroy(deploy_mutex);
  delete deploy_mutex;

  if (_proxy)
    delete _proxy;
}

//=============================================================================
/*! 
 *  CORBA method: return name of the instance, unique in this Container
 */
//=============================================================================

char* Engines_Parallel_Component_i::instanceName()
{
   return CORBA::string_dup(_instanceName.c_str()) ;
}

//=============================================================================
/*! 
 *  CORBA method: return name of the component class
 */
//=============================================================================

char* Engines_Parallel_Component_i::interfaceName()
{
  return CORBA::string_dup(_interfaceName.c_str()) ;
}

//=============================================================================
/*! 
 *  CORBA method: Get study Id
 *  \return -1: not initialised (Internal Error)
 *           0: multistudy component instance
 *          >0: study id associated to this instance
 */
//=============================================================================

CORBA::Long Engines_Parallel_Component_i::getStudyId()
{
  return _studyId;
}

//=============================================================================
/*! 
 *  CORBA method: Test if instance is alive and responds
 */
//=============================================================================

void Engines_Parallel_Component_i::ping()
{
  //  MESSAGE("Engines_Parallel_Component_i::ping_c() pid "<< getpid() << " threadid " << pthread_self());
}

//=============================================================================
/*! 
 *  CORBA method: Deactivate this instance. CORBA object is deactivated (do not
 *  respond any more to CORBA calls), the connection to Regsitry is removed
 *  (Registry informed of deactivation), internal server reference counter on
 *  the derived servant class is decremented, to allow destruction of the class
 *  (delete) by POA, when there are no more references.
 *  -- TO BE USED BY CONTAINER ONLY (Container housekeeping) --
 */
//=============================================================================

void Engines_Parallel_Component_i::destroy()
{
  MESSAGE("Engines_Parallel_Component_i::destroy()");
  //SCRUTE(pd_refCount);

  delete _notifSupplier;
  _notifSupplier = 0;

  delete _myConnexionToRegistry;
  _myConnexionToRegistry = 0 ;
  _poa->deactivate_object(*_id) ;
  CORBA::release(_poa) ;
  delete(_id) ;
  //SCRUTE(pd_refCount);
  _thisObj->_remove_ref();
  //SCRUTE(pd_refCount);
  MESSAGE("Engines_Parallel_Component_i::destroyed") ;
}

//=============================================================================
/*! 
 *  CORBA method: return CORBA reference of the Container
 *
 */
//=============================================================================

Engines::Container_ptr Engines_Parallel_Component_i::GetContainerRef()
{
  MESSAGE("Engines_Parallel_Component_i::GetContainerRef");
  CORBA::Object_ptr o = _poa->id_to_reference(*_contId) ;
  return Engines::Container::_narrow(o);
}

//=============================================================================
/*! 
 *  CORBA method: 
 *  Gives a sequence of (key=string,value=any) to the component. 
 *  Base class component stores the sequence in a map.
 *  The map is cleared before.
 *  This map is for use by derived classes. 
 *  \param dico sequence of (key=string,value=any)
 */
//=============================================================================

void Engines_Parallel_Component_i::setProperties(const Engines::FieldsDict& dico)
{
  _fieldsDict.clear();
  for (CORBA::ULong i=0; i<dico.length(); i++)
    {
      std::string cle(dico[i].key);
      _fieldsDict[cle] = dico[i].value;
    }
}

//=============================================================================
/*! 
 *  CORBA method: 
 *  returns a previously stored map (key=string,value=any) as a sequence.
 *  (see setProperties)
 */
//=============================================================================

Engines::FieldsDict* Engines_Parallel_Component_i::getProperties()
{
  Engines::FieldsDict_var copie = new Engines::FieldsDict;
  copie->length(_fieldsDict.size());
  map<std::string,CORBA::Any>::iterator it;
  CORBA::ULong i = 0;
  for (it = _fieldsDict.begin(); it != _fieldsDict.end(); it++, i++)
    {
      std::string cle((*it).first);
      copie[i].key = CORBA::string_dup(cle.c_str());
      copie[i].value = _fieldsDict[cle];
    }
  return copie._retn();
}

//=============================================================================
/*! 
 *  CORBA method: used by Supervision to give names to this instance
 */
//=============================================================================

void Engines_Parallel_Component_i::Names( const char * graphName ,
                                 const char * nodeName )
{
  _graphName = graphName ;
  _nodeName = nodeName ;
  INFOS("Engines_Parallel_Component_i::Names( '" << _graphName << "' , '"
	<< _nodeName << "' )");
}

//=============================================================================
/*! 
 *  CORBA method: used in Supervision
 */
//=============================================================================

bool Engines_Parallel_Component_i::Kill_impl() 
{
//  MESSAGE("Engines_Parallel_Component_i::Kill_i() pthread_t "<< pthread_self()
//          << " pid " << getpid() << " instanceName "
//          << _instanceName.c_str() << " interface " << _interfaceName.c_str()
//          << " machineName " << GetHostname().c_str()<< " _id " << hex << _id
//          << dec << " _ThreadId " << _ThreadId << " this " << hex << this
//          << dec ) ;

  bool RetVal = false ;
#ifndef WNT
  if ( _ThreadId > 0 && pthread_self() != _ThreadId )
    {
      RetVal = Killer( _ThreadId , 0 ) ;
      _ThreadId = (pthread_t ) -1 ;
    }

#else
  if ( _ThreadId > 0 && pthread_self().p != _ThreadId->p )
    {
      RetVal = Killer( *_ThreadId , 0 ) ;
      _ThreadId = (pthread_t* ) 0 ;
    }

#endif
  return RetVal ;
}

//=============================================================================
/*! 
 *  CORBA method: used in Supervision
 */
//=============================================================================

bool Engines_Parallel_Component_i::Stop_impl()
{
  MESSAGE("Engines_Parallel_Component_i::Stop_i() pthread_t "<< pthread_self()
          << " pid " << getpid() << " instanceName "
          << _instanceName.c_str() << " interface " << _interfaceName.c_str()
          << " machineName " << GetHostname().c_str()<< " _id " << hex << _id
          << dec << " _ThreadId " << _ThreadId );
  

  bool RetVal = false ;
#ifndef WNT
  if ( _ThreadId > 0 && pthread_self() != _ThreadId )
    {
      RetVal = Killer( _ThreadId , 0 ) ;
      _ThreadId = (pthread_t ) -1 ;
    }
#else
  if ( _ThreadId > 0 && pthread_self().p != _ThreadId->p )
    {
      RetVal = Killer( *_ThreadId , 0 ) ;
      _ThreadId = (pthread_t* ) 0 ;
    }
#endif
  return RetVal ;
}

//=============================================================================
/*! 
 *  CORBA method: used in Supervision
 */
//=============================================================================

bool Engines_Parallel_Component_i::Suspend_impl()
{
  MESSAGE("Engines_Parallel_Component_i::Suspend_i() pthread_t "<< pthread_self()
          << " pid " << getpid() << " instanceName "
          << _instanceName.c_str() << " interface " << _interfaceName.c_str()
          << " machineName " << GetHostname().c_str()<< " _id " << hex << _id
          << dec << " _ThreadId " << _ThreadId );

  bool RetVal = false ;
#ifndef WNT
  if ( _ThreadId > 0 && pthread_self() != _ThreadId )
#else
  if ( _ThreadId > 0 && pthread_self().p != _ThreadId->p )
#endif
    {
      if ( _Sleeping )
	{
	  return false ;
	}
    else 
      {
#ifndef WNT
	RetVal = Killer( _ThreadId ,SIGINT ) ;
#else
	RetVal = Killer( *_ThreadId ,SIGINT ) ;
#endif
	//if ( RetVal ) _Sleeping = true;

      }
    }
  return RetVal ;
}

//=============================================================================
/*! 
 *  CORBA method: used in Supervision
 */
//=============================================================================

bool Engines_Parallel_Component_i::Resume_impl()
{
  MESSAGE("Engines_Parallel_Component_i::Resume_i() pthread_t "<< pthread_self()
          << " pid " << getpid() << " instanceName "
          << _instanceName.c_str() << " interface " << _interfaceName.c_str()
          << " machineName " << GetHostname().c_str()<< " _id " << hex << _id
          << dec << " _ThreadId " << _ThreadId );
  bool RetVal = false ;
#ifndef WNT
  if ( _ThreadId > 0 && pthread_self() != _ThreadId )
#else
  if ( _ThreadId > 0 && pthread_self().p != _ThreadId->p )
#endif
    {
    if ( _Sleeping ) 
      {
	_Sleeping = false ;
	RetVal = true ;
      }
    else
      {
	RetVal = false ;
      }
    }
  return RetVal ;
}

//=============================================================================
/*! 
 *  CORBA method: 
 */
//=============================================================================

CORBA::Long Engines_Parallel_Component_i::CpuUsed_impl()
{
  long cpu = 0 ;
  if ( _ThreadId || _Executed )
    {
    if ( _ThreadId > 0 )
      {
#ifndef WNT
      if ( pthread_self() != _ThreadId )
#else
      if ( pthread_self().p != _ThreadId->p )
#endif
	{
        if ( _Sleeping )
	  {
	  }
        else
	  {
	    // Get Cpu in the appropriate thread with that object !...
	    theEngines_Component = this ;
#ifndef WNT
	    Killer( _ThreadId ,SIGUSR1 ) ;
#else
	    Killer( *_ThreadId ,SIGUSR11 ) ;
#endif
	  }
        cpu = _ThreadCpuUsed ;
	}
      else
	{
	  _ThreadCpuUsed = CpuUsed() ;
	  cpu = _ThreadCpuUsed ;
	  // cout << pthread_self() << " Engines_Parallel_Component_i::CpuUsed_impl "
	  //      << _serviceName << " " << cpu << endl ;
      }
    }
    else 
      {
	cpu = _ThreadCpuUsed ;
	// cout << pthread_self() << " Engines_Parallel_Component_i::CpuUsed_impl "
	//      << _serviceName << " " << cpu<< endl ;
      }
    }
  else
    {
      // cout<< pthread_self()<<"Engines_Parallel_Component_i::CpuUsed_impl _ThreadId "
      //     <<_ThreadId <<" "<<_serviceName<<" _StartUsed "<<_StartUsed<<endl;
    }
  return cpu ;
}


//=============================================================================
/*! 
 *  C++ method: return Container Servant
 */
//=============================================================================

Engines_Parallel_Container_i *Engines_Parallel_Component_i::GetContainerPtr()
{
  return dynamic_cast<Engines_Parallel_Container_i*>(_poa->id_to_servant(*_contId)) ;
}

//=============================================================================
/*! 
 *  C++ method: set study Id
 *  \param studyId         0 if instance is not associated to a study, 
 *                         >0 otherwise (== study id)
 *  \return true if the set of study Id is OK
 *  must be set once by Container, at instance creation,
 *  and cannot be changed after.
 */
//=============================================================================

CORBA::Boolean Engines_Parallel_Component_i::setStudyId(CORBA::Long studyId)
{
  ASSERT( studyId >= 0);
  CORBA::Boolean ret = false;
  if (_studyId < 0) // --- not yet initialized 
    {
      _studyId = studyId;
      ret = true;
    }
  else
    if ( _studyId == studyId) ret = true;
  return ret;
}

//=============================================================================
/*! 
 *  C++ method: return CORBA instance id, the id is set in derived class
 *  constructor, when instance is activated.
 */
//=============================================================================

PortableServer::ObjectId * Engines_Parallel_Component_i::getId()
{
//  MESSAGE("PortableServer::ObjectId * Engines_Parallel_Component_i::getId()");
  return _id ;
}

//=============================================================================
/*! 
 *  C++ method: used by derived classes for supervision
 */
//=============================================================================

void Engines_Parallel_Component_i::beginService(const char *serviceName)
{
  MESSAGE(pthread_self() << "Send BeginService notification for " <<serviceName
	  << endl << "Component instance : " << _instanceName << endl << endl);
#ifndef WNT
  _ThreadId = pthread_self() ;
#else
  _ThreadId = new pthread_t;
  _ThreadId->p = pthread_self().p ;
  _ThreadId->x = pthread_self().x ;
#endif
  _StartUsed = 0 ;
  _StartUsed = CpuUsed_impl() ;
  _ThreadCpuUsed = 0 ;
  _Executed = true ;
  _serviceName = serviceName ;
  if ( pthread_setcanceltype( PTHREAD_CANCEL_ASYNCHRONOUS , NULL ) )
    {
      perror("pthread_setcanceltype ") ;
      exit(0) ;
    }
  if ( pthread_setcancelstate( PTHREAD_CANCEL_ENABLE , NULL ) )
    {
      perror("pthread_setcancelstate ") ;
      exit(0) ;
    }
//  MESSAGE(pthread_self() << " Return from BeginService for " << serviceName
//          << " ThreadId " << _ThreadId << " StartUsed " << _StartUsed
//          << " _graphName " << _graphName << " _nodeName " << _nodeName );

  // --- for supervisor : all strings given with setProperties
  //     are set in environment
  bool overwrite = true;
  map<std::string,CORBA::Any>::iterator it;
  for (it = _fieldsDict.begin(); it != _fieldsDict.end(); it++)
    {
      std::string cle((*it).first);
      if ((*it).second.type()->kind() == CORBA::tk_string)
	{
	  const char* value;
	  (*it).second >>= value;
	  // ---todo: replace __GNUC__ test by an autoconf macro AC_CHECK_FUNC.
#if defined __GNUC__
	  //int ret = setenv(cle.c_str(), value, overwrite);
	  setenv(cle.c_str(), value, overwrite);
#else
	  //CCRT porting : setenv not defined in stdlib.h
	  std::string s(cle);
	  s+='=';
	  s+=value;
	  // char* cast because 1st arg of linux putenv function
	  // is not a const char* !
	  //int ret=putenv((char *)s.c_str());
	  putenv((char *)s.c_str());
	  //End of CCRT porting
#endif
	  MESSAGE("--- setenv: "<<cle<<" = "<< value);
	}
    }
}

//=============================================================================
/*! 
 *  C++ method: used by derived classes for supervision
 */
//=============================================================================

void Engines_Parallel_Component_i::endService(const char *serviceName)
{
  _ThreadCpuUsed = CpuUsed_impl() ;
  MESSAGE(pthread_self() << " Send EndService notification for " << serviceName
	  << endl << " Component instance : " << _instanceName << " StartUsed "
          << _StartUsed << " _ThreadCpuUsed "<< _ThreadCpuUsed << endl <<endl);
  _ThreadId = 0 ;
}

//=============================================================================
/*! 
 *  C++ method: -- CHECK IF USED --
 */
//=============================================================================

char* Engines_Parallel_Component_i::graphName()
{
  return CORBA::string_dup( _graphName.c_str() ) ;
}

//=============================================================================
/*! 
 *  C++ method: -- CHECK IF USED --
 */
//=============================================================================

char* Engines_Parallel_Component_i::nodeName()
{
  return CORBA::string_dup( _nodeName.c_str() ) ;
}

//=============================================================================
/*! 
 *  C++ method: used in Supervision (see kill_impl)
 */
//=============================================================================

bool Engines_Parallel_Component_i::Killer( pthread_t ThreadId , int signum )
{
#ifndef WNT
  if ( ThreadId )
#else
  if ( ThreadId.p )
#endif
    {
      if ( signum == 0 )
	{
	  if ( pthread_cancel( ThreadId ) )
	    {
	      perror("Killer pthread_cancel error") ;
	      return false ;
	    }
	  else
	    {
	      MESSAGE(pthread_self() << "Killer : ThreadId " << ThreadId
		      << " pthread_canceled") ;
	    }
	}
      else
	{
	  if ( pthread_kill( ThreadId , signum ) == -1 )
	    {
	      perror("Killer pthread_kill error") ;
	      return false ;
	    }
	  else 
	    {
	      MESSAGE(pthread_self() << "Killer : ThreadId " << ThreadId
		      << " pthread_killed(" << signum << ")") ;
	    }
	}
    }
  return true ;
}

//=============================================================================
/*! 
 *  C++ method:
 */ 
//=============================================================================

void SetCpuUsed()
{
  theEngines_Component->SetCurCpu() ;
}

//=============================================================================
/*! 
 *  C++ method:
 */
//=============================================================================

void Engines_Parallel_Component_i::SetCurCpu()
{
  _ThreadCpuUsed =  CpuUsed() ;
  //  MESSAGE(pthread_self() << 
  //  " Engines_Parallel_Component_i::SetCurCpu() _ThreadCpuUsed " << _ThreadCpuUsed) ;
}

//=============================================================================
/*! 
 *  C++ method:
 */
//=============================================================================

long Engines_Parallel_Component_i::CpuUsed()
{
  long cpu = 0 ;
#ifndef WNT
  struct rusage usage ;
  if ( _ThreadId || _Executed )
    {
      if ( getrusage( RUSAGE_SELF , &usage ) == -1 )
	{
	  perror("Engines_Parallel_Component_i::CpuUsed") ;
	  return 0 ;
	}
      cpu = usage.ru_utime.tv_sec - _StartUsed ;
      // cout << pthread_self() << " Engines_Parallel_Component_i::CpuUsed " << " "
      //      << _serviceName   << usage.ru_utime.tv_sec << " - " << _StartUsed
      //      << " = " << cpu << endl ;
    }
  else
    {
      // cout << pthread_self() << "Engines_Parallel_Component_i::CpuUsed _ThreadId "
      //      << _ThreadId << " " << _serviceName<< " _StartUsed " 
      //      << _StartUsed << endl ;
    }
#else
	// NOT implementet yet
#endif


  return cpu ;
}

//=============================================================================
/*! 
 *  C++ method: Send message to event channel
 */
//=============================================================================

void Engines_Parallel_Component_i::sendMessage(const char *event_type,
				      const char *message)
{
    _notifSupplier->Send(graphName(), nodeName(), event_type, message);
}

//=============================================================================
/*! 
 *  C++ method: return standard library name built on component name
 */
//=============================================================================

string Engines_Parallel_Component_i::GetDynLibraryName(const char *componentName)
{
  string ret="lib";
  ret+=componentName;
  ret+="Engine.so";
  return ret;
}

//=============================================================================
/*! 
 *  C++ method: DumpPython default implementation
 */
//=============================================================================

Engines::TMPFile* Engines_Parallel_Component_i::DumpPython(CORBA::Object_ptr theStudy, 
							   CORBA::Boolean isPublished, 
							   CORBA::Boolean& isValidScript)
{
  const char* aScript = "def RebuildData(theStudy): pass";
  char* aBuffer = new char[strlen(aScript)+1];
  strcpy(aBuffer, aScript);
  CORBA::Octet* anOctetBuf =  (CORBA::Octet*)aBuffer;
  int aBufferSize = strlen(aBuffer)+1;
  Engines::TMPFile_var aStreamFile = new Engines::TMPFile(aBufferSize, aBufferSize, anOctetBuf, 1); 
  isValidScript = true;
  return aStreamFile._retn(); 
}


Engines::Salome_file_ptr 
Engines_Parallel_Component_i::setInputFileToService(const char* service_name, 
						    const char* Salome_file_name) 
{
  // Try to find the service, if it doesn't exist, we add it.
  _Service_file_map_it = _Input_Service_file_map.find(service_name);
  if (_Service_file_map_it ==  _Input_Service_file_map.end()) {
    _t_Salome_file_map * _map = new _t_Salome_file_map();
    _Input_Service_file_map[service_name] = _map;
    _t_Proxy_Salome_file_map * _proxy_map = new _t_Proxy_Salome_file_map();
    _Proxy_Input_Service_file_map[service_name] = _proxy_map;
  }
  _t_Salome_file_map * _map = _Input_Service_file_map[service_name];
  _t_Proxy_Salome_file_map * _proxy_map = _Proxy_Input_Service_file_map[service_name];
  
  std::string proxy_ior;

  // Try to find the Salome_file ...
  _Salome_file_map_it = _map->find(Salome_file_name);
  if (_Salome_file_map_it ==  _map->end()) {

    // We create a new PaCO++ object.
    // He has the same configuration than
    // his component

    pthread_mutex_lock(deploy_mutex);
    // Firstly, we have to create the proxy object
    // of the Salome_file and transmit his
    // reference to the other nodes.
    if (getMyRank() == 0) {
      Engines::Parallel_Salome_file_proxy_impl * proxy = 
	new Engines::Parallel_Salome_file_proxy_impl(CORBA::ORB::_duplicate(_orb));
      PaCO_operation * proxy_global_ptr =  proxy->getContext("global_paco_context");
      proxy_global_ptr->setTypeClient(true);
      PaCO::PacoTopology_t client_topo;
      client_topo.total = 1;
      proxy_global_ptr->setClientTopo(client_topo);
      PaCO::PacoTopology_t serveur_topo;
      serveur_topo.total = getTotalNode();
      proxy->setTopo(serveur_topo);

      // We initialize the object with the context of the Parallel component
      PaCO_operation * compo_global_ptr =  getContext("global_paco_context");
      //compo_global_ptr->init_context(proxy_global_ptr);
      proxy_global_ptr->init_context(compo_global_ptr);
      // We register the CORBA objet into the POA
      CORBA::Object_ptr proxy_ref = proxy->_this();

      // We send the reference to all the nodes...
      CORBA::Object_ptr comp_proxy = _orb->string_to_object(_ior.c_str());
      Engines::Parallel_Component_var component_proxy = Engines::Parallel_Component::_narrow(comp_proxy);
      component_proxy->send_parallel_proxy_object(proxy_ref);

      // Adding proxy into the map
      (*_proxy_map)[Salome_file_name] = proxy;
    }
    else {
      this->wait_parallel_object_proxy();
    }

    proxy_ior = this->get_parallel_proxy_object();

    // We register each node of the parallel Salome_file object
    // into the proxy.
    for (int i = 0; i < getTotalNode(); i++) {
      if (i ==  getMyRank()) {
	Parallel_Salome_file_i * servant = 
	  new Parallel_Salome_file_i(CORBA::ORB::_duplicate(_orb), proxy_ior.c_str());
	PaCO_operation * servant_global_ptr = servant->getContext("global_paco_context");
	
	// We initialize the object with the context of the Parallel component
	PaCO_operation * compo_global_ptr =  this->getContext("global_paco_context");
//	compo_global_ptr->init_context(servant_global_ptr);
	servant_global_ptr->init_context(compo_global_ptr);
	
	// We register the CORBA objet into the POA
	servant->POA_PaCO::InterfaceParallel::_this();

	// Register the servant
	servant->deploy(getMyRank());

	// Adding servant to the map
	(*_map)[Salome_file_name] = servant;
      }

      PaCO_operation * compo_global_ptr =  this->getContext("global_paco_context");
      compo_global_ptr->my_com->paco_barrier();
    }

    // Parallel_Salome_file is created and deployed
    delete _proxy;
    _proxy = NULL;
    pthread_mutex_unlock(deploy_mutex);
  }
  else {
    // Salome_file_name already added into the service
    // throw Exception
    SALOME::ExceptionStruct es;
    es.type = SALOME::INTERNAL_ERROR;
    es.text = "Salome_file_name already added";
    throw SALOME::SALOME_Exception(es);
  }

  CORBA::Object_ptr obj = _orb->string_to_object(proxy_ior.c_str());
  return Engines::Salome_file::_narrow(obj);
}

Engines::Salome_file_ptr 
Engines_Parallel_Component_i::setOutputFileToService(const char* service_name, 
						     const char* Salome_file_name) 
{
  // Try to find the service, if it doesn't exist, we add it.
  _Service_file_map_it = _Output_Service_file_map.find(service_name);
  if (_Service_file_map_it ==  _Output_Service_file_map.end()) {
    _t_Salome_file_map * _map = new _t_Salome_file_map();
    _Output_Service_file_map[service_name] = _map;
    _t_Proxy_Salome_file_map * _proxy_map = new _t_Proxy_Salome_file_map();
    _Proxy_Output_Service_file_map[service_name] = _proxy_map;
  }
  _t_Salome_file_map * _map = _Output_Service_file_map[service_name];
  _t_Proxy_Salome_file_map * _proxy_map = _Proxy_Output_Service_file_map[service_name];
  
  std::string proxy_ior;

  // Try to find the Salome_file ...
  _Salome_file_map_it = _map->find(Salome_file_name);
  if (_Salome_file_map_it ==  _map->end()) {

    // We create a new PaCO++ object.
    // He has the same configuration than
    // his component

    pthread_mutex_lock(deploy_mutex);
    // Firstly, we have to create the proxy object
    // of the Salome_file and transmit his
    // reference to the other nodes.
    if (getMyRank() == 0) {
      Engines::Parallel_Salome_file_proxy_impl * proxy = 
	new Engines::Parallel_Salome_file_proxy_impl(CORBA::ORB::_duplicate(_orb));
      PaCO_operation * proxy_global_ptr =  proxy->getContext("global_paco_context");
      proxy_global_ptr->setTypeClient(true);
      PaCO::PacoTopology_t client_topo;
      client_topo.total = 1;
      proxy_global_ptr->setClientTopo(client_topo);
      PaCO::PacoTopology_t serveur_topo;
      serveur_topo.total = getTotalNode();
      proxy->setTopo(serveur_topo);

      // We initialize the object with the context of the Parallel component
      PaCO_operation * compo_global_ptr =  getContext("global_paco_context");
      //compo_global_ptr->init_context(proxy_global_ptr);
      proxy_global_ptr->init_context(compo_global_ptr);
      // We register the CORBA objet into the POA
      CORBA::Object_ptr proxy_ref = proxy->_this();

      // We send the reference to all the nodes...
      CORBA::Object_ptr comp_proxy = _orb->string_to_object(_ior.c_str());
      Engines::Parallel_Component_var component_proxy = Engines::Parallel_Component::_narrow(comp_proxy);
      component_proxy->send_parallel_proxy_object(proxy_ref);

      // Adding proxy into the map
      (*_proxy_map)[Salome_file_name] = proxy;
    }
    else {
      this->wait_parallel_object_proxy();
    }

    proxy_ior = this->get_parallel_proxy_object();

    // We register each node of the parallel Salome_file object
    // into the proxy.
    for (int i = 0; i < getTotalNode(); i++) {
      if (i ==  getMyRank()) {
	Parallel_Salome_file_i * servant = 
	  new Parallel_Salome_file_i(CORBA::ORB::_duplicate(_orb), proxy_ior.c_str());
	PaCO_operation * servant_global_ptr = servant->getContext("global_paco_context");
	
	// We initialize the object with the context of the Parallel component
	PaCO_operation * compo_global_ptr =  this->getContext("global_paco_context");
//	compo_global_ptr->init_context(servant_global_ptr);
	servant_global_ptr->init_context(compo_global_ptr);
	
	// We register the CORBA objet into the POA
	servant->POA_PaCO::InterfaceParallel::_this();

	// Register the servant
	servant->deploy(getMyRank());

	// Adding servant to the map
	(*_map)[Salome_file_name] = servant;
      }

      PaCO_operation * compo_global_ptr =  this->getContext("global_paco_context");
      compo_global_ptr->my_com->paco_barrier();
    }

    // Parallel_Salome_file is created and deployed
    delete _proxy;
    _proxy = NULL;
    pthread_mutex_unlock(deploy_mutex);
  }
  else {
    // Salome_file_name already added into the service
    // throw Exception
    SALOME::ExceptionStruct es;
    es.type = SALOME::INTERNAL_ERROR;
    es.text = "Salome_file_name already added";
    throw SALOME::SALOME_Exception(es);
  }

  CORBA::Object_ptr obj = _orb->string_to_object(proxy_ior.c_str());
  return Engines::Salome_file::_narrow(obj);
}

Engines::Salome_file_ptr 
Engines_Parallel_Component_i::getInputServiceSalome_file(const char* service_name, 
						const char* Salome_file_name) 
{
  // Try to find the service, if it doesn't exist, we throw an exception.
  _Proxy_Service_file_map_it = _Proxy_Input_Service_file_map.find(service_name);
  if (_Proxy_Service_file_map_it ==  _Proxy_Input_Service_file_map.end()) {
    SALOME::ExceptionStruct es;
    es.type = SALOME::INTERNAL_ERROR;
    es.text = "service doesn't have salome files";
    throw SALOME::SALOME_Exception(es);
  }
  _t_Proxy_Salome_file_map * _map = _Proxy_Input_Service_file_map[service_name];

  // Try to find the Salome_file ...
  _Proxy_Salome_file_map_it = _map->find(Salome_file_name);
  if (_Proxy_Salome_file_map_it ==  _map->end()) {
    SALOME::ExceptionStruct es;
    es.type = SALOME::INTERNAL_ERROR;
    es.text = "service doesn't have this Salome_file";
    throw SALOME::SALOME_Exception(es);
  }

  // Client get the proxy object
  Engines::Parallel_Salome_file_proxy_impl * Sfile = (*_map)[Salome_file_name];
  return Sfile->_this();
}

Engines::Salome_file_ptr 
Engines_Parallel_Component_i::getOutputServiceSalome_file(const char* service_name, 
						const char* Salome_file_name) 
{
  // Try to find the service, if it doesn't exist, we throw an exception.
  _Proxy_Service_file_map_it = _Proxy_Output_Service_file_map.find(service_name);
  if (_Proxy_Service_file_map_it ==  _Proxy_Output_Service_file_map.end()) {
    SALOME::ExceptionStruct es;
    es.type = SALOME::INTERNAL_ERROR;
    es.text = "service doesn't have salome files";
    throw SALOME::SALOME_Exception(es);
  }
  _t_Proxy_Salome_file_map * _map = _Proxy_Output_Service_file_map[service_name];

  // Try to find the Salome_file ...
  _Proxy_Salome_file_map_it = _map->find(Salome_file_name);
  if (_Proxy_Salome_file_map_it ==  _map->end()) {
    SALOME::ExceptionStruct es;
    es.type = SALOME::INTERNAL_ERROR;
    es.text = "service doesn't have this Salome_file";
    throw SALOME::SALOME_Exception(es);
  }

  // Client get the proxy object
  Engines::Parallel_Salome_file_proxy_impl * Sfile = (*_map)[Salome_file_name];
  return Sfile->_this();
}


void 
Engines_Parallel_Component_i::checkInputServiceFiles(const char* service_name) 
{
  // Try to find the service, if it doesn't exist, nothing to do.
  _Proxy_Service_file_map_it = _Proxy_Input_Service_file_map.find(service_name);
  if (_Proxy_Service_file_map_it !=  _Proxy_Input_Service_file_map.end()) {
    _t_Proxy_Salome_file_map * _proxy_map = _Proxy_Input_Service_file_map[service_name];
    _t_Proxy_Salome_file_map::iterator begin = _proxy_map->begin();
    _t_Proxy_Salome_file_map::iterator end = _proxy_map->end();

    for(;begin!=end;begin++) {
      Engines::Parallel_Salome_file_proxy_impl * file = begin->second;
      file->recvFiles();
    }
  }
}

void 
Engines_Parallel_Component_i::checkOutputServiceFiles(const char* service_name) 
{
  // Try to find the service, if it doesn't exist, nothing to do.
  _Proxy_Service_file_map_it = _Proxy_Output_Service_file_map.find(service_name);
  if (_Proxy_Service_file_map_it !=  _Proxy_Output_Service_file_map.end()) {
    _t_Proxy_Salome_file_map * _map = _Proxy_Output_Service_file_map[service_name];
    _t_Proxy_Salome_file_map::iterator begin = _map->begin();
    _t_Proxy_Salome_file_map::iterator end = _map->end();

    for(;begin!=end;begin++) {
      Engines::Parallel_Salome_file_proxy_impl * file = begin->second;
      file->recvFiles();
    }
  }

}

void 
Engines_Parallel_Component_i::send_parallel_proxy_object(CORBA::Object_ptr proxy_ref) {
  _proxy = _orb->object_to_string(proxy_ref);
}

void 
Engines_Parallel_Component_i::wait_parallel_object_proxy() {
  char * proxy = NULL;
  proxy =  get_parallel_proxy_object();
  while(proxy == NULL)
  {
    sleep(1);
    proxy = get_parallel_proxy_object();
  }
}

char * 
Engines_Parallel_Component_i::get_parallel_proxy_object() {
  return _proxy;
}
