//  SALOME ContainersManager
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
//  File   : ContainersManager_i.cxx
//  Author : Jean Rahuel
//  Module : SALOME
//  $Header:

using namespace std ;

#include "Utils_SINGLETON.hxx"

#include "utilities.h"
#include "ContainersManager_i.hxx"

Manager_i::Manager_i() {
  MESSAGE( "Manager_i::Manager_i default constructor" ) ;
}

void Manager_i::Init( CORBA::ORB_ptr orb ,
                      PortableServer::POA_ptr poa ,
                      SALOME_NamingService * NamingService ,
                      int argc ,
                      char ** argv ) {
  _Orb = CORBA::ORB::_duplicate( orb ) ;
  _Poa = PortableServer::POA::_duplicate( poa ) ;
  _Id = _Poa->activate_object ( this ) ;
  _NamingService = NamingService ;
  CORBA::Object_var obj = NamingService->Resolve( "/Kernel/ResourcesManager" ) ;
  _ResourcesManager = Resources::Manager::_narrow( obj ) ;
  if ( CORBA::is_nil( _ResourcesManager ) ) {
    MESSAGE( "Manager_i::Manager_i unable to locate the ResourcesManager in the NamingService" ) ;
    exit( 1 ) ;
  }
  int i ;
  i = 0 ;
  while ( argv[ i ] ) {
    MESSAGE("           argv" << i << " " << argv[ i ]) ;
    i++ ;
  }
  if ( argc == 2 && strcmp( argv[1] , "-ORBInitRef" ) == 0 && strcmp( argv[2] , "NameService=corbaname::" ) ) {
    string ORBInitRef = &argv[2][23] ;
    for ( i = 0 ; i < strlen( ORBInitRef.c_str() ) ; i++ ) {
      if ( ORBInitRef[i] == ':' ) {
        const char * CPort = ORBInitRef.c_str() ;
        int Port ;
        sscanf( &CPort[i+1] ,"%d", &_NamingServicePort) ;
        ORBInitRef[i] = '\0' ;
        _NamingServiceHostName = ORBInitRef ;
      }
    }
    if ( i == strlen( ORBInitRef.c_str() ) ) {
      _NamingServiceHostName = ORBInitRef ;
      _NamingServicePort     = 2809 ;
    }
    if ( strcmp( _NamingServiceHostName.c_str() , "localhost" ) == 0 ) {
      _NamingServiceHostName = GetHostname().c_str() ;
    }
  }
  else {
    _NamingServiceHostName = GetHostname().c_str() ;
    _NamingServicePort     = 2809 ;
    system( "netstat -apn | grep -F -e omniNames > /tmp/NsComputer.log" ) ;
    ifstream netstat( "/tmp/NsComputer.log" ) ;
    string tcp,z,ORBInitRef ;
    netstat >> tcp >> z >> z >> ORBInitRef ;
    system( "rm -f /tmp/NsComputer.log" ) ;
    for ( i = 0 ; i < strlen( ORBInitRef.c_str() ) ; i++ ) {
      if ( ORBInitRef[i] == ':' ) {
        const char * CPort = ORBInitRef.c_str() ;
        int Port ;
        sscanf( &CPort[i+1] ,"%d", &_NamingServicePort) ;
        MESSAGE( "Manager_i::Manager_i NamingService on " << _NamingServiceHostName.c_str() << " with port "
                 << _NamingServicePort ) ;
      }
    }
  }
  MESSAGE( "Manager_i::Manager_i NamingServiceHostName " << _NamingServiceHostName << " _NamingServicePort "
           << _NamingServicePort ) ;

  pthread_mutex_init( &_MutexManager , NULL ) ;

  _ListOfComputers = new Resources::ListOfComputers() ;
  _ListOfContainers = new Engines::ListOfContainers() ;
  _ListOfComponents = new Engines::ListOfComponents() ;

}

void Manager_i::destroy() {
  MESSAGE( "Manager_i::destroy" ) ;
  int i ;
  MESSAGE( "Manager_i::destroy " << _ListOfEnginesComponents.size() << " components" ) ;
  for ( i = 0 ; i < _ListOfEnginesComponents.size() ; i++ ) {
    try {
      _NamingService->Destroy_Name( _ListOfComponentsNames[ i ].c_str() ) ;
      _ListOfEnginesComponents[ i ]->destroy() ;
      MESSAGE( "Manager_i::destroy Component " << _ListOfComponentsNames[ i ] ) ;
    }
    catch ( ... ) {
      MESSAGE( "Manager_i::destroy Component " << _ListOfComponentsNames[ i ] << " failed" ) ;
    }
  }
  MESSAGE( "Manager_i::destroy " << _ListOfEnginesContainers.size() << " containers" ) ;
  for ( i = 0 ; i < _ListOfEnginesContainers.size() ; i++ ) {
    try {
      _NamingService->Destroy_Name( _ListOfContainersNames[ i ].c_str() ) ;
      _ListOfEnginesContainers[ i ]->destroy() ;
      MESSAGE( "Manager_i::destroy Container " << _ListOfContainersNames[ i ] ) ;
    }
    catch ( ... ) {
      MESSAGE( "Manager_i::destroy Container " << _ListOfContainersNames[ i ] << " failed" ) ;
    }
  }
  _Poa->deactivate_object(*_Id) ;
  CORBA::release(_Poa) ;
  delete(_Id) ;
  this->_remove_ref();
  _NamingService->Destroy_Name( "/Kernel/ContainersManager" ) ;
}

Manager_i::~Manager_i() {
  MESSAGE( "Manager_i::~Manager_i" ) ;
}

Containers::MachineParameters * Manager_i::Parameters() {
  Containers::MachineParameters_var aMachineParameters = new Containers::MachineParameters() ;
  aMachineParameters->with_xterm    = 0 ;
  aMachineParameters->Os            = Containers::Linux ;
  aMachineParameters->Memory        = 0 ;
  aMachineParameters->CpuClock      = 0 ;
  aMachineParameters->NbProc        = 1 ;
  aMachineParameters->NbNode        = 1 ;
  aMachineParameters->HostName      = CORBA::string_dup( "" ) ;
  aMachineParameters->ContainerName = CORBA::string_dup( "" ) ;
  aMachineParameters->ContainerType = Engines::Cpp ;
  aMachineParameters->NsHostName    = CORBA::string_dup( _NamingServiceHostName.c_str() ) ;
  aMachineParameters->NsPort        = _NamingServicePort ;
  return aMachineParameters._retn();
}

bool Manager_i::ping() {
  MESSAGE( "Manager_i::ping" ) ;
  return true ;
}

string Manager_i::ContainerName( const char * aComputerContainer ,
                                 string * theComputer ,
                                 string * theContainer ) {
  char * ContainerName = new char [ strlen( aComputerContainer ) + 1 ] ;
  strcpy( ContainerName , aComputerContainer ) ;
  string theComputerContainer("/Containers/");
  char * slash = strchr( ContainerName , '/' ) ;
  if ( !slash ) {
    *theComputer = GetHostname() ;
    theComputerContainer += *theComputer ;
    theComputerContainer += "/" ;
    *theContainer = ContainerName ;
    theComputerContainer += *theContainer ;
  }
  else {
    slash[ 0 ] = '\0' ;
    slash += 1 ;
    *theContainer = slash ;
    if ( !strcmp( ContainerName , "localhost" ) ) {
      *theComputer = GetHostname() ;
    }
    else {
      *theComputer = ContainerName ;
    }
    theComputerContainer += *theComputer ;
    theComputerContainer += "/" ;
    theComputerContainer += *theContainer ;
  }
  if ( strlen( theContainer->c_str() ) == 0 ) {
    theContainer = new string( "FactoryServer" ) ;
  }
  return theComputerContainer ;
}

Engines::Container_ptr Manager_i::FindContainer( const Containers::MachineParameters & myParams ) {
  Containers::MachineParameters MyParams = (Containers::MachineParameters & ) myParams ;
  Engines::Container_var aContainer = Engines::Container::_nil() ;
  if ( strcmp( MyParams.HostName ,"localhost" ) == 0 ) {
    MyParams.HostName = GetHostname().c_str() ;
  }
  MESSAGE( "Manager_i::FindContainer " << MyParams.HostName << " " << MyParams.ContainerName ) ;
  MESSAGE( "Manager_i::FindContainer MutexManager pthread_mutex_lock :" ) ;
  if ( pthread_mutex_lock( &_MutexManager ) ) {
    perror("Manager_i::FindContainer MutexManager pthread_mutex_lock ") ;
    exit( 0 ) ;
  }
  MESSAGE( "Manager_i::FindContainer MutexManager pthread_mutex_locked" ) ;
  try {
    _ListOfContainers = FindContainersLocked( MyParams ) ;
    _EnginesContainer = Engines::Container::_nil() ;
    if ( _ListOfContainers->length() ) {
      _EnginesContainer = _ListOfContainers[ 0 ] ;
      aContainer = _ListOfContainers[ 0 ] ;
      MESSAGE( "Manager_i::FindContainer --> " << aContainer->machineName() << " "
               << aContainer->name() ) ;
    }
    else {
      MESSAGE( "Manager_i::FindContainer --> Engines::Container::_nil" ) ;
    }
  }
  catch ( ... ) {
    MESSAGE( "Manager_i::FindContainer catched" ) ;
  }
  if ( pthread_mutex_unlock( &_MutexManager ) ) {
    perror("Manager_i::FindContainer MutexManager pthread_mutex_unlock ") ;
    exit( 0 ) ;
  }
  MESSAGE( "Manager_i::FindContainer MutexManager pthread_mutex_unlocked" ) ;
  return Engines::Container::_duplicate( aContainer ) ;
}

Engines::Container_ptr Manager_i::FindOneContainer( const char * aContainerName ) {
  Containers::MachineParameters * MyParams = Parameters() ;
  string theComputer ;
  string theContainer ;
  ContainerName( aContainerName , &theComputer , &theContainer ) ;
  MyParams->HostName = theComputer.c_str() ;
  MyParams->ContainerName = theContainer.c_str() ;
  int l = strlen( aContainerName ) ;
  if ( strcmp( &aContainerName[ l-2 ] , "Py" ) ) {
    MyParams->ContainerType = Engines::Cpp ;
  }
  else {
    MyParams->ContainerType = Engines::Python ;
  }
  if ( strcmp( MyParams->HostName ,"localhost" ) == 0 ) {
    MyParams->HostName = GetHostname().c_str() ;
  }
  MESSAGE( "Manager_i::FindOneContainer " << MyParams->HostName << " " << MyParams->ContainerName ) ;
  Engines::Container_ptr aContainer = FindContainer( *MyParams ) ;
  delete MyParams ;
  return aContainer ;
}

Engines::ListOfContainers * Manager_i::FindContainers( const Containers::MachineParameters & myParams ) {
  Containers::MachineParameters MyParams = (Containers::MachineParameters & ) myParams ;
  if ( strcmp( MyParams.HostName ,"localhost" ) == 0 ) {
    MyParams.HostName = GetHostname().c_str() ;
  }
  if ( strlen( MyParams.ContainerName ) && MyParams.ContainerType == Engines::Undefined ) {
    MyParams.ContainerType = Engines::Cpp ;
  }
  MESSAGE( "Manager_i::FindContainers MutexManager pthread_mutex_lock :" ) ;
  if ( pthread_mutex_lock( &_MutexManager ) ) {
    perror("Manager_i::FindContainersMutexManager pthread_mutex_lock ") ;
    exit( 0 ) ;
  }
  Engines::ListOfContainers_var aListOfContainers ;
  MESSAGE( "Manager_i::FindContainers MutexManager pthread_mutex_locked" ) ;
  try {
    aListOfContainers = FindContainersLocked( MyParams ) ;
  }
  catch ( ... ) {
    MESSAGE( "Manager_i::FindContainers catched" ) ;
  }
//  aListOfContainers->length( _ListOfContainers->length() ) ;
//  int i ;
//  for ( i = 0 ; i < _ListOfContainers->length() ; i++ ) {
//    aListOfContainers[ i ] =  Engines::Container::_duplicate( _ListOfContainers[ i ] ) ;
//    MESSAGE( "Manager_i::FindContainers " << i << ". " << _ListOfContainers[ i ]->name() ) ;
//  }
  if ( pthread_mutex_unlock( &_MutexManager ) ) {
    perror("Manager_i::FindContainers MutexManager pthread_mutex_unlock ") ;
    exit( 0 ) ;
  }
  MESSAGE( "Manager_i::FindContainers MutexManager pthread_mutex_unlocked" ) ;
  return aListOfContainers._retn() ;
}

Engines::ListOfContainers * Manager_i::FindContainersLocked( Containers::MachineParameters & MyParams ) {
  _ListOfComputers = _ResourcesManager->GetComputers( MyParams ) ;
  MESSAGE( "Manager_i::FindContainers " << MyParams.HostName << " " << MyParams.ContainerName << " "
           << _ListOfComputers->length() << " computers found" ) ;
  _ListOfContainers = new Engines::ListOfContainers() ;
  _ListOfContainers->length( 0 ) ;
  int i ;
  for ( i = 0 ; i < _ListOfComputers->length() ; i++ ) {
    _FullHostName = string( "/Containers/" ) ;
    _ResourcesComputer = _ListOfComputers[ i ] ;
    _ComputerParameters = _ResourcesComputer->Parameters() ;
    _FullHostName += _ComputerParameters->Alias ;
    if ( strlen( MyParams.ContainerName ) ) {
      _FullContainerName = _FullHostName + "/" + string( (char * ) MyParams.ContainerName ) ;
      _ContainerObject = _NamingService->Resolve( _FullContainerName.c_str() ) ;
      MESSAGE( "Manager_i::FindContainers NamingService->Resolve( " << _FullContainerName << " )" ) ;
      if ( !CORBA::is_nil( _ContainerObject ) ) {
	try {
          _EnginesContainer = Engines::Container::_narrow( _ContainerObject ) ;
          _EnginesContainer->ping() ;
	}
        catch(...) {
          _ContainerObject = CORBA::Object::_nil() ;
          _EnginesContainer = Engines::Container::_nil() ;
          MESSAGE( "Manager_i::FindContainers " << _FullContainerName << " NOT responding" ) ;
	}
      }
      if ( !CORBA::is_nil( _ContainerObject ) ) {
        _EnginesContainer = Engines::Container::_narrow( _ContainerObject ) ;
        if ( _EnginesContainer->type() == MyParams.ContainerType ) {
          int size = _ListOfContainers->length() ;
          _ListOfContainers->length( size + 1 ) ;
          _ListOfContainers[ size ] = Engines::Container::_duplicate( _EnginesContainer ) ;
          MESSAGE( "Manager_i::FindContainers --> " << _EnginesContainer->machineName() << " "
                   << _EnginesContainer->name() << " " << _EnginesContainer->type() ) ;
        }
        else {
          MESSAGE( "Manager_i::FindContainers --> " << _EnginesContainer->machineName() << " "
                   << _EnginesContainer->name() << " " << _EnginesContainer->type() << " # "
                   << MyParams.ContainerType ) ;
	}
      }
      else {
        MESSAGE( "Manager_i::FindContainers " << _FullContainerName << " unknown" ) ;
      }
    }
    else {
      if ( _NamingService->Change_Directory( _FullHostName.c_str() ) ) {
        vector<string> theListOfContainers = _NamingService->list_directory() ;
        MESSAGE( "Manager_i::FindContainers " << theListOfContainers.size() << " containers found." ) ;
        int j ;
        for ( j = 0 ; j < theListOfContainers.size() ; j++ ) {
          _FullContainerName = _FullHostName + "/" + theListOfContainers[ j ] ;
          MESSAGE( "Manager_i::FindContainers " << j << " " << _FullContainerName ) ;
          _ContainerObject = _NamingService->Resolve( _FullContainerName.c_str() ) ;
          if ( !CORBA::is_nil( _ContainerObject ) ) {
            _EnginesContainer = Engines::Container::_narrow( _ContainerObject ) ;
            if ( MyParams.ContainerType == Engines::Undefined ||
                 _EnginesContainer->type() == MyParams.ContainerType ) {
              int size = _ListOfContainers->length() ;
              _ListOfContainers->length( size + 1 ) ;
              _ListOfContainers[ size ] = Engines::Container::_duplicate( _EnginesContainer );
              MESSAGE( "Manager_i::FindContainers --> " << _EnginesContainer->machineName() << " "
                       << _EnginesContainer->name() << " " << _EnginesContainer->type() ) ;
            }
            else {
              MESSAGE( "Manager_i::FindContainers --> " << _EnginesContainer->machineName() << " "
                       << _EnginesContainer->name() << " " << _EnginesContainer->type() << " # "
                       << MyParams.ContainerType ) ;
              _EnginesContainer = Engines::Container::_nil() ;
  	    }
          }
          else {
            MESSAGE( "Manager_i::FindContainers " << _FullContainerName << " unknown" ) ;
          }
	}
      }
      
    }
  }
  return _ListOfContainers._retn() ;
}

Engines::Container_ptr Manager_i::FindOrStartContainer( const Containers::MachineParameters & myParams ) {
  Containers::MachineParameters MyParams = (Containers::MachineParameters & ) myParams ;
  if ( strcmp( MyParams.HostName ,"localhost" ) == 0 ) {
    MyParams.HostName = GetHostname().c_str() ;
  }
  Engines::Container_var aContainer = Engines::Container::_nil() ;
  MESSAGE( "Manager_i::FindOrStartContainer " << MyParams.HostName << " " << MyParams.ContainerName
           << " " << MyParams.ContainerType << " " << MyParams.Os << " " << MyParams.Memory << " "
           << MyParams.CpuClock << " " << MyParams.NbProc << " " << MyParams.NbNode << " "
           << MyParams.NsHostName << " " << MyParams.NsPort  ) ;
  MESSAGE( "Manager_i::FindOrStartContainer MutexManager pthread_mutex_lock :" ) ;
  if ( pthread_mutex_lock( &_MutexManager ) ) {
    perror("Manager_i::FindOrStartContainer MutexManager pthread_mutex_lock ") ;
    exit( 0 ) ;
  }
  MESSAGE( "Manager_i::FindOrStartContainer MutexManager pthread_mutex_locked" ) ;
  try {
    _StartContainer = true ;
    aContainer  = FindOrStartContainerLocked( MyParams , "" ) ;
  }
  catch ( ... ) {
    MESSAGE("Manager_i::FindOrStartContainer catch") ;
  }
  if ( pthread_mutex_unlock( &_MutexManager ) ) {
    perror("Manager_i::FindOrStartContainer MutexManager pthread_nriContainmutex_unlock ") ;
    exit( 0 ) ;
  }
  MESSAGE( "Manager_i::FindOrStartContainer MutexManager pthread_mutex_unlocked" ) ;
  return Engines::Container::_duplicate( aContainer ) ;
}

Engines::Container_ptr Manager_i::FindOrStartContainerLocked( Containers::MachineParameters & myParams ,
                                                              const char * aComponentName ) {
  _EnginesContainer = Engines::Container::_nil() ;
  _EnginesComponent = Engines::Component::_nil() ;
  _ListOfComponents = new Engines::ListOfComponents() ;
  _ListOfComponents->length( 0 ) ;
  _ListOfContainers = FindContainersLocked( myParams ) ;
  _ComponentName = aComponentName ;
  MESSAGE( "MutexManager FindOrStartContainerLocked " << _ListOfContainers->length() << " containers found"
           << " CORBA::is_nil( _EnginesContainer ) " << CORBA::is_nil( _EnginesContainer ) ) ;
  if ( _ListOfContainers->length() ) {
    _EnginesContainer = Engines::Container::_nil() ;
    int i ;
    for ( i = 0 ; i < _ListOfContainers->length() && ( ( _StartContainer && CORBA::is_nil( _EnginesContainer ) ) || !_StartContainer ) ; i++ ) {
      _FullHostName = "/Containers/" ;
      char * aHostName = _ListOfContainers[ i ]->machineName() ;
      _HostName = string( aHostName ) ;
      _FullHostName += _HostName ;
      char * aContainerName = _ListOfContainers[ i ]->name() ;
      aContainerName = strrchr( aContainerName , '/' ) + 1 ;
      _ContainerName = string( aContainerName ) ;
      _FullContainerName = _FullHostName + "/" + _ContainerName ;
      MESSAGE( "Manager_i::FindOrStartContainerLocked Container " << i << " " << _FullContainerName
               << " searched Component '" << _ComponentName << "'" ) ;
      if ( strlen( _ComponentName.c_str() ) ) {
        _FullComponentName = _FullContainerName + "/" + _ComponentName ;
        MESSAGE( "Manager_i::FindOrStartContainerLocked  Component " << _FullComponentName ) ;
        _ComponentObject = _NamingService->Resolve( _FullComponentName.c_str() ) ;
        if ( !CORBA::is_nil( _ComponentObject ) ) {
          _EnginesContainer = _ListOfContainers[ i ] ;
          MESSAGE( "Manager_i::FindOrStartContainerLocked --> " << _EnginesContainer->machineName() << " "
                   << _EnginesContainer->name() << " Component " << _ComponentName ) ;
          try {
            _EnginesComponent = Engines::Component::_duplicate( Engines::Component::_narrow( _ComponentObject ) ) ;
            _EnginesComponent->ping() ;
            int size = _ListOfComponents->length() ;
            _ListOfComponents->length( size + 1 ) ;
            _ListOfComponents[ size ] = Engines::Component::_duplicate( _EnginesComponent ) ;
            MESSAGE( "Manager_i::FindOrStartContainerLocked --> " << _EnginesComponent->instanceName() << " "
                     << _EnginesComponent->interfaceName() ) ;
	  }
          catch( ... ) {
            _EnginesComponent = Engines::Component::_nil() ;
            MESSAGE( "Manager_i::FindOrStartContainerLocked --> " << myParams.HostName << " "
                     << myParams.ContainerName << " " << _ComponentName << " NOT responding" ) ;
	  }
        }
        else {
          MESSAGE( "Manager_i::FindOrStartContainerLocked --> " << myParams.HostName << " "
                   << myParams.ContainerName << " " << _ComponentName << " NOT found" ) ;
	}
      }
      else {
        _EnginesContainer = _ListOfContainers[ i ] ;
        if ( _NamingService->Change_Directory( _FullContainerName.c_str() ) ) {
          vector<string> theListOfComponents = _NamingService->list_directory() ;
          MESSAGE( "Manager_i::FindOrStartContainerLocked " << theListOfComponents.size()
                   << " components found." ) ;
          int j ;
          for ( j = 0 ; j < theListOfComponents.size() ; j++ ) {
            _FullComponentName = _FullContainerName + "/" + theListOfComponents[ j ] ;
            MESSAGE( "Manager_i::FindOrStartContainerLocked " << j << " Component " << _FullComponentName ) ;
            _ComponentObject = _NamingService->Resolve( _FullComponentName.c_str() ) ;
            if ( !CORBA::is_nil( _ComponentObject ) ) {
              MESSAGE( "Manager_i::FindOrStartContainerLocked --> " << _EnginesContainer->machineName() << " "
                       << _EnginesContainer->name() ) ;
              _EnginesComponent = Engines::Component::_duplicate( Engines::Component::_narrow( _ComponentObject ) ) ;
              int size = _ListOfComponents->length() ;
              _ListOfComponents->length( size + 1 ) ;
              _ListOfComponents[ size ] = Engines::Component::_duplicate( _EnginesComponent ) ;
              MESSAGE( "Manager_i::FindOrStartContainerLocked --> " << _EnginesComponent->instanceName()
                       << " " << _EnginesComponent->interfaceName() ) ;
	    }
	  }
        }
      }
    }
  }
  else if ( !CORBA::is_nil( _EnginesContainer ) && strlen( _ComponentName.c_str() ) ) {
    _FullComponentName = _FullContainerName + "/" + _ComponentName ;
    MESSAGE( "Manager_i::FindOrStartContainerLocked  Component " << _FullComponentName ) ;
    _ComponentObject = _NamingService->Resolve( _FullComponentName.c_str() ) ;
    if ( !CORBA::is_nil( _ComponentObject ) ) {
      MESSAGE( "Manager_i::FindOrStartContainerLocked --> " << _EnginesContainer->machineName() << " "
               << _EnginesContainer->name() << " Component " << _FullComponentName ) ;
      _EnginesComponent = Engines::Component::_duplicate( Engines::Component::_narrow( _ComponentObject ) ) ;
      MESSAGE( "Manager_i::FindOrStartContainerLocked --> " << _EnginesComponent->instanceName() << " "
               << _EnginesComponent->interfaceName() ) ;
    }
    else {
      MESSAGE( "Manager_i::FindOrStartContainerLocked --> " << myParams.HostName << " "
               << myParams.ContainerName << " " << _ComponentName << " NOT found" ) ;
    }
  }
  if ( CORBA::is_nil( _EnginesContainer ) && _StartContainer ) {
    if ( _ListOfContainers->length() ) {
      _EnginesContainer = _ListOfContainers[ 0 ] ;
    }
    else {
      StartContainer( myParams ) ;
    }
    if ( strlen( _ComponentName.c_str() ) ) {
      _FullComponentName = _FullContainerName + "/" + _ComponentName ;
    }
  }
  return Engines::Container::_duplicate( _EnginesContainer ) ;
}

Engines::Container_ptr Manager_i::StartContainer( const Containers::MachineParameters & myParams ) {
  Containers::MachineParameters MyParams = (Containers::MachineParameters & ) myParams ;
  if ( _ListOfContainers->length() ) {
    MyParams.HostName = CORBA::string_dup( _ListOfContainers[ 0 ]->machineName() ) ;
    char * aContainerName = _ListOfContainers[ 0 ]->name() ;
    aContainerName = strrchr( aContainerName , '/' ) + 1 ;
    MyParams.ContainerName = CORBA::string_dup( aContainerName ) ;
  }
  else if ( _ListOfComputers->length() ) {
    MyParams.HostName = CORBA::string_dup( _ListOfComputers[ 0 ]->Alias() ) ;
    if ( strlen( MyParams.ContainerName ) == 0 ) {
      if ( MyParams.ContainerType == Engines::Cpp ) {
        MyParams.ContainerName = "FactoryServer" ;
      }
      else {
        MyParams.ContainerName = "FactoryServerPy" ;
      }
    }
  }
  MESSAGE( "Manager_i::StartContainer " << MyParams.HostName << " " << MyParams.ContainerName
           << " " << MyParams.ContainerType << " " << MyParams.Os << " " << MyParams.Memory << " "
           << MyParams.CpuClock << " " << MyParams.NbProc << " " << MyParams.NbNode << " "
           << MyParams.NsHostName << " " << MyParams.NsPort << " " <<  _ListOfContainers->length()
           << " computers" ) ;
  _ResourcesComputer = _ResourcesManager->SearchComputer( MyParams.HostName ) ;
  if ( CORBA::is_nil( _EnginesContainer ) ) {
    Resources::ComputerEnvironment * aComputerEnvironment = NULL ;
    if ( !CORBA::is_nil( _ResourcesComputer ) ) {
      aComputerEnvironment = _ResourcesComputer->Environment() ;
    }
    string rsh( "" ) ;
    char * HostName = MyParams.HostName ;
    if ( strcmp( HostName , GetHostname().c_str() ) ) {
      if ( aComputerEnvironment == NULL || _ResourcesManager->SshAccess( HostName ) ) {
        rsh += "ssh " ;
      }
      else {
        rsh += "rsh -n " ;
      }
      if ( !CORBA::is_nil( _ResourcesComputer ) ) {
        string aUserName = _ResourcesManager->UserName( HostName ) ;
        if ( aUserName.length() ) {
          rsh += "-l " ;
          rsh += aUserName ;
          rsh += " " ;
	}
      }
      rsh += MyParams.HostName ;
      rsh += " sh -c \"'" ;
      if ( aComputerEnvironment != NULL ) {
        int size = aComputerEnvironment->Module_Root_Dir_Names.length() ;
        int i ;
        bool GeomModule = false ;
        for ( i = 0 ; i < size ; i++ ) {
          if ( i > 0 ) {
            rsh += " ; " ;
          }
          rsh += aComputerEnvironment->Module_Root_Dir_Names[ i ] ;
          if ( strcmp( aComputerEnvironment->Module_Root_Dir_Names[ i ] , "GEOM_ROOT_DIR" ) == 0 ) {
            GeomModule = true ;
          }
          rsh += "=" ;
          rsh += aComputerEnvironment->Module_Root_Dir_Values[ i ] ;
          rsh += " ; export " ;
          rsh += aComputerEnvironment->Module_Root_Dir_Names[ i ] ;
        }
        if ( size > 0 ) {
          rsh += " ; " ;
        }
        rsh += "PATH=" ;
        int j ;
        string Path = (char * ) aComputerEnvironment->Path ;
        for ( j = 0 ; j < Path.size() ; j++ ) {
          if ( Path[ j ] == '$' ) {
            rsh += "\\" ;
  	  }
          rsh += Path[ j ] ;
        }
        rsh += " ; export PATH" ;
        rsh += " ; LD_LIBRARY_PATH=" ;
        string Ld_Library_Path = (char * ) aComputerEnvironment->Ld_Library_Path ;
        for ( j = 0 ; j < Ld_Library_Path.size() ; j++ ) {
          if ( Ld_Library_Path[ j ] == '$' ) {
            rsh += "\\" ;
  	  }
          rsh += Ld_Library_Path[ j ] ;
        }
        rsh += " ; export LD_LIBRARY_PATH" ;
        rsh += " ; PYTHONPATH=" ;
        string PythonPath = (char * ) aComputerEnvironment->PythonPath ;
        for ( j = 0 ; j < PythonPath.size() ; j++ ) {
          if ( PythonPath[ j ] == '$' ) {
            rsh += "\\" ;
	  }
          rsh += PythonPath[ j ] ;
        }
        rsh += " ; export PYTHONPATH" ;
        rsh += " ; CASROOT=" ;
        rsh += aComputerEnvironment->CasRoot ;
        rsh += " ; export CASROOT" ;
        rsh += " ; CSF_PluginDefaults=" ;
        rsh += "\\" ;
        rsh += "${KERNEL_ROOT_DIR}/share/salome/resources" ;
        rsh += " ; export CSF_PluginDefaults" ;
        rsh += " ; CSF_SALOMEDS_ResourcesDefaults=" ;
        rsh += "\\" ;
        rsh += "${KERNEL_ROOT_DIR}/share/salome/resources" ;
        rsh += " ; export CSF_SALOMEDS_ResourcesDefaults" ;
        if ( GeomModule ) {
          rsh += " ; CSF_GEOMDS_ResourcesDefaults=" ;
          rsh += "\\" ;
          rsh += "${GEOM_ROOT_DIR}/share/salome/resources" ;
          rsh += " ; export CSF_GEOMDS_ResourcesDefaults" ;
        }
        rsh += " ; " ;
      }
    }
    if ( MyParams.ContainerType == Engines::Cpp ||  MyParams.ContainerType == Engines::Undefined ) {
      rsh += "SALOME_Container " ;
    }
    else if ( MyParams.ContainerType == Engines::Python ) {
      rsh += "SALOME_ContainerPy.py " ;
    }
    rsh += MyParams.ContainerName ;
    rsh += " -ORBInitRef NameService=corbaname::" ;
    rsh += MyParams.NsHostName ;
    if ( MyParams.NsPort != 0 ) {
      rsh += ":" ;
      ostringstream astr ;
      astr << MyParams.NsPort ;
      rsh += astr.str().c_str() ;
    }
    rsh += " > /tmp/" ;
    rsh += MyParams.ContainerName ;
    if ( MyParams.ContainerType == Engines::Cpp || MyParams.ContainerType == Engines::Undefined ) {
      rsh += "_Cpp_" ;
    }
    else if ( MyParams.ContainerType == Engines::Python ) {
      rsh += "_Py_" ;
    }
    rsh += MyParams.HostName ;
    rsh += ".log 2>&1 " ;
    if ( strcmp( HostName , GetHostname().c_str() ) ) {
      rsh += "'\"" ;
    }
    rsh += " &" ;
    cout << endl << endl << endl << endl << "StartContainer " << rsh << endl ;
    int status = system( rsh.c_str() ) ;
    if (status == -1) {
      INFOS("StartContainer rsh/ssh failed (system command status -1)") ;
    }
    else if (status == 217) {
      INFOS("StartContainer rsh/ssh failed (system command status 217)") ;
    }
    else {
      int count = 21 ;
      while ( CORBA::is_nil( _EnginesContainer ) && count ) {
        sleep( 1 ) ;
        count-- ;
        if ( count != 21 ) {
          MESSAGE( "StartContainer" << count << ". Waiting for " << MyParams.ContainerName << " on "
                   << MyParams.HostName ) ;
	}
        FindContainersLocked( MyParams ) ;
      }
      if ( CORBA::is_nil( _EnginesContainer ) ) {
        INFOS("StartContainer rsh/ssh failed " ) ;
      }
    }
  }
  else {
    MESSAGE( "Manager_i::StartContainer -> Container start_impl( " << _EnginesContainer->name() << ") for "
             << MyParams.ContainerName ) ;
    _EnginesContainer = _EnginesContainer->start_impl( MyParams.ContainerName , MyParams.ContainerType ) ;
  }
  if ( !CORBA::is_nil( _EnginesContainer ) ) {
    int size = _ListOfEnginesContainers.size() ;
    _ListOfEnginesContainers.resize( size + 1 ) ;
    _ListOfContainersNames.resize( size + 1 ) ;
    _ListOfEnginesContainers[ size ] = Engines::Container::_duplicate( _EnginesContainer ) ;
    _ListOfContainersNames[ size ] = _EnginesContainer->name() ;
  }
  return Engines::Container::_duplicate( _EnginesContainer ) ;
}

Engines::Component_ptr Manager_i::FindComponent( const Containers::MachineParameters & myParams ,
                                                 const char * ComponentName ) {
  MESSAGE( "Manager_i::FindComponent MutexManager pthread_mutex_lock :" ) ;
  if ( pthread_mutex_lock( &_MutexManager ) ) {
    perror("Manager_i::FindComponent MutexManager pthread_mutex_lock ") ;
    exit( 0 ) ;
  }
  MESSAGE( "Manager_i::FindComponent MutexManager pthread_mutex_locked" ) ;
  Engines::Component_ptr aComponent = Engines::Component::_nil() ;
  try {
    _StartContainer = false ;
    aComponent = Find_Component( myParams , ComponentName ) ;
  }
  catch ( ... ) {
    MESSAGE( "Manager_i::FindComponent catched" ) ;
  }
  if ( pthread_mutex_unlock( &_MutexManager ) ) {
    perror("Manager_i::FindComponent MutexManager pthread_mutex_unlock ") ;
    exit( 0 ) ;
  }
  MESSAGE( "Manager_i::FindComponent MutexManager pthread_mutex_unlocked" ) ;
  return aComponent ;
}

Engines::Component_ptr Manager_i::Find_Component( const Containers::MachineParameters & myParams ,
                                                  const char * ComponentName ) {
  Containers::MachineParameters MyParams = (Containers::MachineParameters & ) myParams ;
  if ( strcmp( MyParams.HostName ,"localhost" ) == 0 ) {
    MyParams.HostName = GetHostname().c_str() ;
  }
  MESSAGE( "FindComponent HostName " << MyParams.HostName << " ContainerName " << MyParams.ContainerName
           << " ComponentName " << ComponentName ) ;
  FindOrStartContainerLocked( (Containers::MachineParameters & ) MyParams , ComponentName ) ;
  if ( CORBA::is_nil( _EnginesComponent ) ) {
    MESSAGE( "FindComponent ComponentName " << ComponentName << " NOT found" ) ;
  }
  else {
    MESSAGE( "FindComponent instanceName " << _EnginesComponent->instanceName() << " interfaceName "
             << _EnginesComponent->interfaceName() ) ;
  }
  return Engines::Component::_duplicate( _EnginesComponent ) ;
}

Engines::Component_ptr Manager_i::FindOneComponent( const char * aContainerName ,
                                                    const char * ComponentName ) {
  Containers::MachineParameters * MyParams = Parameters() ;
  string theComputer ;
  string theContainer ;
  ContainerName( aContainerName , &theComputer , &theContainer ) ;
  MyParams->HostName = theComputer.c_str() ;
  MyParams->ContainerName = theContainer.c_str() ;
  int l = strlen( aContainerName ) ;
  if ( strcmp( &aContainerName[ l-2 ] , "Py" ) ) {
    MyParams->ContainerType = Engines::Cpp ;
  }
  else {
    MyParams->ContainerType = Engines::Python ;
  }
  if ( strcmp( MyParams->HostName ,"localhost" ) == 0 ) {
    MyParams->HostName = GetHostname().c_str() ;
  }
  MESSAGE( "Manager_i::FindOneComponent " << MyParams->HostName << " " << MyParams->ContainerName ) ;
  MESSAGE( "FindComponents HostName " << MyParams->HostName << " ContainerName " << MyParams->ContainerName
           << " ComponentName " << ComponentName ) ;
  MESSAGE( "Manager_i::FindOneComponent MutexManager pthread_mutex_lock :" ) ;
  if ( pthread_mutex_lock( &_MutexManager ) ) {
    perror("Manager_i::FindOneComponent MutexManager pthread_mutex_lock ") ;
    exit( 0 ) ;
  }
  MESSAGE( "Manager_i::FindOneComponent MutexManager pthread_mutex_locked" ) ;
  Engines::Component_ptr aComponent = Find_Component( *MyParams , ComponentName ) ;
  if ( pthread_mutex_unlock( &_MutexManager ) ) {
    perror("Manager_i::FindOneComponent MutexManager pthread_mutex_unlock ") ;
    exit( 0 ) ;
  }
  MESSAGE( "Manager_i::FindOneComponent MutexManager pthread_mutex_unlocked" ) ;
  delete MyParams ;
  return aComponent ;
}

Engines::ListOfComponents * Manager_i::FindComponents( const Containers::MachineParameters & myParams ,
                                                       const char * ComponentName ) {
  Containers::MachineParameters MyParams = (Containers::MachineParameters & ) myParams ;
  if ( strcmp( MyParams.HostName ,"localhost" ) == 0 ) {
    MyParams.HostName = GetHostname().c_str() ;
  }
  MESSAGE( "FindComponents HostName " << MyParams.HostName << " ContainerName " << MyParams.ContainerName
           << " ComponentName " << ComponentName ) ;
  MESSAGE( "Manager_i::FindComponents MutexManager pthread_mutex_lock :" ) ;
  if ( pthread_mutex_lock( &_MutexManager ) ) {
    perror("Manager_i::FindComponents MutexManager pthread_mutex_lock ") ;
    exit( 0 ) ;
  }
  MESSAGE( "Manager_i::FindComponents MutexManager pthread_mutex_locked" ) ;
  Engines::ListOfComponents_var aListOfComponents = new Engines::ListOfComponents() ;
  try {
    _StartContainer = false ;
    _EnginesContainer = FindOrStartContainerLocked( (Containers::MachineParameters & ) MyParams , ComponentName ) ;
    if ( _ListOfComponents->length() ) {
      MESSAGE( "FindComponents " << _ListOfComponents->length() << " components" ) ;
      int i ;
      for ( i = 0 ; i < _ListOfComponents->length() ; i++ ) {
        MESSAGE( "FindComponents" << i << " instanceName " << _ListOfComponents[ i ]->instanceName()
                 << " interfaceName " << _ListOfComponents[ i ]->interfaceName() ) ;
        _ListOfComponents[ i ]->ping() ;
      }
    }
    else if ( !CORBA::is_nil( _EnginesComponent ) ) {
      _ListOfComponents->length( 1 ) ;
      _ListOfComponents[ 0 ] = Engines::Component::_duplicate( _EnginesComponent ) ;
      MESSAGE( "FindComponents instanceName " << _EnginesComponent->instanceName() << " interfaceName "
               << _EnginesComponent->interfaceName() << " " << _ListOfComponents->length() << " components" ) ;
      _ListOfComponents[ 0 ]->ping() ;
    }
    else {
      MESSAGE( "FindComponents ComponentName " << ComponentName << " NOT found" ) ;
    }
    aListOfComponents = _ListOfComponents ;
  }
  catch ( ... ) {
    MESSAGE( "Manager_i::FindComponents catched" ) ;
  }
  if ( pthread_mutex_unlock( &_MutexManager ) ) {
    perror("Manager_i::FindComponentsMutexManager pthread_mutex_unlock ") ;
    exit( 0 ) ;
  }
  MESSAGE( "Manager_i::FindComponentsMutexManager pthread_mutex_unlocked" ) ;
  return aListOfComponents._retn() ;
}


Engines::Component_ptr Manager_i::FindOrLoad_ComponentPath( const Containers::MachineParameters & myParams ,
                                                            const char * ComponentName ,
                                                            const char * ImplementationPath ) {
  Engines::Container_var aContainer = Engines::Container::_nil() ;
  Engines::Component_var EnginesComponent = Engines::Component::_nil() ;
  Containers::MachineParameters MyParams = (Containers::MachineParameters & ) myParams ;
  if ( strcmp( MyParams.HostName ,"localhost" ) == 0 ) {
    MyParams.HostName = GetHostname().c_str() ;
  }
  BEGIN_OF("FindOrLoad_Component( const Containers::MachineParameters & MyParams , const char * ComponentName , const char * ImplementationPath 1)");
  MESSAGE( "Manager_i::FindOrLoad_ComponentPath MutexManager pthread_mutex_lock :" ) ;
  if ( pthread_mutex_lock( &_MutexManager ) ) {
    perror("Manager_i::FindOrLoad_ComponentPath MutexManager pthread_mutex_lock ") ;
    exit( 0 ) ;
  }
  MESSAGE( "Manager_i::FindOrLoad_ComponentPath MutexManager pthread_mutex_locked" ) ;
  try {
    _StartContainer = true ;
    EnginesComponent = Find_Component(  MyParams , ComponentName ) ;
    if ( CORBA::is_nil( EnginesComponent ) ) {
      MESSAGE("FindOrLoad_ComponentPath Component not found " << _ComponentName ) ;
//      aContainer = FindOrStartContainerLocked( (Containers::MachineParameters & ) MyParams , ComponentName ) ;
    }
    else {
      MESSAGE("FindOrLoad_ComponentPath Component was found " << _ComponentName ) ;
    }
    aContainer = _EnginesContainer ;
  }
  catch ( ... ) {
    MESSAGE("FindOrLoad_ComponentPath catch") ;
  }
  string aComponentName = _ComponentName ;
  string aFullComponentName = _FullComponentName ;
  if ( pthread_mutex_unlock( &_MutexManager ) ) {
    perror("Manager_i::FindOrLoad_ComponentPath MutexManager pthread_mutex_unlock ") ;
    exit( 0 ) ;
  }
  MESSAGE( "Manager_i::FindOrLoad_ComponentPath MutexManager pthread_mutex_unlocked" ) ;
  if ( !CORBA::is_nil( aContainer ) && CORBA::is_nil( EnginesComponent ) ) {
    MESSAGE("FindOrLoad_ComponentPath Component not found ! trying to load " << aFullComponentName ) ;
    EnginesComponent = aContainer->load_impl( ComponentName, ImplementationPath ) ;
    if ( !CORBA::is_nil( EnginesComponent ) ) {
      MESSAGE( "FindOrLoad_ComponentPath Component launched ! " << aFullComponentName ) ;
      try {
	EnginesComponent->ping() ; 
      }
      catch ( CORBA::COMM_FAILURE& ) {
	INFOS("FindOrLoad_ComponentPath Caught CORBA::SystemException CommFailure. Engine " << aFullComponentName << "does not respond" ) ;
        EnginesComponent = Engines::Component::_nil() ;
      }
    }
    else {
      MESSAGE( "FindOrLoad_ComponentPath Component NOT launched ! " << aFullComponentName ) ;
    }
  }
  if ( !CORBA::is_nil( _EnginesComponent ) ) {
    MESSAGE( "Manager_i::FindOrLoad_ComponentPath MutexManager pthread_mutex_lock :" ) ;
    if ( pthread_mutex_lock( &_MutexManager ) ) {
      perror("Manager_i::FindOrLoad_ComponentPath MutexManager pthread_mutex_lock ") ;
      exit( 0 ) ;
    }
    MESSAGE( "Manager_i::FindOrLoad_ComponentPath MutexManager pthread_mutex_locked" ) ;
    int size = _ListOfEnginesComponents.size() ;
    _ListOfEnginesComponents.resize( size + 1 ) ;
    _ListOfComponentsNames.resize( size + 1 ) ;
    _ListOfEnginesComponents[ size ] = Engines::Component::_duplicate( EnginesComponent ) ;
    _ListOfComponentsNames[ size ] = aFullComponentName ;
    if ( pthread_mutex_unlock( &_MutexManager ) ) {
      perror("Manager_i::FindOrLoad_ComponentPath MutexManager pthread_mutex_unlock ") ;
      exit( 0 ) ;
    }
    MESSAGE( "Manager_i::FindOrLoad_ComponentPath MutexManager pthread_mutex_unlocked" ) ;
  }
  return Engines::Component::_duplicate( EnginesComponent ) ;
}

Engines::Component_ptr Manager_i::FindOrLoad_Component( const Containers::MachineParameters & myParams ,
                                                        const char * ComponentName ) {
  Engines::Container_var aContainer = Engines::Container::_nil() ;
  Engines::Component_var EnginesComponent =  Engines::Component::_nil() ;
  Containers::MachineParameters MyParams = (Containers::MachineParameters & ) myParams ;
  if ( strcmp( MyParams.HostName ,"localhost" ) == 0 ) {
    MyParams.HostName = GetHostname().c_str() ;
  }
  BEGIN_OF("FindOrLoad_Component( const Containers::MachineParameters & MyParams , const char * ComponentName )");
  MESSAGE( "Manager_i::FindOrLoad_Component MutexManager pthread_mutex_lock :" ) ;
  if ( pthread_mutex_lock( &_MutexManager ) ) {
    perror("Manager_i::FindOrLoad_Component MutexManager pthread_mutex_lock ") ;
    exit( 0 ) ;
  }
  MESSAGE( "Manager_i::FindOrLoad_Component MutexManager pthread_mutex_locked" ) ;
  try {
    _StartContainer = true ;
    EnginesComponent = Find_Component(  MyParams , ComponentName ) ;
    if ( CORBA::is_nil( EnginesComponent ) ) {
      MESSAGE("FindOrLoad_Component Component not found " << _FullComponentName ) ;
//      aContainer = FindOrStartContainerLocked( (Containers::MachineParameters & ) MyParams , ComponentName ) ;
    }
    else {
      MESSAGE("FindOrLoad_Component Component was found " << _FullComponentName ) ;
    }
    aContainer = _EnginesContainer ;
  }
  catch ( ... ) {
    MESSAGE("FindOrLoad_Component catch") ;
  }
  string aComponentName = _ComponentName ;
  string aFullComponentName = _FullComponentName ;
  if ( pthread_mutex_unlock( &_MutexManager ) ) {
    perror("Manager_i::FindOrLoad_Component MutexManager pthread_mutex_unlock ") ;
    exit( 0 ) ;
  }
  MESSAGE( "Manager_i::FindOrLoad_Component MutexManager pthread_mutex_unlocked" ) ;
  if ( !CORBA::is_nil( aContainer ) && CORBA::is_nil( EnginesComponent ) ) {
    MESSAGE("FindOrLoad_Component Component not found ! trying to load " << aFullComponentName ) ;
    string ImplementationPath( "lib" ) ;
    ImplementationPath += ComponentName ;
    ImplementationPath += "Engine.so" ;
    try {
      EnginesComponent = aContainer->load_impl( ComponentName, ImplementationPath.c_str() ) ;
      if ( !CORBA::is_nil( EnginesComponent ) ) {
        MESSAGE( "FindOrLoad_Component Component launched ! " << _ComponentName << " on " << aFullComponentName ) ;
        try {
	  EnginesComponent->ping() ; 
        }
        catch ( CORBA::COMM_FAILURE& ) {
	  INFOS("FindOrLoad_Component Caught CORBA::SystemException CommFailure. Engine " << aFullComponentName << "does not respond" ) ;
          EnginesComponent = Engines::Component::_nil() ;
        }
      }
      else {
        MESSAGE( "FindOrLoad_Component Component NOT launched ! " << aFullComponentName ) ;
      }
    }
    catch (...) {
      EnginesComponent = Engines::Component::_nil() ;
      MESSAGE( "Component NOT launched ! " << aFullComponentName << " load_impl catched" ) ;
    }
  }
  if ( !CORBA::is_nil( EnginesComponent ) ) {
    MESSAGE( "Manager_i::FindOrLoad_Component MutexManager pthread_mutex_lock :" ) ;
    if ( pthread_mutex_lock( &_MutexManager ) ) {
      perror("Manager_i::FindOrLoad_Component MutexManager pthread_mutex_lock ") ;
      exit( 0 ) ;
    }
    MESSAGE( "Manager_i::FindOrLoad_Component MutexManager pthread_mutex_locked" ) ;
    int size = _ListOfEnginesComponents.size() ;
    _ListOfEnginesComponents.resize( size + 1 ) ;
    _ListOfComponentsNames.resize( size + 1 ) ;
    _ListOfEnginesComponents[ size ] = Engines::Component::_duplicate( EnginesComponent ) ;
    _ListOfComponentsNames[ size ] = aFullComponentName ;
    MESSAGE( "FindOrLoad_Component " << aFullComponentName << " added in listofcomponents" ) ;
    int i ;
    for ( i = 0 ; i < size + 1 ; i++ ) {
      MESSAGE( "FindOrLoad_Component " << i << ". " << _ListOfComponentsNames[ i ] ) ;
    }
    if ( pthread_mutex_unlock( &_MutexManager ) ) {
      perror("Manager_i::FindOrLoad_Component MutexManager pthread_mutex_unlock ") ;
      exit( 0 ) ;
    }
    MESSAGE( "Manager_i::FindOrLoad_Component MutexManager pthread_mutex_unlocked" ) ;
  }
  return Engines::Component::_duplicate( EnginesComponent ) ;
}
//  try {
//    CORBA::Object_var obj = _NS->Resolve(path.c_str());
//    if ( CORBA::is_nil( obj ) ) {
//      MESSAGE("Component not found ! trying to load " << path);
//	  CORBA::Object_var obj2 = _NS->Resolve("/Kernel/ModulCatalog");
//	  SALOME_ModuleCatalog::ModuleCatalog_var Catalog = 
//	    SALOME_ModuleCatalog::ModuleCatalog::_narrow(obj2);

//	  SALOME_ModuleCatalog::Acomponent_ptr compoInfo = 
//	    Catalog->GetComponent(componentName);
//	  if (CORBA::is_nil (compoInfo)) 
//	    {
//	      INFOS("Catalog Error : Component not found in the catalog")
//              return Engines::Component::_nil();
//		exit (-1);
//	    }
	  
//	  string  path;
//	  try
//	    {
//	      path = compoInfo->GetPathPrefix( machine ) ;
//              path += "/" ;
//	    }
//	  catch (SALOME_ModuleCatalog::NotFound&)
//	    {
//	      MESSAGE("GetPathPrefix(" << machine << ") not found!"
//		      << "trying localhost");
//	      try {
//                path = compoInfo->GetPathPrefix("localhost") ;
//                path += "/" ;
//	      }
//	      catch (SALOME_ModuleCatalog::NotFound&) {
//	        MESSAGE("GetPathPrefix(localhost) not found!") ;
//                path = "" ;
//	      }
//	    }

//	  SCRUTE(path); 
//	  string implementation(path);
//	  implementation += "lib";
//	  implementation += componentName;
//	  implementation += "Engine.so";
	  
//	  Engines::Component_var compo 
//	    = cont->load_impl(componentName, implementation.c_str());

//	  ASSERT(!CORBA::is_nil(compo));
//	  MESSAGE("Component launched !" << path);
//	  return compo;
//    }
//    else
//	{
//	  MESSAGE("Component found !" << path);
//	  Engines::Component_var compo = Engines::Component::_narrow(obj);
//	  ASSERT(!CORBA::is_nil(compo));
//	  try
//	    {
//	      string instanceName = compo->instanceName(); 
//	    }
//	  catch (CORBA::COMM_FAILURE&)
//	    {
//	      INFOS("Caught CORBA::SystemException CommFailure. Engine "
//		    << path << "does not respond" );
//	    }
//	  return compo;
//	}
//    }
//  catch (ServiceUnreachable&)
//    {
//      INFOS("Caught exception: Naming Service Unreachable");
//    }
//  catch (...)
//    {
//      INFOS("Caught unknown exception.");
//    }
//  return Engines::Component::_nil();
//}

