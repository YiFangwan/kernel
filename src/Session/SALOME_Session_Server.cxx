//  SALOME Session : implementation of Session.idl
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
//  File   : SALOME_Session_Server.cxx
//  Author : Paul RASCLE, EDF
//  Module : SALOME
//  $Header$

# include "Utils_ORB_INIT.hxx"
# include "Utils_SINGLETON.hxx"
#include "SALOME_Container_i.hxx"
# include "SALOME_NamingService.hxx"
#include <iostream>
#include <unistd.h>
using namespace std;

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(SALOME_Session)

#include "utilities.h"

#include "SALOME_Session_i.hxx"

#include <Python.h>
static PyMethodDef MethodPyVoidMethod[] = {
  { NULL,        NULL }
};


//! CORBA server for SALOME Session
/*!
 *  SALOME_Session Server launches a SALOME session servant.
 *  The servant registers to the Naming Service.
 *  See SALOME_Session.idl for interface specification.
 *
 *  Main services offered by the servant are:
 *   - launch GUI
 *   - stop Session (must be idle)
 *   - get session state
 */

int main(int argc, char **argv)
{

  Py_Initialize() ;
  PySys_SetArgv( argc , argv ) ;
  Py_InitModule( "InitPyRunMethod" , MethodPyVoidMethod ) ;

  try
    {
    // Initialise the ORB.
    ORB_INIT &init = *SINGLETON_<ORB_INIT>::Instance() ;
    ASSERT(SINGLETON_<ORB_INIT>::IsAlreadyExisting()) ;
    CORBA::ORB_var &orb = init( argc , argv ) ;
    ASSERT(!CORBA::is_nil(orb));

    // Obtain a reference to the root POA.
    CORBA::Object_var obj =orb->resolve_initial_references("RootPOA") ;
    PortableServer::POA_var root_poa = PortableServer::POA::_narrow(obj) ;
    ASSERT(!CORBA::is_nil(root_poa));

    // -------------------------------------------------------------------------------
    // ---- naming service & Logger

    CosNaming::NamingContext_var myNS;
    SALOME_Logger::Logger_var log;
    SALOME_NamingService &naming = *SINGLETON_<SALOME_NamingService>::Instance() ;

    {   
      int SESSION=0;
      const char * Env = getenv("USE_LOGGER"); 
      int EnvL =0;
      if ((Env!=NULL) && (strlen(Env)))
	EnvL=1;
    
      CosNaming::Name name;
      name.length(1);
      name[0].id=CORBA::string_dup("Logger");    
      
      long TIMESleep = 250000000;
      int NumberOfTries = 40;
      timespec ts_req;
      ts_req.tv_nsec=TIMESleep;
      ts_req.tv_sec=0;
      timespec ts_rem;
      ts_rem.tv_nsec=0;
      ts_rem.tv_sec=0;
    
      for (int i = 1; i<=NumberOfTries; i++)
	{
	  if (i!=1) 
	    int a=nanosleep(&ts_req,&ts_rem);
	  try
	    { 
	      CORBA::Object_var theObj = orb->resolve_initial_references("NameService");
	      if (!CORBA::is_nil(theObj))
		myNS = CosNaming::NamingContext::_narrow(theObj);
	    }
	  catch( CORBA::COMM_FAILURE& )
	    {
	      MESSAGE( "Session Server: CORBA::COMM_FAILURE: Unable to contact the Naming Service" );
	    }
	  if(!CORBA::is_nil(myNS))
	    {
	      MESSAGE( "Session Server: Naming Service was found" );
	      if(EnvL==1) // use Logger
		{
		  for(int j=1; j<=NumberOfTries; j++)
		    {
		      if (j!=1) 
			int a=nanosleep(&ts_req, &ts_rem);
		      try
			{
			  CORBA::Object_var object = myNS->resolve(name); // find Logger
			  if (!CORBA::is_nil(object))
			    log = SALOME_Logger::Logger::_narrow(object);
			  if (!CORBA::is_nil(log))
			    {
			      MESSAGE( "Session Server: Logger Server was found" );
			      log->ping();
			      SESSION=1;
			      break;
			    }
			}
		      catch(CosNaming::NamingContext::NotFound)
			{ MESSAGE( "Session Server: Logger Server wasn't found" ); }
		      catch(...)
			{ MESSAGE( "Session Server: Unknown exception" ); }
		    }
		}
	    }
	  if ((SESSION==1)||((EnvL==0)&&(!CORBA::is_nil(myNS)))) // Naming service found, & Logger if needed
	    break;
	}
    }
   
    // -------------------------------------------------------------------------------
    // ---- container

    // define policy objects     
    PortableServer::ImplicitActivationPolicy_var implicitActivation =
      root_poa->create_implicit_activation_policy(PortableServer::NO_IMPLICIT_ACTIVATION) ;

      // default = NO_IMPLICIT_ACTIVATION
    PortableServer::ThreadPolicy_var threadPolicy =
      root_poa->create_thread_policy(PortableServer::ORB_CTRL_MODEL) ;
      // default = ORB_CTRL_MODEL, other choice SINGLE_THREAD_MODEL

    // create policy list
    CORBA::PolicyList policyList;
    policyList.length(2);
    policyList[0] = PortableServer::ImplicitActivationPolicy::_duplicate(implicitActivation) ;
    policyList[1] = PortableServer::ThreadPolicy::_duplicate(threadPolicy) ;

    // create the child POA
    PortableServer::POAManager_var nil_mgr = PortableServer::POAManager::_nil() ;
    // PortableServer::POA_var factory_poa =
    //   root_poa->create_POA("factory_poa", pman, policyList) ;
    PortableServer::POA_var factory_poa =
      root_poa->create_POA("factory_poa", nil_mgr, policyList) ;
    //with nil_mgr instead of pman, a new POA manager is created with the new POA
    
    // destroy policy objects
    implicitActivation->destroy() ;
    threadPolicy->destroy() ;

    // servant : container

    char *containerName = "";
    if (argc >1) 
    {
	containerName = argv[1] ;
    }

    Engines_Container_i * myContainer 
     = new Engines_Container_i(orb, factory_poa, containerName , argc , argv );

    //PortableServer::ObjectId_var mySALOME_Containerid = factory_poa->activate_object(myContainer) ;
    //MESSAGE("poa->activate_object(myContainer)");

    // obtain the factory poa manager
    PortableServer::POAManager_var pmanfac = factory_poa->the_POAManager();
    pmanfac->activate() ;
    MESSAGE("pmanfac->activate()")

    // -------------------------------------------------------------------------------
    // servant : session

    SALOME_Session_i * mySALOME_Session = new SALOME_Session_i(argc, argv, orb, root_poa) ;
    PortableServer::ObjectId_var mySALOME_Sessionid = root_poa->activate_object(mySALOME_Session) ;
    MESSAGE("poa->activate_object(mySALOME_Session)");

    obj = mySALOME_Session->_this() ;
    CORBA::String_var sior(orb->object_to_string(obj)) ;
    mySALOME_Session->NSregister();
    mySALOME_Session->_remove_ref() ;

    // obtain the root poa manager
    PortableServer::POAManager_var pman = root_poa->the_POAManager();
    pman->activate() ;
    MESSAGE("pman->activate()")

    orb->run() ;
    orb->destroy() ;
    }
  catch (CORBA::SystemException&)
    {
      INFOS("Caught CORBA::SystemException.");
    }
  catch(PortableServer::POA::WrongPolicy&)
    {
      INFOS("Caught CORBA::WrongPolicyException.");
    }
  catch(PortableServer::POA::ServantAlreadyActive&)
    {
      INFOS("Caught CORBA::ServantAlreadyActiveException");
    }
  catch (CORBA::Exception&)
    {
      INFOS("Caught CORBA::Exception.");
    }
    catch (...)
    {
      INFOS("Caught unknown exception.");
    }
  return 0 ;
}
