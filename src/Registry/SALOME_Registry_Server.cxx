//  SALOME Registry : Registry server implementation
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
//  File   : SALOME_Registry_Server.cxx
//  Author : Pascale NOYRET - Antoine YESSAYAN, EDF
//  Module : SALOME
//  $Header$

#include <stdlib.h>
#include <iostream.h>
#include <fstream.h>

extern "C"
{
# include <stdio.h>
}

#include "utilities.h"
#include "LocalTraceCollector.hxx"
#include "Utils_ORB_INIT.hxx"
#include "Utils_SINGLETON.hxx"
#include "Utils_SALOME_Exception.hxx"
#include "Utils_CommException.hxx"
#include "ServiceUnreachable.hxx"
#include "SALOME_NamingService.hxx"
#include "RegistryService.hxx"

#ifdef CHECKTIME
#include <Utils_Timer.hxx>
#endif
using namespace std;

int main( int argc , char **argv )
{
  LocalTraceCollector *myThreadTrace = LocalTraceCollector::instance();
  BEGIN_OF( argv[0] )
    INFOS_COMPILATION 
    SCRUTE(argc) 
    if( argc<3 )
      {
	MESSAGE("you must provide the Salome session name when you call SALOME_Registry_Server") ;
	throw CommException("you must provide the Salome session name when you call SALOME_Registry_Server") ;
      }
  const char *ptrSessionName=0 ;

  int k=0 ;
  for ( k=1 ; k<argc ; k++ )
    {
      if( strcmp(argv[k],"--salome_session")==0 )
	{
	  ptrSessionName=argv[k+1] ;
	  break ;
	}
    }
  ASSERT(ptrSessionName) ;
  ASSERT(strlen( ptrSessionName )>0) ;
  const char *registryName = "Registry" ;
  ORB_INIT &init = *SINGLETON_<ORB_INIT>::Instance() ;
  CORBA::ORB_var &orb = init( argc , argv ) ;
  //
  long TIMESleep = 250000000;
  int NumberOfTries = 40;
  int a;
  timespec ts_req;
  ts_req.tv_nsec=TIMESleep;
  ts_req.tv_sec=0;
  timespec ts_rem;
  ts_rem.tv_nsec=0;
  ts_rem.tv_sec=0;
  CosNaming::NamingContext_var inc;
  PortableServer::POA_var poa;
  CORBA::Object_var theObj;
  CORBA::Object_var obj;
  CORBA::Object_var object;
  SALOME_NamingService &naming = *SINGLETON_<SALOME_NamingService>::Instance() ;
  Registry::Components_var varComponents;
  int REGISTRY=0;
  const char * Env = getenv("USE_LOGGER");
  int EnvL =0;
  if ((Env!=NULL) && (strlen(Env)))
    EnvL=1;
  CosNaming::Name name;
  name.length(1);
  name[0].id=CORBA::string_dup("Logger");  
  PortableServer::POAManager_var manager; 
  for (int i = 1; i<=NumberOfTries; i++)
    {
      if (i!=1) 
	a=nanosleep(&ts_req,&ts_rem);
      try
	{ 
	  obj = orb->resolve_initial_references("RootPOA");
	  if(!CORBA::is_nil(obj))
	    poa = PortableServer::POA::_narrow(obj);
	  if(!CORBA::is_nil(poa))
	    manager = poa->the_POAManager();
	  if(!CORBA::is_nil(orb)) 
	    theObj = orb->resolve_initial_references("NameService");
	  if (!CORBA::is_nil(theObj))
	    inc = CosNaming::NamingContext::_narrow(theObj);
	}
      catch( CORBA::COMM_FAILURE& )
	{
	  MESSAGE( "Registry Server: CORBA::COMM_FAILURE: Unable to contact the Naming Service" );
	}
      if(!CORBA::is_nil(inc))
	{
	  MESSAGE( "Registry Server: Naming Service was found" );
	  if(EnvL==1)
	    {
	      for(int j=1; j<=NumberOfTries; j++)
		{
		  if (j!=1) 
		    a=nanosleep(&ts_req, &ts_rem);
		  try
		    {
		      object = inc->resolve(name);
		    }
		  catch(CosNaming::NamingContext::NotFound)
		    {
		      MESSAGE( "Registry Server: Logger Server wasn't found" );
		    }
		  catch(...)
		    {
		      MESSAGE( "Registry Server: Unknown exception" );
		    }
		  if (!CORBA::is_nil(object))
		    {
		      MESSAGE( "Module Catalog Server: Logger Server was found" );
		      REGISTRY=1;
		      break;
		    }
		}
	    }
	}
      if ((REGISTRY==1)||((EnvL==0)&&(!CORBA::is_nil(inc))))
	break;
    }

  try
    {
      naming.init_orb( orb ) ;
      RegistryService *ptrRegistry = SINGLETON_<RegistryService>::Instance() ;
      ptrRegistry->SessionName( ptrSessionName ) ;
      varComponents = ptrRegistry->_this() ;
      // The RegistryService must not already exist.
	    
      try
	{
	  CORBA::Object_var pipo = naming.Resolve( registryName ) ;
	  if (CORBA::is_nil(pipo) )  throw ServiceUnreachable() ;
	  MESSAGE("RegistryService servant already existing" ) ;
	  exit( EXIT_FAILURE ) ;
	}
      catch( const ServiceUnreachable &ex )
	{
	}
      catch( const CORBA::Exception &exx )
	{
	}
      string absoluteName = string("/") + registryName;
      naming.Register( varComponents , absoluteName.c_str() ) ;
      MESSAGE("On attend les requetes des clients") ;
      try
	{
	  // Activation du POA
	  MESSAGE("Activation du POA") ;
	  manager->activate() ;
		
	  // Lancement de l'ORB
	  MESSAGE("Lancement de l'ORB") ;
#ifdef CHECKTIME
	  Utils_Timer timer;
	  timer.Start();
	  timer.Stop();
	  MESSAGE("SALOME_Registry_Server.cxx - orb->run()");
	  timer.ShowAbsolute();
#endif
	  orb->run() ;
	}
      catch( const CORBA::Exception &ex )
	{
	  MESSAGE("Erreur systeme") ;
	  return EXIT_FAILURE ;
	}
	    
    }
  catch( const SALOME_Exception &ex )
    {
      MESSAGE( "Communication Error : " << ex.what() )
	return EXIT_FAILURE ;
    }
	
  END_OF( argv[0] ) ;
  delete myThreadTrace;
  return 0 ;
}
