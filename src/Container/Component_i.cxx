//  SALOME Container : implementation of container and engine for Kernel
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
//  File   : Component_i.cxx
//  Author : Paul RASCLE, EDF - MARC TAJCHMAN, CEA
//  Module : SALOME
//  $Header$

#include "SALOME_Component_i.hxx"
#include "RegistryConnexion.hxx"
#include "OpUtil.hxx"
#include <stdio.h>
#include <dlfcn.h>
#include <cstdlib>
#include "utilities.h"
using namespace std;

extern bool _Sleeping ;
static Engines_Component_i * theEngines_Component ;

Engines_Component_i::Engines_Component_i() :
  _instanceName(""),
  _interfaceName(""),
  _myConnexionToRegistry(0),
  _ThreadId(0) , _ThreadCpuUsed(0) , _Executed(false) , _graphName("") , _nodeName("") {
  MESSAGE("Component constructor");
  _orb = 0 ;
  _poa = 0;
  _contId = 0 ;
  pthread_mutex_init( &_MutexThreadId , NULL ) ;
}

Engines_Component_i::Engines_Component_i(CORBA::ORB_ptr orb,
					 PortableServer::POA_ptr poa, 
					 PortableServer::ObjectId * contId, 
					 const char *instanceName,
					 const char *interfaceName,
                                         bool notif) :
  _instanceName(instanceName),
  _interfaceName(interfaceName),
  _myConnexionToRegistry(0),
  _ThreadId(0) , _ThreadCpuUsed(0) , _Executed(false) , _graphName("") , _nodeName("") {
  MESSAGE("Component constructor with instanceName "<< _instanceName);
  _orb = CORBA::ORB::_duplicate(orb);
  _poa = PortableServer::POA::_duplicate(poa);
  _contId = contId ;
  pthread_mutex_init( &_MutexThreadId , NULL ) ;
  CORBA::Object_var o = _poa->id_to_reference(*contId); // container ior...
  const CORBA::String_var ior = _orb->object_to_string(o);
  _myConnexionToRegistry = new RegistryConnexion(0, 0, ior, "theSession", _instanceName.c_str());

  _notifSupplier = new NOTIFICATION_Supplier(instanceName, notif);
}

// Constructeur pour composant parallele: ne pas faire appel au registry!!
Engines_Component_i::Engines_Component_i(CORBA::ORB_ptr orb,
					 PortableServer::POA_ptr poa, 
					 PortableServer::ObjectId * contId, 
					 const char *instanceName,
					 const char *interfaceName,
					 int flag,
                                         bool notif )
  : _instanceName(instanceName),
    _interfaceName(interfaceName),
    _myConnexionToRegistry(0),
    _ThreadId(0) , _ThreadCpuUsed(0) , _Executed(false) , _graphName("") , _nodeName("") {
  MESSAGE("Component constructor with instanceName without Registry"<< _instanceName);
  _orb = CORBA::ORB::_duplicate(orb);
  _poa = PortableServer::POA::_duplicate(poa);
  _contId = contId ;
  pthread_mutex_init( &_MutexThreadId , NULL ) ;
  //  CORBA::Object_var myself = this->_this(); //appel a _this = increment reference

  _notifSupplier = new NOTIFICATION_Supplier(instanceName, notif);
}

Engines_Component_i::~Engines_Component_i()
{
  MESSAGE("Component destructor");
//   delete _myConnexionToRegistry;
//   _myConnexionToRegistry = 0 ;
}

char* Engines_Component_i::instanceName() {
   return CORBA::string_dup(_instanceName.c_str()) ;
}

char* Engines_Component_i::interfaceName() {
   return CORBA::string_dup(_interfaceName.c_str()) ;
}

void Engines_Component_i::ping()
{
  MESSAGE("Engines_Component_i::ping() pid "<< getpid() << " threadid "
          << pthread_self());
}

void Engines_Component_i::destroy()
{
  MESSAGE("Engines_Component_i::destroy()");

  delete _notifSupplier;
  _notifSupplier = 0;

  delete _myConnexionToRegistry;
  _myConnexionToRegistry = 0 ;
  _poa->deactivate_object(*_id) ;
  CORBA::release(_poa) ;
  delete(_id) ;
  _thisObj->_remove_ref();
  MESSAGE("Engines_Component_i::destroyed") ;
}

Engines::Container_ptr Engines_Component_i::GetContainerRef()
{
  MESSAGE("Engines_Component_i::GetContainerRef");
  CORBA::Object_ptr o = _poa->id_to_reference(*_contId) ;
  return Engines::Container::_narrow(o);
}

PortableServer::ObjectId * Engines_Component_i::getId()
{
//  MESSAGE("PortableServer::ObjectId * Engines_Component_i::getId()");
  return _id ;
}

void Engines_Component_i::setProperties(const Engines::FieldsDict& dico)
{
  _fieldsDict.clear();
  for (CORBA::ULong i=0; i<dico.length(); i++)
    {
      std::string cle(dico[i].key);
      _fieldsDict[cle] = dico[i].value;
    }
}

Engines::FieldsDict* Engines_Component_i::getProperties()
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

void Engines_Component_i::beginService(const char *serviceName)
{
  MESSAGE(pthread_self() << "Send BeginService notification for " << serviceName << endl
	  << "Component instance : " << _instanceName << endl << endl);
  int sts ;
  if ( (sts = pthread_mutex_lock( &_MutexThreadId ) ) ) {
    perror("pthread_mutex_lock _MutexThreadId ") ;
    cout << "Component_i::beginService pthread_mutex_lock _MutexThreadId sts " << sts << endl ;
    exit( 0 ) ;
  }
// Datas for CpuUsed, Kill, Suspend and Resume
// It is not actually possible to have that functionnalities for more than one method
// of the same Component of the same Container at the same time because there is
// only once instance
  if ( _ThreadId == 0 ) {
    _ThreadId = pthread_self() ;
    _StartUsed = 0 ;
    _StartUsed = CpuUsed_impl() ;
    _ThreadCpuUsed = 0 ;
    _Executed = true ;
    _serviceName = serviceName ;
  }
  else {
    MESSAGE("beginService('" << serviceName << "' cannot have CpuUsed/Kill/Suspend/Resume functionnalities. "
            << _serviceName << " is already running !" ) ;
  }
  if ( (sts = pthread_mutex_unlock( &_MutexThreadId ) ) ) {
    perror("pthread_mutex_lock _MutexThreadId ") ;
    cout << "Component_i::beginService pthread_mutex_unlock _MutexThreadId sts " << sts << endl ;
    exit( 0 ) ;
  }

  if ( pthread_setcanceltype( PTHREAD_CANCEL_ASYNCHRONOUS , NULL ) ) {
    perror("pthread_setcanceltype ") ;
    exit(0) ;
  }
  if ( pthread_setcancelstate( PTHREAD_CANCEL_ENABLE , NULL ) ) {
    perror("pthread_setcancelstate ") ;
    exit(0) ;
  }
//  MESSAGE(pthread_self() << " Return from BeginService for " << serviceName
//          << " ThreadId " << _ThreadId << " StartUsed " << _StartUsed
//          << " _graphName " << _graphName << " _nodeName " << _nodeName );

  // --- for supervisor : all strings given with setProperties
  //     are set in environment
//  bool overwrite = true;
  map<std::string,CORBA::Any>::iterator it;
  for (it = _fieldsDict.begin(); it != _fieldsDict.end(); it++)
    {
      std::string cle((*it).first);
      if ((*it).second.type()->kind() == CORBA::tk_string)
	{
	  const char* value;
	  (*it).second >>= value;
	  //CCRT porting : setenv not defined in stdlib.h
	  std::string s(cle);
	  s+='=';
	  s+=value;
	  //char* cast because 1st arg of linux putenv function is not a const char* !!!
	  putenv((char *)s.c_str());
	  //End of CCRT porting
	  //int ret = setenv(cle.c_str(), value, overwrite);
	  MESSAGE("--- setenv: "<<cle<<" = "<< value);
	}
    }
}

void Engines_Component_i::endService(const char *serviceName)
{
  if ( pthread_self() == _ThreadId ) {
    _ThreadCpuUsed = CpuUsed_impl() ;
    MESSAGE(pthread_self() << " Send EndService notification for " << serviceName << endl
	    << " Component instance : " << _instanceName << " StartUsed " << _StartUsed << " _ThreadCpuUsed "
            << _ThreadCpuUsed << endl << endl ) ;
    _ThreadId = 0 ;
  }
  else {
    MESSAGE(pthread_self() << " Send EndService notification for " << serviceName
            << " # active _ThreadId " << _ThreadId ) ;
  }
}

void Engines_Component_i::Names( const char * graphName ,
                                 const char * nodeName ) {
  _graphName = graphName ;
  _nodeName = nodeName ;
//  MESSAGE("Engines_Component_i::Names( '" << _graphName << "' , '"
//          << _nodeName << "' )");
}

char* Engines_Component_i::graphName() {
  return CORBA::string_dup( _graphName.c_str() ) ;
}

char* Engines_Component_i::nodeName() {
  return CORBA::string_dup( _nodeName.c_str() ) ;
}

bool Engines_Component_i::Killer( pthread_t ThreadId , int signum ) {
  string signame ;
  if ( signum == 0 ) {
    signame = string("pthread_cancel") ;// 0
  }
  else if ( signum == SIGINT ) {
    signame = string("SIGINT Kill") ;// 2
  }
  else if ( signum == SIGCONT ) {
    signame = string("SIGCONT Resume") ;// 18
  }
  else if ( signum == SIGUSR1 ) {
    signame = string("SIGUSR1 CpuUsed") ;// 10
  }
  else if ( signum == SIGUSR2 ) {
    signame = string("SIGUSR2 Suspend") ;// 12
  }
  cout << pthread_self() << "Killer : ThreadId " << ThreadId << " pthread_cancel/kill( "
       << signame << " ) :" << endl ;
  if ( ThreadId > 0 ) {
    if ( signum == 0 ) {
      int sts = pthread_cancel( ThreadId ) ;
      if ( sts != 0 && sts != ESRCH ) {
        perror("perror Killer pthread_cancel error") ;
        cout << pthread_self() << "Killer : ThreadId " << ThreadId << " sts " << sts
             << " errno " << errno << endl ;
        return false ;
      }
      else {
        cout << pthread_self() << "Killer : ThreadId " << ThreadId << " pthread_canceled" << endl ;
      }
    }
    else {
      if ( pthread_kill( ThreadId , signum ) == -1 ) {
        perror("Killer pthread_kill error") ;
        return false ;
      }
      else {
        MESSAGE(pthread_self() << "Killer : ThreadId " << ThreadId << " pthread_killed("
                << signame << ")") ;
      }
    }
  }
  else {
    return false ;
  }
  return true ;
}

bool Engines_Component_i::Kill_impl() {
  cout << "Begin Engines_Component_i::Kill_impl() pthread_t "<< pthread_self()
       << " _ThreadId " << _ThreadId << " errno " << errno << endl ;
  bool RetVal = false ;
  if ( _ThreadId > 0 ) {
    if ( pthread_self() != _ThreadId ) { // Do not kill myself ...
      RetVal = Killer( _ThreadId , 0 ) ;
    }
    _ThreadId = 0 ;
  }
  cout << "End Engines_Component_i::Kill_impl() pthread_t "<< pthread_self()
       << " _ThreadId " << _ThreadId << " errno " << errno << endl ;
  return RetVal ;
}

bool Engines_Component_i::Stop_impl() {
  MESSAGE( "Engines_Component_i::Stop_impl() pthread_t "<< pthread_self()
           << " _ThreadId " << _ThreadId << endl ) ;
  bool RetVal = false ;
  if ( _ThreadId > 0 ) {
    if ( pthread_self() != _ThreadId ) { // Do not kill myself ...
      RetVal = Killer( _ThreadId , 0 ) ;
    }
    _ThreadId = 0 ;
  }
  return RetVal ;
}

bool Engines_Component_i::Suspend_impl() {
  MESSAGE( "Engines_Component_i::Suspend_impl() pthread_t "<< pthread_self()
           << " _ThreadId " << _ThreadId << endl ) ;
  bool RetVal = false ;
  if ( _ThreadId > 0 && pthread_self() != _ThreadId ) {
    if ( _Sleeping ) {
      return false ;
    }
    else {
      RetVal = Killer( _ThreadId ,SIGINT ) ;
    }
  }
  return RetVal ;
}

bool Engines_Component_i::Resume_impl() {
  MESSAGE( "Engines_Component_i::Resume_impl() pthread_t "<< pthread_self()
           << " _ThreadId " << _ThreadId << endl ) ;
  bool RetVal = false ;
  if ( _ThreadId > 0 && pthread_self() != _ThreadId ) {
    if ( _Sleeping ) {
      _Sleeping = false ;
      RetVal = true ;
    }
    else {
      RetVal = false ;
    }
  }
  return RetVal ;

}

bool SetCpuUsed() {
  if ( theEngines_Component ) {
    theEngines_Component->SetCurCpu() ;
    return true ;
  }
  else {
    INFOS(pthread_self() << "Engines_Component_i(SetCpuUsed) theEngines_Component==NULL") ;
  }
  return false ;
}
void Engines_Component_i::SetCurCpu() {
  _ThreadCpuUsed =  CpuUsed() ;
//  MESSAGE(pthread_self() << " Engines_Component_i::SetCurCpu() _ThreadCpuUsed " << _ThreadCpuUsed) ;
}

#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>

long Engines_Component_i::CpuUsed() {
  long cpu = 0 ;
  struct rusage usage ;
  if ( _ThreadId || _Executed ) {
    if ( getrusage( RUSAGE_SELF , &usage ) == -1 ) {
      perror("Engines_Component_i::CpuUsed") ;
      return 0 ;
    }
    cpu = usage.ru_utime.tv_sec - _StartUsed ;
//    cout << pthread_self() << " Engines_Component_i::CpuUsed " << " " << _serviceName
//         << usage.ru_utime.tv_sec << " - " << _StartUsed << " = " << cpu << endl ;
  }
  else {
//    cout << pthread_self() << "Engines_Component_i::CpuUsed _ThreadId " << _ThreadId << " " << _serviceName
//         << " _StartUsed " << _StartUsed << endl ;
  }
  return cpu ;
}

CORBA::Long Engines_Component_i::CpuUsed_impl() {
  long cpu = 0 ;
  cout << "Begin Engines_Component_i::CpuUsed_impl() pthread_t "<< pthread_self()
       << " _ThreadId " << _ThreadId << " errno " << errno << endl ;
  if ( _ThreadId || _Executed ) {
    if ( _ThreadId > 0 ) {
      if ( pthread_self() != _ThreadId ) {
        if ( _Sleeping ) {
        }
        else {
// Get Cpu in the appropriate thread with that object !...
          theEngines_Component = this ;
          Killer( _ThreadId ,SIGUSR1 ) ;
        }
        cpu = _ThreadCpuUsed ;
      }
      else {
        if ( _Sleeping ) {
        }
        else {
          _ThreadCpuUsed = CpuUsed() ;
	}
        cpu = _ThreadCpuUsed ;
//        cout << pthread_self() << " Engines_Component_i::CpuUsed_impl " << _serviceName << " " << cpu
//             << endl ;
      }
    }
    else {
      cpu = _ThreadCpuUsed ;
//      cout << pthread_self() << " Engines_Component_i::CpuUsed_impl " << _serviceName << " " << cpu
//           << endl ;
    }
  }
  else {
//    cout << pthread_self() << "Engines_Component_i::CpuUsed_impl _ThreadId " << _ThreadId << " "
//         << _serviceName << " _StartUsed " << _StartUsed << endl ;
  }
  cout << "End Engines_Component_i::CpuUsed_impl() pthread_t "<< pthread_self()
       << " _ThreadId " << _ThreadId << " errno " << errno << endl ;
  return cpu ;
}

// Send message to event channel

void Engines_Component_i::sendMessage(const char *event_type, const char *message) {
    _notifSupplier->Send(graphName(), nodeName(), event_type, message);
}
