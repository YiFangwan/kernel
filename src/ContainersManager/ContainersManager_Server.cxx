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
//  File   : ContainersManager_Server.cxx
//  Author : Jean Rahuel
//  Module : SALOME
//  $Header: 

#include <iostream.h>
#include "SALOME_NamingService.hxx"
#include "Utils_ORB_INIT.hxx"
#include "Utils_SINGLETON.hxx"

#include "ContainersManager_i.hxx"

using namespace std ;

int main( int argc , char **argv ) {
  try {

// initialize the ORB
#if WITH_SINGLETON
    ORB_INIT &init = *SINGLETON_<ORB_INIT>::Instance() ;
    ASSERT(SINGLETON_<ORB_INIT>::IsAlreadyExisting()) ;
    CORBA::ORB_var &orb = init( argc , argv ) ;
#else
    CORBA::ORB_var orb = CORBA::ORB_init(argc, argv) ;
#endif
 
    long TIMESleep = 250000000 ;
    int NumberOfTries = 40 ;
    int a ;
    int i ;
    int j ;
    timespec ts_req ;
    ts_req.tv_nsec=TIMESleep ;
    ts_req.tv_sec=0 ;
    timespec ts_rem ;
    ts_rem.tv_nsec=0 ;
    ts_rem.tv_sec=0 ;

// initialize POA
    CosNaming::NamingContext_var inc ;
    PortableServer::POA_var poa ;
    CORBA::Object_var theObj = CORBA::Object::_nil() ;
    CORBA::Object_var obj = CORBA::Object::_nil() ;
    CORBA::Object_var object = CORBA::Object::_nil() ;
#if WITH_SINGLETON
    SALOME_NamingService &naming = *SINGLETON_<SALOME_NamingService>::Instance() ;
#else
    theObj = orb->resolve_initial_references("NameService");
    inc = CosNaming::NamingContext::_narrow(theObj);
#endif
    int CONTAINERS_MANAGER = 0 ;
    const char * Env = getenv("USE_LOGGER") ;
    int EnvL =0;
    if ( ( Env != NULL ) && ( strlen(Env ) ) ) {
	EnvL=1 ;
    }
    CosNaming::Name name ;
    name.length( 1 ) ;
    name[0].id = CORBA::string_dup("Logger") ;    
    PortableServer::POAManager_var mgr ; 
    for ( i = 1 ; i <= NumberOfTries ; i++ ) {
      if ( i != 1 ) {
	a=nanosleep( &ts_req , &ts_rem ) ;
      }
      try { 
	obj = orb->resolve_initial_references( "RootPOA" ) ;
	if ( !CORBA::is_nil( obj ) ) {
	  poa = PortableServer::POA::_narrow( obj ) ;
	}
	if ( !CORBA::is_nil( poa ) ) {
	  mgr = poa->the_POAManager() ;
	}
	if ( !CORBA::is_nil( orb ) ) {
	  theObj = orb->resolve_initial_references( "NameService" ) ;
	}
	if ( !CORBA::is_nil( theObj ) ) {
	  inc = CosNaming::NamingContext::_narrow(theObj) ;
	}
      }
      catch ( CORBA::COMM_FAILURE& ) {
	MESSAGE( "ContainersManager : CORBA::COMM_FAILURE: Unable to contact the Naming Service" ) ;
      }
      if ( !CORBA::is_nil( inc ) ) {
	MESSAGE( "ContainersManager : Naming Service was found" ) ;
	if ( EnvL == 1 ) {
	  for ( j = 1 ; j <= NumberOfTries ; j++ ) {
	    if ( j !=1 ) {
	      a=nanosleep( &ts_req , &ts_rem ) ;
	    }
	    try {
	      object = inc->resolve( name ) ;
	    }
	    catch ( CosNaming::NamingContext::NotFound ) { 
	      MESSAGE( "ContainersManager : Logger Server wasn't found" ) ;
	    }
	    catch (...) { 
	      MESSAGE( "ContainersManager : Unknown exception" ) ;
	    }
	    if ( !CORBA::is_nil( object ) ) {
	      MESSAGE( "ContainersManager : Loger Server was found" ) ;
	      CONTAINERS_MANAGER = 1 ;
	      break;
	    }
	  }
	}
      }
      if ( ( CONTAINERS_MANAGER == 1 ) || ( (EnvL ==0 ) && ( !CORBA::is_nil( inc ) ) ) ) {
        break;
      }
    }

// initialise Naming Service
    SALOME_NamingService *_NS ;
    _NS = new SALOME_NamingService( orb ) ;

// Active ContainersManager
#if WITH_SINGLETON
    Manager_i * MyContainersManager = SINGLETON_<Manager_i>::Instance() ;
    cout << "ContainersManager_Server -> MyContainersManager->Init" << endl ;
    MyContainersManager->Init( orb , poa , _NS , argc , argv ) ;
#else
    Manager_i * MyContainersManager = new Manager_i() ;
    MyContainersManager->Init( orb , poa , _NS , argc , argv ) ;
#endif
//    _Id = _Poa->activate_object ( MyContainersManager ) ;
    cout << "ContainersManager_Server -> mgr->activate" << endl ;
    mgr->activate() ;
    CORBA::Object_ptr TheContainersManager = MyContainersManager->_this() ;

// register ContainersManager in Naming Service
    _NS->Register( TheContainersManager , "/Kernel/ContainersManager" ) ;

    MESSAGE("Running ContainersManager Server.") ;

    orb->run();
 
    poa->destroy( 1 , 1 ) ;
 
    orb->destroy();

  }
  catch ( CORBA::SystemException& ) {
    INFOS("Caught CORBA::SystemException.") ;
  }
  catch ( CORBA::Exception& ) {
    INFOS("Caught CORBA::Exception.") ;
  }

  return 0 ;
}
