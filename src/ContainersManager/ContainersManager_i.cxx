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

#include "Utils_SINGLETON.hxx"

#include "utilities.h"
#include "ContainersManager_i.hxx"

using namespace std ;

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
    MESSAGE("           argv" << i << " '" << argv[ i ] << "'" ) ;
    i++ ;
  }
  if ( argc == 3 && strcmp( argv[1] , "-ORBInitRef" ) == NULL &&
       strcmp( argv[2] , "NameService=corbaname::localhost" ) == NULL ) {
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
  if ( pthread_mutex_unlock( &_MutexManager ) ) {
    perror("Manager_i::destroy MutexManager pthread_mutex_unlock ") ;
  }
  int i ;
  Containers::MachineParameters * MyParams = Parameters() ;
  MyParams->ContainerType = Engines::UndefinedContainerType ;
  MESSAGE( "Manager_i::destroy components FindComponents :" ) ;
  Engines::ListOfComponents_var aListOfEnginesComponents = FindComponents( *MyParams , "" ) ;
  MESSAGE( "Manager_i::destroy components FindContainers :" ) ;
  Engines::ListOfContainers_var aListOfEnginesContainers = FindContainers( *MyParams ) ;
  MESSAGE( "Manager_i::destroy " << aListOfEnginesComponents->length() << " components" ) ;
  for ( i = 0 ; i < aListOfEnginesComponents->length() ; i++ ) {
    string interfaceName = aListOfEnginesComponents[ i ]->interfaceName() ;
    try {
      MESSAGE( "Manager_i::destroy trying to destroy Component " << interfaceName ) ;
      aListOfEnginesComponents[ i ]->destroy() ;
      MESSAGE( "Manager_i::destroy Component " << interfaceName << " destroyed" ) ;
    }
    catch ( ... ) {
      MESSAGE( "Manager_i::destroy Component " << interfaceName << " failed" ) ;
    }
  }
  MESSAGE( "Manager_i::destroy " << aListOfEnginesContainers->length() << " containers" ) ;
  for ( i = 0 ; i < aListOfEnginesContainers->length() ; i++ ) {
    string aContainerName ;
    string aComputerName ;
    ostringstream astr ;
    try {
      aContainerName = aListOfEnginesContainers[ i ]->name() ;
      aComputerName = aListOfEnginesContainers[ i ]->machineName() ;
      astr << aListOfEnginesContainers[ i ]->getPID() ;
      MESSAGE( "Manager_i::destroy trying to destroy Container " << aContainerName ) ;
      aListOfEnginesContainers[ i ]->destroy() ;
      MESSAGE( "Manager_i::destroy Container " << aContainerName << " destroyed" ) ;
    }
    catch ( ... ) {
      MESSAGE( "Manager_i::destroy destroy of Container " << aContainerName << " failed" ) ;
    }
    string KillContainer ;
    if ( strcmp( aComputerName.c_str() , GetHostname().c_str() ) ) {
      if ( _ResourcesManager->SshAccess( aComputerName.c_str() ) ) {
        KillContainer = "ssh " ;
      }
      else {
        KillContainer = "rsh " ;
      }
      KillContainer += aComputerName ;
    }
    KillContainer += string( "kill " ) ;
    KillContainer += astr.str() ;
//    KillContainer += " &" ;
    int status = system( KillContainer.c_str() ) ;
    if ( status == 0 ) {
      MESSAGE( "Manager_i::destroy kill of Container " << aContainerName << " "
               << KillContainer << " successsed" ) ;
    }
    else {
      MESSAGE( "Manager_i::destroy kill of Container " << aContainerName << " "
               << KillContainer << " failed" ) ;
    }
  }
  _Poa->deactivate_object( *_Id ) ;
  CORBA::release( _Poa ) ;
#if !WITH_SINGLETON
  delete( _Id ) ;
  this->_remove_ref();
#endif
  MESSAGE( "Manager_i::destroy NamingService->Destroy_Name /Kernel/ContainersManager" ) ;
  _NamingService->Destroy_Name( "/Kernel/ContainersManager" ) ;
  MESSAGE( "Manager_i::destroyed" ) ;
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
  aMachineParameters->ContainerType = Engines::CppContainer ;
  aMachineParameters->NsHostName    = CORBA::string_dup( _NamingServiceHostName.c_str() ) ;
  aMachineParameters->NsPort        = _NamingServicePort ;
  return aMachineParameters._retn();
}

bool Manager_i::ping() {
  MESSAGE( "Manager_i::ping" ) ;
  return true ;
}

//string Manager_i::ContainerName( const char * aComputerContainer ,
//                                 string * theComputer ,
//                                 string * theContainer ) {
//  char * ContainerName = new char [ strlen( aComputerContainer ) + 1 ] ;
//  strcpy( ContainerName , aComputerContainer ) ;
//  string theComputerContainer("/Containers/");
//  char * slash = strchr( ContainerName , '/' ) ;
//  if ( !slash ) {
//    *theComputer = GetHostname() ;
//    theComputerContainer += *theComputer ;
//    theComputerContainer += "/" ;
//    *theContainer = ContainerName ;
//    theComputerContainer += *theContainer ;
//  }
//  else {
//    slash[ 0 ] = '\0' ;
//    slash += 1 ;
//    *theContainer = slash ;
//    if ( !strcmp( ContainerName , "localhost" ) ) {
//      *theComputer = GetHostname() ;
//    }
//    else {
//      *theComputer = ContainerName ;
//    }
//    theComputerContainer += *theComputer ;
//    theComputerContainer += "/" ;
//    theComputerContainer += *theContainer ;
//  }
//  if ( strlen( theContainer->c_str() ) == 0 ) {
//    theContainer = new string( "FactoryServer" ) ;
//  }
//  return theComputerContainer ;
//}


Engines::ListOfContainers * Manager_i::AllContainers() {
  MESSAGE( "Manager_i::AllContainers" ) ;
  Engines::ListOfContainers_var aListOfContainers ;
  Containers::MachineParameters_var aMachineParameters = Parameters() ;
  aMachineParameters->ContainerType = Engines::UndefinedContainerType ;
  aListOfContainers = FindContainers( aMachineParameters ) ;
  return aListOfContainers._retn() ;
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

  aContainer = FindContainerLocked( MyParams ) ;

  if ( pthread_mutex_unlock( &_MutexManager ) ) {
    perror("Manager_i::FindContainer MutexManager pthread_mutex_unlock ") ;
    exit( 0 ) ;
  }
  MESSAGE( "Manager_i::FindContainer MutexManager pthread_mutex_unlocked" ) ;
  return Engines::Container::_duplicate( aContainer ) ;
}

Engines::Container_ptr Manager_i::FindContainerLocked( Containers::MachineParameters & MyParams ) {
  Engines::Container_var aContainer = Engines::Container::_nil() ;
  try {
    _ListOfContainers = FindContainersLocked( MyParams , false ) ;
    _EnginesContainer = Engines::Container::_nil() ;
    if ( _ListOfContainers->length() ) {
      _EnginesContainer = _ListOfContainers[ 0 ] ;
      aContainer = _ListOfContainers[ 0 ] ;
      MESSAGE( "Manager_i::FindContainerLocked --> " << aContainer->machineName() << " "
               << aContainer->name() ) ;
    }
    else {
      MESSAGE( "Manager_i::FindContainerLocked --> Engines::Container::_nil" ) ;
    }
  }
  catch ( ... ) {
    MESSAGE( "Manager_i::FindContainerLocked catched" ) ;
  }
  return Engines::Container::_duplicate( aContainer ) ;
}

Engines::Container_ptr Manager_i::FindOneContainer( const char * aHostName ,
                                                    const char * aContainerName ) {
  Containers::MachineParameters * MyParams = Parameters() ;
  string theComputer = aHostName ;
  string theContainer = aContainerName ;
//  ContainerName( aContainerName , &theComputer , &theContainer ) ;
  MyParams->HostName = theComputer.c_str() ;
  MyParams->ContainerName = theContainer.c_str() ;
  MyParams->ContainerType = Engines::UndefinedContainerType ;
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
  MESSAGE( "Manager_i::FindContainers MutexManager pthread_mutex_lock :" ) ;
  if ( pthread_mutex_lock( &_MutexManager ) ) {
    perror("Manager_i::FindContainersMutexManager pthread_mutex_lock ") ;
    exit( 0 ) ;
  }
  Engines::ListOfContainers_var aListOfContainers ;
  MESSAGE( "Manager_i::FindContainers MutexManager pthread_mutex_locked" ) ;
  try {
    aListOfContainers = FindContainersLocked( MyParams , false ) ;
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

Engines::ListOfContainers * Manager_i::FindContainersLocked( Containers::MachineParameters & MyParams , bool _StartContainer ) {
  MESSAGE( "Manager_i::FindContainersLocked GetComputers :" ) ;
  _ListOfComputers = _ResourcesManager->GetComputers( MyParams ) ;
  MESSAGE( "Manager_i::FindContainersLocked " << MyParams.HostName << " "
           << MyParams.ContainerName << " " << _ListOfComputers->length()
           << " computers found" ) ;
  _ListOfContainers = new Engines::ListOfContainers() ;
  _ListOfContainers->length( 0 ) ;
  int NumberOfComputers = _ListOfComputers->length() ;
  if ( NumberOfComputers == 0 ) {
    NumberOfComputers = 1 ;
  }
  int i ;
  for ( i = 0 ; i < NumberOfComputers ; i++ ) {
    _FullHostName = string( "/Containers/" ) ;
    if ( _ListOfComputers->length() ) {
      _ResourcesComputer = _ListOfComputers[ i ] ;
      _ComputerParameters = _ResourcesComputer->Parameters() ;
      _FullHostName += _ComputerParameters->Alias ;
    }
    else {
      _FullHostName += MyParams.HostName ;
    }
    if ( strlen( MyParams.ContainerName ) ) {
      _FullContainerName = _FullHostName + "/" + string( (char * ) MyParams.ContainerName ) ;
      _ContainerObject = _NamingService->Resolve( _FullContainerName.c_str() ) ;
      MESSAGE( "Manager_i::FindContainersLocked NamingService->Resolve( "
               << _FullContainerName << " )" ) ;
      if ( !CORBA::is_nil( _ContainerObject ) ) {
        try {
          _EnginesContainer = Engines::Container::_narrow( _ContainerObject ) ;
          _EnginesContainer->ping() ;
	}
        catch(...) {
          _ContainerObject = CORBA::Object::_nil() ;
          _EnginesContainer = Engines::Container::_nil() ;
          MESSAGE( "Manager_i::FindContainersLocked " << _FullContainerName << " NOT responding" ) ;
        }
      }
      if ( !CORBA::is_nil( _ContainerObject ) ) {
        try {
          _EnginesContainer = Engines::Container::_narrow( _ContainerObject ) ;
          if ( MyParams.ContainerType == Engines::UndefinedContainerType ||
            _EnginesContainer->type() == MyParams.ContainerType ) {
            int size = _ListOfContainers->length() ;
            _ListOfContainers->length( size + 1 ) ;
            _ListOfContainers[ size ] = Engines::Container::_duplicate( _EnginesContainer ) ;
            MESSAGE( "Manager_i::FindContainersLocked --> "
                     << _EnginesContainer->machineName() << " "
                     << _EnginesContainer->name() << " " << _EnginesContainer->type() ) ;
          }
          else {
            MESSAGE( "Manager_i::FindContainersLocked --> "
                     << _EnginesContainer->machineName() << " "
                     << _EnginesContainer->name() << " "
                     << _EnginesContainer->type() << " # "
                     << MyParams.ContainerType ) ;
  	  }
        }
        catch ( ... ) {
          MESSAGE( "Manager_i::FindContainersLocked " << _FullContainerName
                   << " catched NOT responding " ) ;
        }
      }
      else {
        MESSAGE( "Manager_i::FindContainers " << _FullContainerName << " unknown" ) ;
      }
    }
    else {
      if ( _NamingService->Change_Directory( _FullHostName.c_str() ) ) {
        vector<string> theListOfContainers = _NamingService->list_directory() ;
        MESSAGE( "Manager_i::FindContainersLocked " << theListOfContainers.size()
                 << " containers found." ) ;
        int j ;
        for ( j = 0 ; j < theListOfContainers.size() ; j++ ) {
          _FullContainerName = _FullHostName + "/" + theListOfContainers[ j ] ;
          MESSAGE( "Manager_i::FindContainersLocked " << j << " " << _FullContainerName ) ;
          _ContainerObject = _NamingService->Resolve( _FullContainerName.c_str() ) ;
          if ( !CORBA::is_nil( _ContainerObject ) ) {
            try {
              _EnginesContainer = Engines::Container::_narrow( _ContainerObject ) ;
              _EnginesContainer->ping() ;
              if ( MyParams.ContainerType == Engines::UndefinedContainerType ||
                   _EnginesContainer->type() == MyParams.ContainerType ) {
                int size = _ListOfContainers->length() ;
                _ListOfContainers->length( size + 1 ) ;
                _ListOfContainers[ size ] = Engines::Container::_duplicate( _EnginesContainer );
                MESSAGE( "Manager_i::FindContainersLocked --> "
                         << _EnginesContainer->machineName() << " "
                         << _EnginesContainer->name() << " "
                         << _EnginesContainer->type() ) ;
              }
              else {
                MESSAGE( "Manager_i::FindContainersLocked --> "
                         << _EnginesContainer->machineName() << " "
                         << _EnginesContainer->name() << " "
                         << _EnginesContainer->type() << " # "
                         << MyParams.ContainerType ) ;
                _EnginesContainer = Engines::Container::_nil() ;
  	      }
	    }
            catch( ... ) {
              MESSAGE( "Manager_i::FindContainersLocked " << _FullContainerName
                       << " catched NOT responding" ) ;
              _EnginesContainer = Engines::Container::_nil() ;
	    }
          }
          else {
            MESSAGE( "Manager_i::FindContainersLocked " << _FullContainerName
                     << " unknown" ) ;
          }
	}
      }
      else {
//        MESSAGE( "Manager_i::FindContainersLocked " << _FullHostName.c_str()
//                 << " not found in Naming Service" ) ;
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
  if ( MyParams.ContainerType == Engines::UndefinedContainerType ) {
    MyParams.ContainerType == Engines::CppContainer ;
  }
  Engines::Container_var aContainer = Engines::Container::_nil() ;
  MESSAGE( "Manager_i::FindOrStartContainer " << MyParams.HostName << " "
           << MyParams.ContainerName
           << " " << MyParams.ContainerType << " " << MyParams.Os << " "
           << MyParams.Memory << " "
           << MyParams.CpuClock << " " << MyParams.NbProc << " "
           << MyParams.NbNode << " "
           << MyParams.NsHostName << " " << MyParams.NsPort  ) ;
  MESSAGE( "Manager_i::FindOrStartContainer MutexManager pthread_mutex_lock :" ) ;
  if ( pthread_mutex_lock( &_MutexManager ) ) {
    perror("Manager_i::FindOrStartContainer MutexManager pthread_mutex_lock ") ;
    exit( 0 ) ;
  }
  MESSAGE( "Manager_i::FindOrStartContainer MutexManager pthread_mutex_locked" ) ;
  try {
    aContainer  = FindOrStartContainerLocked( MyParams , "" , true ) ;
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
                                                              const char * aComponentName ,
                                                              bool _StartContainer ) {
  _EnginesContainer = Engines::Container::_nil() ;
  _EnginesComponent = Engines::Component::_nil() ;
  _ListOfComponents = new Engines::ListOfComponents() ;
  _ListOfComponents->length( 0 ) ;
  MESSAGE( "Manager_i::FindOrStartContainerLocked FindContainersLocked :" ) ;
  _ListOfContainers = FindContainersLocked( myParams , _StartContainer ) ;
  _ComponentName = aComponentName ;
  MESSAGE( "MutexManager FindOrStartContainerLocked "
           << _ListOfContainers->length() << " containers found"
           << " CORBA::is_nil( _EnginesContainer ) "
           << CORBA::is_nil( _EnginesContainer ) ) ;
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
      MESSAGE( "Manager_i::FindOrStartContainerLocked Container " << i << " "
               << _FullContainerName
               << " searched Component '" << _ComponentName << "'" ) ;
      if ( strlen( _ComponentName.c_str() ) ) {
        _FullComponentName = _FullContainerName + "/" + _ComponentName ;
        MESSAGE( "Manager_i::FindOrStartContainerLocked  Component "
                 << _FullComponentName ) ;
        _ComponentObject = _NamingService->Resolve( _FullComponentName.c_str() ) ;
        if ( !CORBA::is_nil( _ComponentObject ) ) {
          _EnginesContainer = _ListOfContainers[ i ] ;
          MESSAGE( "Manager_i::FindOrStartContainerLocked --> "
                   << _EnginesContainer->machineName() << " "
                   << _EnginesContainer->name() << " Component "
                   << _ComponentName ) ;
          try {
            _EnginesComponent = Engines::Component::_duplicate( Engines::Component::_narrow( _ComponentObject ) ) ;
            _EnginesComponent->ping() ;
            int size = _ListOfComponents->length() ;
            _ListOfComponents->length( size + 1 ) ;
            _ListOfComponents[ size ] = Engines::Component::_duplicate( _EnginesComponent ) ;
            MESSAGE( "Manager_i::FindOrStartContainerLocked --> "
                     << _EnginesComponent->instanceName() << " "
                     << _EnginesComponent->interfaceName() ) ;
	  }
          catch( ... ) {
            _EnginesComponent = Engines::Component::_nil() ;
            MESSAGE( "Manager_i::FindOrStartContainerLocked --> "
                     << myParams.HostName << " "
                     << myParams.ContainerName << " " << _ComponentName
                     << " NOT responding" ) ;
	  }
        }
        else {
          MESSAGE( "Manager_i::FindOrStartContainerLocked --> "
                   << myParams.HostName << " "
                   << myParams.ContainerName << " " << _ComponentName
                   << " NOT found" ) ;
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
            MESSAGE( "Manager_i::FindOrStartContainerLocked " << j
                     << " Component " << _FullComponentName ) ;
            _ComponentObject = _NamingService->Resolve( _FullComponentName.c_str() ) ;
            if ( !CORBA::is_nil( _ComponentObject ) ) {
              MESSAGE( "Manager_i::FindOrStartContainerLocked --> "
                       << _EnginesContainer->machineName() << " "
                       << _EnginesContainer->name() ) ;
              try {
                _EnginesComponent = Engines::Component::_duplicate( Engines::Component::_narrow( _ComponentObject ) ) ;
                _EnginesComponent->ping() ;
                int size = _ListOfComponents->length() ;
                _ListOfComponents->length( size + 1 ) ;
                _ListOfComponents[ size ] = Engines::Component::_duplicate( _EnginesComponent ) ;
                MESSAGE( "Manager_i::FindOrStartContainerLocked --> "
                         << _EnginesComponent->instanceName()
                         << " " << _EnginesComponent->interfaceName() ) ;
	      }
              catch ( ... ) {
                MESSAGE( "Manager_i::FindOrStartContainerLocked catched NOT responding" ) ;
                _EnginesComponent = Engines::Component::_nil() ;
	      }
	    }
	  }
        }
      }
    }
  }
  else if ( !CORBA::is_nil( _EnginesContainer ) && strlen( _ComponentName.c_str() ) ) {
    _FullComponentName = _FullContainerName + "/" + _ComponentName ;
    MESSAGE( "Manager_i::FindOrStartContainerLocked  Component "
             << _FullComponentName ) ;
    _ComponentObject = _NamingService->Resolve( _FullComponentName.c_str() ) ;
    if ( !CORBA::is_nil( _ComponentObject ) ) {
      try {
        MESSAGE( "Manager_i::FindOrStartContainerLocked --> "
                 << _EnginesContainer->machineName() << " "
                 << _EnginesContainer->name() << " Component "
                 << _FullComponentName ) ;
        _EnginesComponent = Engines::Component::_duplicate( Engines::Component::_narrow( _ComponentObject ) ) ;
        _EnginesComponent->ping() ;
        MESSAGE( "Manager_i::FindOrStartContainerLocked --> "
                 << _EnginesComponent->instanceName() << " "
                 << _EnginesComponent->interfaceName() ) ;
      }
      catch ( ... ) {
        MESSAGE( "Manager_i::FindOrStartContainerLocked --> "
                 << myParams.HostName << " "
                 << myParams.ContainerName << " " << _ComponentName
                 << " catched NOT responding" ) ;
        _EnginesComponent = Engines::Component::_nil() ;
      }
    }
    else {
      MESSAGE( "Manager_i::FindOrStartContainerLocked --> " << myParams.HostName
               << " " << myParams.ContainerName << " " << _ComponentName
               << " NOT found" ) ;
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
  MESSAGE( "return from FindOrStartContainerLocked " ) ;
  return Engines::Container::_duplicate( _EnginesContainer ) ;
}

Engines::Container_ptr Manager_i::StartContainer( const Containers::MachineParameters & myParams ) {
  Containers::MachineParameters MyParams = (Containers::MachineParameters & ) myParams ;
  MESSAGE( "Manager_i::StartContainer HostName '" << MyParams.HostName
           << "' ContainerName '" << MyParams.ContainerName << "'" ) ;
  if ( _ListOfContainers->length() ) {
    MyParams.HostName = CORBA::string_dup( _ListOfContainers[ 0 ]->machineName() ) ;
    char * aContainerName = _ListOfContainers[ 0 ]->name() ;
    aContainerName = strrchr( aContainerName , '/' ) + 1 ;
    MyParams.ContainerName = CORBA::string_dup( aContainerName ) ;
  }
  else if ( _ListOfComputers->length() ) {
    Resources::Computer_var aComputer = _ResourcesManager->GetComputer( _ListOfComputers ) ;
    if ( !CORBA::is_nil( aComputer ) ) {
      MyParams.HostName = CORBA::string_dup( aComputer->Alias() ) ;
    }
    if ( strlen( MyParams.ContainerName ) == 0 ) {
      if ( MyParams.ContainerType == Engines::CppContainer ) {
        MyParams.ContainerName = "FactoryServer" ;
      }
      else {
        MyParams.ContainerName = "FactoryServerPy" ;
      }
    }
    Containers::MachineParameters MyContainerParams = MyParams ;
    MyContainerParams.ContainerName = "" ;
    FindContainerLocked( MyContainerParams ) ;
  }
  MESSAGE( "Manager_i::StartContainer MyParams " << MyParams.HostName << " "
           << MyParams.ContainerName << " " << MyParams.ContainerType << " "
           << MyParams.Os << " " << MyParams.Memory << " " << MyParams.CpuClock
           << " " << MyParams.NbProc << " " << MyParams.NbNode << " "
           << MyParams.NsHostName << " " << MyParams.NsPort << " "
           <<  _ListOfContainers->length() << " containers " << _ListOfComputers->length()
           << " computers localhost " << GetHostname().c_str() ) ;
  _ResourcesComputer = _ResourcesManager->SearchComputer( MyParams.HostName ) ;
  if ( CORBA::is_nil( _EnginesContainer ) ) {
    Resources::ComputerEnvironment * aComputerEnvironment = NULL ;
    if ( !CORBA::is_nil( _ResourcesComputer ) ) {
      aComputerEnvironment = _ResourcesComputer->Environment() ;
    }
    string rsh( "" ) ;
    char * HostName = MyParams.HostName ;
    MESSAGE( "Manager_i::StartContainer MyParams '" << MyParams.HostName
             << "' GetHostname '" <<  GetHostname().c_str() << "'" ) ;
    if ( strcmp( HostName , GetHostname().c_str() ) ) {
      if ( aComputerEnvironment == NULL || _ResourcesManager->SshAccess( HostName ) ) {
        rsh += "ssh " ;
      }
      else {
        rsh += "rsh " ;
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
//        bool GeomModule = false ;
        for ( i = 0 ; i < size ; i++ ) {
          if ( i > 0 ) {
            rsh += " ; " ;
          }
          rsh += aComputerEnvironment->Module_Root_Dir_Names[ i ] ;
//          if ( strcmp( aComputerEnvironment->Module_Root_Dir_Names[ i ] , "GEOM_ROOT_DIR" ) == 0 ) {
//            GeomModule = true ;
//          }
          rsh += "=" ;
          rsh += aComputerEnvironment->Module_Root_Dir_Values[ i ] ;
          rsh += " ; export " ;
          rsh += aComputerEnvironment->Module_Root_Dir_Names[ i ] ;
        }
        if ( size > 0 ) {
          rsh += " ; " ;
        }
//        rsh += "${PRODUCTS_ROOT_DIR}/env_products.sh ; " ;
        rsh += "${KERNEL_ROOT_DIR}/env_products.sh ; " ; // Pour l'instant
        rsh += "${KERNEL_ROOT_DIR}/env_modules.sh ; " ;
//        rsh += "PATH=" ;
//        int j ;
//        string Path = (char * ) aComputerEnvironment->Path ;
//        for ( j = 0 ; j < Path.size() ; j++ ) {
//          if ( Path[ j ] == '$' ) {
//            rsh += "\\" ;
//  	  }
//          rsh += Path[ j ] ;
//        }
//        rsh += " ; export PATH" ;
//        rsh += " ; LD_LIBRARY_PATH=" ;
//        string Ld_Library_Path = (char * ) aComputerEnvironment->Ld_Library_Path ;
//        for ( j = 0 ; j < Ld_Library_Path.size() ; j++ ) {
//          if ( Ld_Library_Path[ j ] == '$' ) {
//            rsh += "\\" ;
//  	  }
//          rsh += Ld_Library_Path[ j ] ;
//        }
//        rsh += " ; export LD_LIBRARY_PATH" ;
//        rsh += " ; PYTHONPATH=" ;
//        string PythonPath = (char * ) aComputerEnvironment->PythonPath ;
//        for ( j = 0 ; j < PythonPath.size() ; j++ ) {
//          if ( PythonPath[ j ] == '$' ) {
//            rsh += "\\" ;
//	  }
//          rsh += PythonPath[ j ] ;
//        }
//        rsh += " ; export PYTHONPATH" ;
//        rsh += " ; CASROOT=" ;
//        rsh += aComputerEnvironment->CasRoot ;
//        rsh += " ; export CASROOT" ;
//        rsh += " ; CSF_PluginDefaults=" ;
//        rsh += "\\" ;
//        rsh += "${KERNEL_ROOT_DIR}/share/salome/resources" ;
//        rsh += " ; export CSF_PluginDefaults" ;
//        rsh += " ; CSF_SALOMEDS_ResourcesDefaults=" ;
//        rsh += "\\" ;
//        rsh += "${KERNEL_ROOT_DIR}/share/salome/resources" ;
//        rsh += " ; export CSF_SALOMEDS_ResourcesDefaults" ;
//        if ( GeomModule ) {
//          rsh += " ; CSF_GEOMDS_ResourcesDefaults=" ;
//          rsh += "\\" ;
//          rsh += "${GEOM_ROOT_DIR}/share/salome/resources" ;
//          rsh += " ; export CSF_GEOMDS_ResourcesDefaults" ;
//        }
//        rsh += " ; " ;
      }
    }
    if ( MyParams.ContainerType == Engines::CppContainer ||  MyParams.ContainerType == Engines::UndefinedContainerType ) {
      rsh += "SALOME_Container " ;
    }
    else if ( MyParams.ContainerType == Engines::PythonContainer ) {
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
//CCRT    char * tmpdir = getenv( "TMPDIR" ) ;
//CCRT    rsh += " > " ;
//CCRT    if ( tmpdir ) {
//CCRT      rsh += tmpdir ;
//CCRT      rsh += "/" ;
//CCRT    }
//CCRT    else {
//CCRT      rsh += "/tmp/" ;
//CCRT    }
    rsh += MyParams.ContainerName ;
    if ( MyParams.ContainerType == Engines::CppContainer || MyParams.ContainerType == Engines::UndefinedContainerType ) {
      rsh += "_Cpp_" ;
    }
    else if ( MyParams.ContainerType == Engines::PythonContainer ) {
      rsh += "_Py_" ;
    }
    rsh += MyParams.HostName ;
    rsh += ".log 2>&1 " ;
    if ( strcmp( HostName , GetHostname().c_str() ) ) {
      rsh += "'\"" ;
    }
    rsh += " &" ;
    cout << endl << endl << "Manager_i::StartContainer " << rsh << endl << endl ;
    MESSAGE( "Manager_i::StartContainer " << rsh ) ;
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
          MESSAGE( "StartContainer" << count << ". Waiting for "
                   << MyParams.ContainerName << " on " << MyParams.HostName ) ;
	}
        FindContainersLocked( MyParams , false ) ;
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
  return Engines::Container::_duplicate( _EnginesContainer ) ;
}

Engines::ListOfComponents * Manager_i::AllComponents() {
  MESSAGE( "Manager_i::AllComponents" ) ;
  Engines::ListOfComponents_var aListOfComponents ;
  Containers::MachineParameters_var aMachineParameters = Parameters() ;
  aMachineParameters->ContainerType = Engines::UndefinedContainerType ;
  aListOfComponents = FindComponents( aMachineParameters , "" ) ;
  return aListOfComponents._retn() ;
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
    aComponent = FindComponentLocked( myParams , ComponentName , false ) ;
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

Engines::Component_ptr Manager_i::FindComponentLocked( const Containers::MachineParameters & myParams ,
                                                       const char * ComponentName ,
                                                       bool _StartContainer ) {
  Containers::MachineParameters MyParams = (Containers::MachineParameters & ) myParams ;
  if ( strcmp( MyParams.HostName ,"localhost" ) == 0 ) {
    MyParams.HostName = GetHostname().c_str() ;
  }
  MESSAGE( "FindComponentLocked HostName " << MyParams.HostName
           << " ContainerName " << MyParams.ContainerName
           << " ComponentName " << ComponentName ) ;
  FindOrStartContainerLocked( (Containers::MachineParameters & ) MyParams , ComponentName , _StartContainer ) ;
  if ( CORBA::is_nil( _EnginesComponent ) ) {
    MESSAGE( "FindComponentLocked ComponentName " << ComponentName << " NOT found" ) ;
  }
  else {
    MESSAGE( "FindComponentLocked instanceName " << _EnginesComponent->instanceName()
             << " interfaceName " << _EnginesComponent->interfaceName() ) ;
  }
  return Engines::Component::_duplicate( _EnginesComponent ) ;
}

Engines::Component_ptr Manager_i::FindOneComponent( const char * aHostName ,
                                                    const char * aContainerName ,
                                                    const char * ComponentName ) {
  Containers::MachineParameters * MyParams = Parameters() ;
  string theComputer = aHostName ;
  string theContainer = aContainerName ;
//  ContainerName( aContainerName , &theComputer , &theContainer ) ;
  MyParams->HostName = theComputer.c_str() ;
  MyParams->ContainerName = theContainer.c_str() ;
  MyParams->ContainerType = Engines::UndefinedContainerType ;
  if ( strcmp( MyParams->HostName ,"localhost" ) == 0 ) {
    MyParams->HostName = GetHostname().c_str() ;
  }
  MESSAGE( "Manager_i::FindOneComponent HostName " << MyParams->HostName
           << " ContainerName " << MyParams->ContainerName
           << " ComponentName " << ComponentName ) ;
  MESSAGE( "Manager_i::FindOneComponent MutexManager pthread_mutex_lock :" ) ;
  if ( pthread_mutex_lock( &_MutexManager ) ) {
    perror("Manager_i::FindOneComponent MutexManager pthread_mutex_lock ") ;
    exit( 0 ) ;
  }
  MESSAGE( "Manager_i::FindOneComponent MutexManager pthread_mutex_locked" ) ;
  Engines::Component_ptr aComponent = FindComponentLocked( *MyParams , ComponentName , false ) ;
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
  MESSAGE( "FindComponents HostName " << myParams.HostName << " ContainerName "
           << myParams.ContainerName
           << " ComponentName " << ComponentName ) ;
  Containers::MachineParameters MyParams = (Containers::MachineParameters & ) myParams ;
  if ( strcmp( MyParams.HostName ,"localhost" ) == 0 ) {
    MyParams.HostName = GetHostname().c_str() ;
  }
  MESSAGE( "FindComponents HostName " << MyParams.HostName << " ContainerName "
           << MyParams.ContainerName
           << " ComponentName " << ComponentName ) ;
  MESSAGE( "Manager_i::FindComponents MutexManager pthread_mutex_lock :" ) ;
  if ( pthread_mutex_lock( &_MutexManager ) ) {
    perror("Manager_i::FindComponents MutexManager pthread_mutex_lock ") ;
    exit( 0 ) ;
  }
  MESSAGE( "Manager_i::FindComponents MutexManager pthread_mutex_locked" ) ;
  Engines::ListOfComponents_var aListOfComponents = FindComponentsLocked( MyParams , ComponentName ) ;
  if ( pthread_mutex_unlock( &_MutexManager ) ) {
    perror("Manager_i::FindComponents MutexManager pthread_mutex_unlock ") ;
    exit( 0 ) ;
  }
  MESSAGE( "Manager_i::FindComponents MutexManager pthread_mutex_unlocked" ) ;
  return aListOfComponents._retn() ;
}


Engines::ListOfComponents * Manager_i::FindComponentsLocked( const Containers::MachineParameters & MyParams ,
                                                             const char * ComponentName ) {
  Engines::ListOfComponents_var aListOfComponents = new Engines::ListOfComponents() ;
  try {
     MESSAGE( "Manager_i::FindComponentsLocked FindOrStartContainerLocked :" ) ;
    _EnginesContainer = FindOrStartContainerLocked( (Containers::MachineParameters & ) MyParams , ComponentName , false ) ;
    if ( _ListOfComponents->length() ) {
      MESSAGE( "FindComponentsLocked " << _ListOfComponents->length() << " components" ) ;
      int i ;
      for ( i = 0 ; i < _ListOfComponents->length() ; i++ ) {
        MESSAGE( "FindComponentsLocked " << i << " instanceName "
                 << _ListOfComponents[ i ]->instanceName()
                 << " interfaceName " << _ListOfComponents[ i ]->interfaceName() ) ;
        _ListOfComponents[ i ]->ping() ;
      }
    }
    else if ( !CORBA::is_nil( _EnginesComponent ) ) {
      _ListOfComponents->length( 1 ) ;
      _ListOfComponents[ 0 ] = Engines::Component::_duplicate( _EnginesComponent ) ;
      MESSAGE( "FindComponentsLocked instanceName "
               << _EnginesComponent->instanceName() << " interfaceName "
               << _EnginesComponent->interfaceName() << " "
               << _ListOfComponents->length() << " components" ) ;
      _ListOfComponents[ 0 ]->ping() ;
    }
    else {
      MESSAGE( "FindComponentsLocked ComponentName " << ComponentName
               << " NOT found" ) ;
    }
    aListOfComponents = _ListOfComponents ;
  }
  catch ( ... ) {
    MESSAGE( "Manager_i::FindComponentsLocked catched" ) ;
  }
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
    EnginesComponent = FindComponentLocked(  MyParams , ComponentName , true ) ;
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
    MESSAGE("FindOrLoad_ComponentPath Component not found ! trying to load "
            << aFullComponentName ) ;
    EnginesComponent = aContainer->load_impl( ComponentName, ImplementationPath ) ;
    if ( !CORBA::is_nil( EnginesComponent ) ) {
      MESSAGE( "FindOrLoad_ComponentPath Component launched ! "
               << aFullComponentName ) ;
      try {
	EnginesComponent->ping() ; 
      }
      catch ( CORBA::COMM_FAILURE& ) {
	INFOS("FindOrLoad_ComponentPath Caught CORBA::SystemException CommFailure. Engine "
              << aFullComponentName << "does not respond" ) ;
        EnginesComponent = Engines::Component::_nil() ;
      }
    }
    else {
      MESSAGE( "FindOrLoad_ComponentPath Component NOT launched ! "
               << aFullComponentName ) ;
    }
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
  if ( !strcmp( MyParams.ContainerName , "" ) &&
       MyParams.ContainerType == Engines::UndefinedContainerType ) {
    MyParams.ContainerType = Engines::CppContainer ;
  }
  BEGIN_OF("FindOrLoad_Component HostName " << MyParams.HostName << " ContainerName "
           << MyParams.ContainerName << " ContainerType " << MyParams.ContainerType
           << " ComponentName " << ComponentName );
  MESSAGE( "Manager_i::FindOrLoad_Component MutexManager pthread_mutex_lock :" ) ;
  if ( pthread_mutex_lock( &_MutexManager ) ) {
    perror("Manager_i::FindOrLoad_Component MutexManager pthread_mutex_lock ") ;
    exit( 0 ) ;
  }
  MESSAGE( "Manager_i::FindOrLoad_Component MutexManager pthread_mutex_locked" ) ;
  try {
    EnginesComponent = FindComponentLocked(  MyParams , ComponentName , true ) ;
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
    string ImplementationPath ;
    if ( MyParams.ContainerType == Engines::PythonContainer ) {
      ImplementationPath += ComponentName ;
    }
    else {
      ImplementationPath += string( "lib" ) ;
      ImplementationPath += ComponentName ;
      ImplementationPath += "Engine.so" ;
    }
    MESSAGE("FindOrLoad_Component Component not found ! trying to load "
            << aFullComponentName << " " << ImplementationPath ) ;
    try {
      EnginesComponent = aContainer->load_impl( ComponentName, ImplementationPath.c_str() ) ;
      if ( !CORBA::is_nil( EnginesComponent ) ) {
        MESSAGE( "FindOrLoad_Component Component launched ! " << _ComponentName
                 << " on " << aFullComponentName ) ;
        try {
	  EnginesComponent->ping() ; 
        }
        catch ( CORBA::COMM_FAILURE& ) {
	  INFOS("FindOrLoad_Component Caught CORBA::SystemException CommFailure. Engine "
                << aFullComponentName << "does not respond" ) ;
          EnginesComponent = Engines::Component::_nil() ;
        }
      }
      else {
        MESSAGE( "FindOrLoad_Component Component NOT launched ! "
                 << aFullComponentName ) ;
      }
    }
    catch (...) {
      EnginesComponent = Engines::Component::_nil() ;
      MESSAGE( "Component NOT launched ! " << aFullComponentName << " load_impl catched" ) ;
    }
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

bool Manager_i::DestroyContainer( const char * aHostName ,
                                  const char * aContainerName ) {
  Containers::MachineParameters * MyParams = Parameters() ;
  string theComputer = aHostName ;
  string theContainer = aContainerName ;
  MyParams->HostName = theComputer.c_str() ;
  MyParams->ContainerName = theContainer.c_str() ;
  int l = strlen( aContainerName ) ;
  MyParams->ContainerType = Engines::UndefinedContainerType ;
  if ( strcmp( MyParams->HostName ,"localhost" ) == 0 ) {
    MyParams->HostName = GetHostname().c_str() ;
  }
  MESSAGE( "Manager_i::DestroyContainer HostName " << MyParams->HostName
           << " ContainerName " << MyParams->ContainerName ) ;
  MESSAGE( "Manager_i::DestroyContainer MutexManager pthread_mutex_lock :" ) ;
  if ( pthread_mutex_lock( &_MutexManager ) ) {
    perror("Manager_i::DestroyContainer MutexManager pthread_mutex_lock ") ;
    exit( 0 ) ;
  }
  MESSAGE( "Manager_i::DestroyContainer MutexManager pthread_mutex_locked" ) ;
  Engines::ListOfComponents_var aListOfComponents = FindComponentsLocked( *MyParams , "" ) ;
  Engines::Container_ptr aContainer = _EnginesContainer ;
  if ( pthread_mutex_unlock( &_MutexManager ) ) {
    perror("Manager_i::DestroyContainer MutexManager pthread_mutex_unlock ") ;
    exit( 0 ) ;
  }
  MESSAGE( "Manager_i::DestroyContainer MutexManager pthread_mutex_unlocked" ) ;
  int i ;
  for ( i = 0 ; i < aListOfComponents->length() ; i++ ) {
    try {
      MESSAGE( "Manager_i::DestroyContainer trying to destroy component "
               << aListOfComponents[ i ]->interfaceName() ) ;
      aListOfComponents[ i ]->ping() ;
      aListOfComponents[ i ]->destroy() ;
      MESSAGE( "Manager_i::DestroyContainer component destroyed" ) ;
    }
    catch ( ... ) {
      MESSAGE( "Manager_i::DestroyContainer destroy component catched" ) ;
    }
  }
  try {
    MESSAGE( "Manager_i::DestroyContainer trying to destroy container " << aContainer->name() ) ;
    aContainer->ping() ;
    aContainer->destroy() ;
    MESSAGE( "Manager_i::DestroyContainer " << aContainerName << " on " << aHostName
             << " destroyed" ) ;
  }
  catch ( ... ) {
    MESSAGE( "Manager_i::DestroyContainer " << aContainerName << " on " << aHostName
             << " catched" ) ;
  }
  delete MyParams ;
  return true ;
}

bool Manager_i::DestroyContainers( const Containers::MachineParameters & MyParams ) {
  MESSAGE( "Manager_i::DestroyContainers HostName " << MyParams.HostName
           << " ContainerName " << MyParams.ContainerName ) ;
  MESSAGE( "Manager_i::DestroyContainers MutexManager pthread_mutex_lock :" ) ;
  if ( pthread_mutex_lock( &_MutexManager ) ) {
    perror("Manager_i::DestroyContainers MutexManager pthread_mutex_lock ") ;
    exit( 0 ) ;
  }
  MESSAGE( "Manager_i::DestroyContainers MutexManager pthread_mutex_locked" ) ;
  Engines::ListOfComponents_var aListOfComponents = FindComponentsLocked( MyParams , "" ) ;
  Engines::ListOfContainers_var aListOfContainers = _ListOfContainers ;
  if ( pthread_mutex_unlock( &_MutexManager ) ) {
    perror("Manager_i::DestroyContainers MutexManager pthread_mutex_unlock ") ;
    exit( 0 ) ;
  }
  MESSAGE( "Manager_i::DestroyContainers MutexManager pthread_mutex_unlocked" ) ;
  int i ;
  for ( i = 0 ; i < aListOfComponents->length() ; i++ ) {
    try {
      aListOfComponents[ i ]->destroy() ;
      MESSAGE( "Manager_i::DestroyContainers component destroyed" ) ;
    }
    catch ( ... ) {
      MESSAGE( "Manager_i::DestroyContainers destroy component catched" ) ;
    }
  }
  for ( i = 0 ; i < aListOfContainers->length() ; i++ ) {
    try {
      MESSAGE( "Manager_i::DestroyContainers destroy of " << aListOfContainers[ i ]->name() ) ;
      aListOfContainers[ i ]->destroy() ;
    }
    catch ( ... ) {
      MESSAGE( "Manager_i::DestroyContainers catched" ) ;
    }
  }
  return true ;
}

ostream & operator<< (ostream & f ,const Engines::ContainerType & t ) {
  switch (t) {
  case Engines::UndefinedContainerType :
    f << "UndefinedContainer";
    break;
  case Engines::CppContainer :
    f << "CppContainer";
    break;
  case Engines::PythonContainer :
    f << "PythonContainer";
    break;
  default :
    f << "UnknownContainerType";
    break;
  }

  return f;
}

