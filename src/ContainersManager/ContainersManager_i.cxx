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

#include "utilities.h"
#include "ContainersManager_i.hxx"

Manager_i::Manager_i() {
}

Manager_i::Manager_i( CORBA::ORB_ptr orb ,
                      SALOME_NamingService * NamingService ,
                      int argc ,
                      char ** argv ) {
  _Orb = orb ;
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

Engines::Container_ptr Manager_i::FindContainer( const Containers::MachineParameters & MyParams ) {
  MESSAGE( "Manager_i::FindContainer starting" ) ;
  CORBA::Object_var ContainerObject = CORBA::Object::_nil() ;
  _EnginesContainer = Engines::Container::_nil() ;
  string _ContainerName = string( "/Containers/" ) ;
  Containers::MachineParameters & myParams = (Containers::MachineParameters & ) MyParams ;
  if ( strcmp( MyParams.HostName ,"localhost" ) == 0 ) {
    myParams.HostName = GetHostname().c_str() ;
  }
  _ContainerName += myParams.HostName ;
  if ( strlen( MyParams.ContainerName ) == 0 ) {
    MESSAGE( "Manager_i::FindContainer ContainerName " << _ContainerName.c_str() ) ;
    if ( _NamingService->Change_Directory( _ContainerName.c_str() ) ) {
      vector<string> aListOfContainers = _NamingService->list_directory() ;
      if ( aListOfContainers.size() ) {
        _ContainerName += "/" ;
        _ContainerName += aListOfContainers[ 0 ] ;
        MESSAGE( "Manager_i::FindContainer ContainerName " << _ContainerName.c_str() ) ;
      }
    }
  }
  else {
    _ContainerName += "/" ;
    _ContainerName += MyParams.ContainerName ;
    MESSAGE( "Manager_i::FindContainer ContainerName " << _ContainerName.c_str() ) ;
  }
  try {
    MESSAGE( "Manager_i::FindContainer ContainerName " << _ContainerName.c_str() ) ;
    ContainerObject = _NamingService->Resolve( _ContainerName.c_str() ) ;
    if ( !CORBA::is_nil( ContainerObject ) ) {
      _EnginesContainer = Engines::Container::_narrow( ContainerObject ) ;
      MESSAGE( "Manager_i::FindContainer Container " << _ContainerName.c_str() << " is known ==> ping" ) ;
      _EnginesContainer->ping() ;
    }
    else {
      MESSAGE( "Manager_i::FindContainer Container " << _ContainerName.c_str() << " unknown" ) ;
    }
  }
  catch (ServiceUnreachable&) {
    MESSAGE( "Caught exception: Naming Service Unreachable" );
  }
  catch (...) {
    MESSAGE( "Caught unknown exception." );
  }
  MESSAGE( "Manager_i::FindContainer ended" ) ;
  return Engines::Container::_duplicate( _EnginesContainer ) ;
}

Engines::ListOfContainers * Manager_i::Containers( const Containers::MachineParameters & MyParams ) {
  MESSAGE("Manager_i::Containers()") ;
  Engines::ListOfContainers_var aListOfContainers = new Engines::ListOfContainers() ;
  CORBA::Object_var ContainerObject = CORBA::Object::_nil() ;
  anEnginesContainer = Engines::Container::_nil() ;
  string aContainerName = string( "/Containers/" ) ;
  if ( strcmp( MyParams.HostName ,"localhost" ) == 0 ) {
    myParams.HostName = GetHostname().c_str() ;
  }
  if ( strlen( MyParams.HostName ) ) { // HostName
    aContainerName += MyParams.HostName ;
    if ( strlen( MyParams.ContainerName ) ) { // ContainerName
      aContainerName += "/" ;
      aContainerName += MyParams.ContainerName ;
      ContainerObject = _NamingService->Resolve( aContainerName.c_str() ) ;
      if ( !CORBA::is_nil( ContainerObject ) ) {
        anEnginesContainer = Engines::Container::_narrow( ContainerObject ) ;
        int size = aListOfContainers.length() ;
        aListOfContainers.length( size + 1 ) ;
        aListOfContainers[ size ] = anEnginesContainer ;
      }
    }
    else { // Pas de ContainerName
      if ( _NamingService->Change_Directory( _ContainerName.c_str() ) ) {
        vector<string> theListOfContainers = _NamingService->list_directory() ;
        int i ;
        for ( i = 0 ; i < theListOfContainers->length() ; i++ ) {
          string ContainersNames = aContainerName + "/" + theListOfContainers[ i ] ;
          ContainerObject = _NamingService->Resolve( ContainersNames.c_str() ) ;
          if ( !CORBA::is_nil( ContainerObject ) ) {
            anEnginesContainer = Engines::Container::_narrow( ContainerObject ) ;
            int size = aListOfContainers.length() ;
            aListOfContainers.length( size + 1 ) ;
            aListOfContainers[ size ] = anEnginesContainer ;
          }
	}
      }
    }
  }
  else { // Pas de HostName
    if ( _NamingService->Change_Directory( _ContainerName.c_str() ) ) {
      vector<string> aListOfHosts = _NamingService->list_directory() ;
      int j ;
      for ( j = 0 ; j < aListOfHosts.size() ; j++ ) {
        string HostsNames = aContainerName + aListOfHosts[ j ] ;
        if ( strlen( MyParams.ContainerName ) ) { // ContainerName
          HostsNames = HostsNames + "/" + MyParams.ContainerName ;
          ContainerObject = _NamingService->Resolve( HostsNames.c_str() ) ;
          if ( !CORBA::is_nil( ContainerObject ) ) {
            anEnginesContainer = Engines::Container::_narrow( ContainerObject ) ;
            int size = aListOfContainers.length() ;
            aListOfContainers.length( size + 1 ) ;
            aListOfContainers[ size ] = anEnginesContainer ;
	  }
	}
        else { // Pas de ContainerName
          if ( _NamingService->Change_Directory( HostsNames.c_str() ) ) {
            vector<string> theListOfContainers = _NamingService->list_directory() ;
            int k ;
            for ( k = 0 ; k < aListOfContainers.size() ; k++ ) {
              string ContainersNames = HostsNames + theListOfContainers[ k ] ;
              ContainerObject = _NamingService->Resolve( HostsNames.c_str() ) ;
              if ( !CORBA::is_nil( ContainerObject ) ) {
                anEnginesContainer = Engines::Container::_narrow( ContainerObject ) ;
                int size = aListOfContainers.length() ;
                aListOfContainers.length( size + 1 ) ;
                aListOfContainers[ size ] = anEnginesContainer ;
	      }
	    }
	  }
	}
      }
    }
  }

  aListOfContainers._retn() ;
}

Engines::Container_ptr Manager_i::FindOrStartContainer( const Containers::MachineParameters & MyParams ) {
  Engines::Container_var aContainer = Engines::Container::_nil() ;
  MESSAGE( "MutexManager pthread_mutex_lock :" ) ;
  if ( pthread_mutex_lock( &_MutexManager ) ) {
    perror("MutexManager pthread_mutex_lock ") ;
    exit( 0 ) ;
  }
  MESSAGE( "MutexManager pthread_mutex_locked" ) ;
  Containers::MachineParameters & myParams = (Containers::MachineParameters & ) MyParams ;
  aContainer = FindOrStartContainerLocked( myParams , "" ) ;
  if ( pthread_mutex_unlock( &_MutexManager ) ) {
    perror("MutexManager pthread_nriContainmutex_unlock ") ;
    exit( 0 ) ;
  }
  MESSAGE( "MutexManager pthread_mutex_unlocked" ) ;
  return Engines::Container::_duplicate( aContainer ) ;
}

Engines::Container_ptr Manager_i::FindOrStartContainerLocked( Containers::MachineParameters & myParams ,
                                                              const char * ComponentName ) {
  Engines::Container_var aContainer = Engines::Container::_nil() ;
  Containers::MachineParameters SearchParams = myParams ;
  if ( strcmp( myParams.HostName , "localhost" ) == 0 ) {
    myParams.HostName = GetHostname().c_str() ;
  }
  if ( myParams.ContainerType == Engines::Undefined ) {
    myParams.ContainerType = Engines::Cpp ;
  }
  if (  myParams.ContainerType == Engines::Cpp ||  myParams.ContainerType == Engines::Python ) {
    if ( ( strlen( myParams.HostName ) || strcmp( myParams.HostName ,"localhost" ) == 0 ) &&
         strlen( myParams.ContainerName ) ) {
      aContainer =  FindContainer( myParams ) ;
      if ( CORBA::is_nil( aContainer ) ) {
        aContainer = StartContainer( myParams ) ;
        if ( CORBA::is_nil( aContainer ) ) {
          MESSAGE( "Manager_i::FindOrStartContainer FindOrStartContainerLocked failed" ) ;
        }
        else {
          MESSAGE( "Manager_i::FindOrStartContainer FindOrStartContainerLocked " << aContainer->name()
                   << " successed" ) ;
	}
      }
      else {
        MESSAGE( "Manager_i::FindOrStartContainer FindOrStartContainerLocked " << aContainer->name()
                 << " successed" ) ;
      }
    }
    else {
      Resources::ListOfComputers_var aListOfComputers ;
      Resources::Computer_var aComputer ;
      if ( strlen( myParams.HostName ) == 0 || strcmp( myParams.HostName ,"localhost" ) == 0 ) {
        if ( strcmp( myParams.HostName ,"localhost" ) == 0 ) {
          myParams.HostName = CORBA::string_dup( _NamingServiceHostName.c_str() ) ;
        }
      }
// appel au ResourcesManager ---> liste de machines ...
      aListOfComputers = _ResourcesManager->GetComputers( myParams ) ;
      if ( aListOfComputers->length() == 0 ) {
        MESSAGE( "Manager_i::FindOrStartContainer NO computer found in ResourcesManager" ) ;
      }
      else if ( strlen( myParams.ContainerName ) ) {
// Boucle de recherche du container dans la liste de machines
        int i ;
        for ( i = 0 ; i < aListOfComputers->length() && CORBA::is_nil( aContainer ) ; i++ ) {
          SearchParams.HostName = aListOfComputers[ i ]->Alias() ;
          aContainer =  FindContainer( SearchParams ) ;
          if ( CORBA::is_nil( aContainer ) ) {
            MESSAGE( "Manager_i::FindOrStartContainer FindOrStartContainerLocked not found on "
                     << SearchParams.HostName ) ;
          }
          else {
            MESSAGE( "Manager_i::FindOrStartContainer FindOrStartContainerLocked " << aContainer->name()
                     << " found on " << SearchParams.HostName ) ;
  	  }
	}
        if ( CORBA::is_nil( aContainer ) ) {
          myParams.HostName = aListOfComputers[ 0 ]->Alias() ;
          aContainer = StartContainer( myParams ) ;
          if ( CORBA::is_nil( aContainer ) ) {
            MESSAGE( "Manager_i::FindOrStartContainer FindOrStartContainerLocked not started with "
                     << myParams.HostName ) ;
          }
          else {
            MESSAGE( "Manager_i::FindOrStartContainer FindOrStartContainerLocked started with "
                     << myParams.HostName << " for " << aContainer->name() ) ;
  	  }
        }
      }
      else {
// Rechercher dans tous les containers de toutes les machines le composant
        if ( strlen( ComponentName ) ) {

	}
        else {
          aComputer = _ResourcesManager->GetComputer( aListOfComputers ) ;
          if ( CORBA::is_nil( aComputer ) ) {
            MESSAGE( "Manager_i::FindOrStartContainer FindOrStartContainerLocked GetComputer failed" ) ;
	  }
          else {
            MESSAGE( "Manager_i::FindOrStartContainer FindOrStartContainerLocked GetComputer successed : "
                     << aComputer->Alias() ) ;
            myParams.HostName = aComputer->Alias() ;
	  }
	}
// Si on ne le trouve pas, prendre les par-defauts
        if ( CORBA::is_nil( aContainer ) ) {
          if ( myParams.ContainerType == Engines::Cpp ) {
            myParams.ContainerName = CORBA::string_dup( Containers::DefaultContainerCpp ) ;
          }
          else if ( myParams.ContainerType == Engines::Python ) {
            myParams.ContainerName = CORBA::string_dup( Containers::DefaultContainerPython ) ;
          }
          aContainer =  FindContainer( myParams ) ;
          if ( CORBA::is_nil( aContainer ) ) {
            aContainer = StartContainer( myParams ) ;
            if ( CORBA::is_nil( aContainer ) ) {
              MESSAGE( "Manager_i::FindOrStartContainer FindOrStartContainerLocked failed" ) ;
            }
            else {
              MESSAGE( "Manager_i::FindOrStartContainer FindOrStartContainerLocked " << aContainer->name()
                       << " successed" ) ;
    	    }
          }
          else {
            MESSAGE( "Manager_i::FindOrStartContainer FindOrStartContainerLocked " << aContainer->name()
                 << " successed" ) ;
	  }
        }
      }
    }
  }
  if ( !CORBA::is_nil( aContainer ) ) {
    MESSAGE( "Manager_i::FindOrStartContainer Container " << aContainer->name() << " is known ==> ping" ) ;
    aContainer->ping() ;
  }
  else {
    MESSAGE( "Manager_i::FindOrStartContainer Container " << _ContainerName << " unknown" ) ;
  }
  _EnginesContainer = aContainer ;
  return Engines::Container::_duplicate( aContainer ) ;
}

Engines::Container_ptr Manager_i::StartContainer( Containers::MachineParameters & myParams ) {
  Engines::Container_var aContainer = Engines::Container::_nil() ;
  Engines::Container_var aStartContainer = Engines::Container::_nil() ;
  string  theContainerName = (char * ) myParams.ContainerName ;
  myParams.ContainerName = "" ;
  aStartContainer = FindContainer( myParams ) ;
  if ( CORBA::is_nil( aStartContainer ) ) {
    MESSAGE( "Manager_i::StartContainer NO Container on " << myParams.HostName ) ;
    myParams.ContainerName = CORBA::string_dup( theContainerName.c_str() ) ;
    string rsh( "" ) ;
    char * HostName = myParams.HostName ;
    if ( strcmp( HostName , GetHostname().c_str() ) ) {
      if ( _ResourcesManager->SshAccess( HostName ) ) {
        rsh += "ssh " ;
      }
      else {
        rsh += "rsh -n " ;
      }
      rsh += myParams.HostName ;
      rsh += " sh -c \"'" ;
      Resources::ComputerEnvironment * aComputerEnvironment = _ResourcesManager->SearchComputer( myParams.HostName )->Environment() ;
      int size = aComputerEnvironment->Module_Root_Dir_Names.length() ;
      int i ;
      bool GeomModule = false ;
      for ( i = 0 ; i < size ; i++ ) {
        if ( i > 0 ) {
          rsh += " ; " ;
        }
        rsh += "export " ;
        rsh += aComputerEnvironment->Module_Root_Dir_Names[ i ] ;
        if ( strcmp( aComputerEnvironment->Module_Root_Dir_Names[ i ] , "GEOM_ROOT_DIR" ) == 0 ) {
          GeomModule = true ;
        }
        rsh += "=" ;
        rsh += aComputerEnvironment->Module_Root_Dir_Values[ i ] ;
      }
      if ( size > 0 ) {
        rsh += " ; " ;
      }
      rsh += "export PATH=" ;
      int j ;
      string Path = (char * ) aComputerEnvironment->Path ;
      for ( j = 0 ; j < Path.size() ; j++ ) {
        if ( Path[ j ] == '$' ) {
          rsh += "\\" ;
	}
        rsh += Path[ j ] ;
      }
      rsh += " ; export LD_LIBRARY_PATH=" ;
      string Ld_Library_Path = (char * ) aComputerEnvironment->Ld_Library_Path ;
      for ( j = 0 ; j < Ld_Library_Path.size() ; j++ ) {
        if ( Ld_Library_Path[ j ] == '$' ) {
          rsh += "\\" ;
	}
        rsh += Ld_Library_Path[ j ] ;
      }
      rsh += " ; export PYTHONPATH=" ;
      string PythonPath = (char * ) aComputerEnvironment->PythonPath ;
      for ( j = 0 ; j < PythonPath.size() ; j++ ) {
        if ( PythonPath[ j ] == '$' ) {
          rsh += "\\" ;
	}
        rsh += PythonPath[ j ] ;
      }
      rsh += " ; export CASROOT=" ;
      rsh += aComputerEnvironment->CasRoot ;
      rsh += " ; export CSF_PluginDefaults=" ;
      rsh += "\\" ;
      rsh += "${KERNEL_ROOT_DIR}/share/salome/resources" ;
      rsh += " ; export CSF_SALOMEDS_ResourcesDefaults=" ;
      rsh += "\\" ;
      rsh += "${KERNEL_ROOT_DIR}/share/salome/resources" ;
      if ( GeomModule ) {
        rsh += " ; export CSF_GEOMDS_ResourcesDefaults=" ;
        rsh += "\\" ;
        rsh += "${GEOM_ROOT_DIR}/share/salome/resources" ;
      }
      rsh += " ; " ;
    }
    if ( myParams.ContainerType == Engines::Cpp ) {
      rsh += "SALOME_Container " ;
    }
    else if ( myParams.ContainerType == Engines::Python ) {
      rsh += "SALOME_ContainerPy.py " ;
    }
    rsh += myParams.ContainerName ;
    rsh += " -ORBInitRef NameService=corbaname::" ;
    rsh += myParams.NsHostName ;
    if ( myParams.NsPort != 0 ) {
      rsh += ":" ;
      ostringstream astr ;
      astr << myParams.NsPort ;
      rsh += astr.str().c_str() ;
    }
    rsh += " > /tmp/" ;
    rsh += myParams.ContainerName ;
    if ( myParams.ContainerType == Engines::Cpp ) {
      rsh += "_Cpp_" ;
    }
    else if ( myParams.ContainerType == Engines::Python ) {
      rsh += "_Py_" ;
    }
    rsh += myParams.HostName ;
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
      while ( CORBA::is_nil( aContainer ) && count ) {
        sleep( 1 ) ;
        count-- ;
        if ( count != 21 ) {
          MESSAGE( "StartContainer" << count << ". Waiting for " << myParams.ContainerName << " on "
                   << myParams.HostName ) ;
	}
        aContainer = FindContainer( myParams ) ;
      }
      if ( CORBA::is_nil( aContainer ) ) {
        INFOS("StartContainer rsh/ssh failed " ) ;
      }
    }
  }
  else {
    MESSAGE( "Manager_i::StartContainer -> Container start_impl( " << aStartContainer->name() << ") for "
             << theContainerName.c_str() ) ;
    aContainer = aStartContainer->start_impl( theContainerName.c_str() , myParams.ContainerType ) ;
  }
  return Engines::Container::_duplicate( aContainer ) ;
}

Engines::Component_ptr Manager_i::FindComponent( const Containers::MachineParameters & MyParams ,
                                                 const char * ComponentName ) {
  CORBA::Object_var ComponentObject = CORBA::Object::_nil() ;
  Engines::Component_var EnginesComponent = Engines::Component::_nil() ;
  Engines::Container_var aContainer = FindContainer( MyParams ) ;
  if ( !CORBA::is_nil( aContainer ) ) {
    _ComponentName = _ContainerName ;
    _ComponentName += "/" ;
    _ComponentName += ComponentName ;
    try {
      ComponentObject = _NamingService->Resolve( _ComponentName.c_str() ) ;
      if ( !CORBA::is_nil( ComponentObject ) ) {
        MESSAGE( "Component found !" << _ComponentName ) ;
        Engines::Component_var EnginesComponent = Engines::Component::_narrow( ComponentObject ) ;
        if ( !CORBA::is_nil( EnginesComponent ) ) {
	  try {
	    EnginesComponent->ping() ; 
	  }
	  catch ( CORBA::COMM_FAILURE& ) {
	    INFOS("Caught CORBA::SystemException CommFailure. Engine " << _ComponentName << "does not respond" ) ;
            EnginesComponent = Engines::Component::_nil() ;
	  }
        }
      }
    }
    catch ( ServiceUnreachable& ) {
      INFOS("Caught exception: Naming Service Unreachable") ;
    }
    catch (...) {
      INFOS("Caught unknown exception.") ;
    }
  }
  return Engines::Component::_duplicate( EnginesComponent ) ;
}

Engines::Component_ptr Manager_i::FindOrLoad_ComponentPath( const Containers::MachineParameters & MyParams ,
                                                            const char * ComponentName ,
                                                            const char * ImplementationPath ) {
  BEGIN_OF("FindOrLoad_Component( const Containers::MachineParameters & MyParams , const char * ComponentName , const char * ImplementationPath 1)");
  MESSAGE( "MutexManager pthread_mutex_lock :" ) ;
  if ( pthread_mutex_lock( &_MutexManager ) ) {
    perror("MutexManager pthread_mutex_lock ") ;
    exit( 0 ) ;
  }
  MESSAGE( "MutexManager pthread_mutex_locked" ) ;
  Engines::Container_var aContainer = Engines::Container::_nil() ;
  Engines::Component_var EnginesComponent = FindComponent(  MyParams , ComponentName ) ;
  if ( CORBA::is_nil( EnginesComponent ) ) {
    aContainer = FindOrStartContainerLocked( (Containers::MachineParameters & ) MyParams , ComponentName ) ;
  }
  else {
    aContainer = _EnginesContainer ;
  }
  if ( pthread_mutex_unlock( &_MutexManager ) ) {
    perror("MutexManager pthread_mutex_unlock ") ;
    exit( 0 ) ;
  }
  MESSAGE( "MutexManager pthread_mutex_unlocked" ) ;
  if ( !CORBA::is_nil( aContainer ) && CORBA::is_nil( EnginesComponent ) ) {
    MESSAGE("Component not found ! trying to load " << _ComponentName ) ;
    EnginesComponent = aContainer->load_impl( ComponentName, ImplementationPath ) ;
    if ( !CORBA::is_nil( EnginesComponent ) ) {
      MESSAGE( "Component launched ! " << _ComponentName ) ;
      try {
	EnginesComponent->ping() ; 
      }
      catch ( CORBA::COMM_FAILURE& ) {
	INFOS("Caught CORBA::SystemException CommFailure. Engine " << _ComponentName << "does not respond" ) ;
        EnginesComponent = Engines::Component::_nil() ;
      }
    }
    else {
      MESSAGE( "Component NOT launched ! " << _ComponentName ) ;
    }
  }
  return Engines::Component::_duplicate( EnginesComponent ) ;
}

Engines::Component_ptr Manager_i::FindOrLoad_Component( const Containers::MachineParameters & MyParams ,
                                                        const char * ComponentName ) {
  BEGIN_OF("FindOrLoad_Component( const Containers::MachineParameters & MyParams , const char * ComponentName )");
  MESSAGE( "MutexManager pthread_mutex_lock :" ) ;
  if ( pthread_mutex_lock( &_MutexManager ) ) {
    perror("MutexManager pthread_mutex_lock ") ;
    exit( 0 ) ;
  }
  MESSAGE( "MutexManager pthread_mutex_locked" ) ;
  Engines::Container_var aContainer = Engines::Container::_nil() ;
  Engines::Component_var EnginesComponent = FindComponent(  MyParams , ComponentName ) ;
  if ( CORBA::is_nil( EnginesComponent ) ) {
    aContainer = FindOrStartContainerLocked( (Containers::MachineParameters & ) MyParams , ComponentName ) ;
  }
  else {
    aContainer = _EnginesContainer ;
  }
  if ( pthread_mutex_unlock( &_MutexManager ) ) {
    perror("MutexManager pthread_mutex_unlock ") ;
    exit( 0 ) ;
  }
  MESSAGE( "MutexManager pthread_mutex_unlocked" ) ;
  if ( !CORBA::is_nil( aContainer ) && CORBA::is_nil( EnginesComponent ) ) {
    MESSAGE("Component not found ! trying to load " << _ComponentName ) ;
    string ImplementationPath( "lib" ) ;
    ImplementationPath += ComponentName ;
    ImplementationPath += "Engine.so" ;
    EnginesComponent = aContainer->load_impl( ComponentName, ImplementationPath.c_str() ) ;
    if ( !CORBA::is_nil( EnginesComponent ) ) {
      MESSAGE( "Component launched ! " << _ComponentName ) ;
      try {
	EnginesComponent->ping() ; 
      }
      catch ( CORBA::COMM_FAILURE& ) {
	INFOS("Caught CORBA::SystemException CommFailure. Engine " << _ComponentName << "does not respond" ) ;
        EnginesComponent = Engines::Component::_nil() ;
      }
    }
    else {
      MESSAGE( "Component NOT launched ! " << _ComponentName ) ;
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

