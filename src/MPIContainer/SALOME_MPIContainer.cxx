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
//  File   : SALOME_MPIContainer.cxx
//  Module : SALOME

using namespace std;
#include <iostream>
#include "MPIContainer_i.hxx"
#include "utilities.h"
#include "LocalTraceCollector.hxx"
#include <mpi.h>

int main(int argc, char* argv[])
{
  LocalTraceCollector *myThreadTrace = LocalTraceCollector::instance();
  int nbproc, numproc;
  MPIContainer_i * myContainer;

  BEGIN_OF(argv[0])
  try {
    
    MESSAGE("Connection MPI");

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&nbproc);
    MPI_Comm_rank(MPI_COMM_WORLD,&numproc);

    MESSAGE("Initialisation CORBA");
    // Initialise the ORB.
    CORBA::ORB_var orb = CORBA::ORB_init(argc, argv);
 
    // Obtain a reference to the root POA.
    CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
    PortableServer::POA_var root_poa = PortableServer::POA::_narrow(obj);

    // obtain the root poa manager
    PortableServer::POAManager_var pman = root_poa->the_POAManager();

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
    PortableServer::POA_var factory_poa =
      root_poa->create_POA("factory_poa", pman, policyList) ;
      //with nil_mgr instead of pman, a new POA manager is created with the new POA
    
    // destroy policy objects
    implicitActivation->destroy() ;
    threadPolicy->destroy() ;

    char *containerName = "";
    if (argc >1) 
    {
	containerName = argv[1] ;
    }

    MESSAGE("Chargement container sur proc: " << numproc);
    myContainer = new MPIContainer_i(nbproc,numproc,orb,factory_poa, containerName);
    MESSAGE("Fin chargement container");

    pman->activate();

    orb->run();
    orb->destroy();

    delete myContainer;
    MPI_Finalize();
  }
  catch(CORBA::SystemException&) {
    INFOS("Caught CORBA::SystemException.")
  }
  catch(PortableServer::POA::WrongPolicy&)
  {
    INFOS("Caught CORBA::WrongPolicyException.")
  }
  catch(PortableServer::POA::ServantAlreadyActive&)
  {
    INFOS("Caught CORBA::ServantAlreadyActiveException")
  }
  catch(CORBA::Exception&) {
    INFOS("Caught CORBA::Exception.")
  }
  catch(...) {
    INFOS("Caught unknown exception.")
   }
  END_OF(argv[0]);
  delete myThreadTrace;
  return 0;
}

