//  SALOME LifeCycleCORBA : implementation of containers and engines life cycle both in Python and C++
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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//
//
//  File   : TestLifeCycleCORBA.cxx
//  Author : Paul RASCLE, EDF - MARC TAJCHMAN, CEA
//  Module : SALOME
//  $Header$

#include <iostream>
#ifndef WNT
#include <unistd.h>
#endif
#include <SALOMEconfig.h>
#ifndef WNT
#include CORBA_CLIENT_HEADER(SALOME_Component)
#include CORBA_CLIENT_HEADER(SALOME_TestComponent)
#else
#include <SALOME_Component.hh>
#include <SALOME_TestComponent.hh>
#endif
#include "SALOME_NamingService.hxx"
#include "SALOME_LifeCycleCORBA.hxx"
#include "SALOME_FileTransferCORBA.hxx"
#include "utilities.h"
#include <OpUtil.hxx>

using namespace std;

int main (int argc, char * argv[])
{

  try
    {
      // --- Initialize omniORB

      CORBA::ORB_var orb = CORBA::ORB_init(argc, argv);
    
      // --- Obtain a reference to the root POA

      CORBA::Object_var obj = orb->resolve_initial_references("RootPOA") ;
      PortableServer::POA_var poa = PortableServer::POA::_narrow(obj) ;

      // --- Naming Service and LifeCycleCORBA interfaces
    
      SALOME_NamingService _NS(orb) ;
      SALOME_LifeCycleCORBA _LCC(&_NS) ;

      // --- get a local container,
      //     load an engine, and invoque methods on that engine

      string containerName = "myServer";
      MESSAGE("FindOrLoadComponent " + containerName + "/" + "SalomeTestComponent" );

      Engines::Component_var mycompo =
	_LCC.FindOrLoad_Component(containerName.c_str(),"SalomeTestComponent");
      ASSERT(!CORBA::is_nil(mycompo));
      Engines::TestComponent_var m1;
      m1 = Engines::TestComponent::_narrow(mycompo);
      ASSERT(!CORBA::is_nil(m1));
      SCRUTE(m1->instanceName());
      MESSAGE("Coucou " << m1->Coucou(1L));

      // --- get another container,
      //     load an engine, and invoque methods on that engine

      string containerName2 = "otherServer";

      Engines::Component_var mycompo2 =
	_LCC.FindOrLoad_Component(containerName2.c_str(),"SALOME_TestComponentPy");
      ASSERT(!CORBA::is_nil(mycompo2));
      Engines::TestComponent_var m2;
      m2 = Engines::TestComponent::_narrow(mycompo2);
      ASSERT(!CORBA::is_nil(m2));
      SCRUTE(m2->instanceName());
      cout << m2->instanceName() << endl;
      MESSAGE("Coucou " << m2->Coucou(1L));

      // --- get a third container,
      //     load an engine, and invoque methods on that engine

      Engines::Component_var mycompo3 =
	_LCC.FindOrLoad_Component("totoPy","SALOME_TestComponentPy");
      ASSERT(!CORBA::is_nil(mycompo3));
      Engines::TestComponent_var m3 = Engines::TestComponent::_narrow(mycompo3);
      ASSERT(!CORBA::is_nil(m3));
      cout << m3->instanceName() << endl;

      // --- yet another container, with hostname,
      //     load an engine, and invoque methods on that engine

      string containerName4 = GetHostname();
      containerName4  += "/titiPy";
      Engines::Component_var mycompo4 = 
	_LCC.FindOrLoad_Component(containerName4.c_str(),"SALOME_TestComponentPy");
      ASSERT(!CORBA::is_nil(mycompo4));
      Engines::TestComponent_var m4 = Engines::TestComponent::_narrow(mycompo4);
      ASSERT(!CORBA::is_nil(m4));
      cout << m4->instanceName() << endl;

      // --- try a local file transfer

      string origFileName = "/home/prascle/petitfichier";
      SALOME_FileTransferCORBA transfer( GetHostname(),
					 origFileName);
      string local = transfer.getLocalFile();
      SCRUTE(local);

      // --- try a file transfer from another computer

      origFileName = "/home/prascle/occ60.tgz";
      SALOME_FileTransferCORBA transfer2( "cli76ce",
					 origFileName);
      local = transfer2.getLocalFile();
      SCRUTE(local);
      local = transfer2.getLocalFile();
      SCRUTE(local);

    }
  catch(CORBA::SystemException& ex)
    {
      INFOS("Caught system exception COMM_FAILURE -- unable to contact the object.");
    }
#ifndef WNT
  catch(CORBA::SystemException&)
    {
      INFOS("Caught a CORBA::SystemException.");
    }
#endif
  catch(CORBA::Exception&)
    {
      INFOS("Caught CORBA::Exception.");
    }
  catch(...)
    {
      INFOS("Caught unknown exception.");
    }

  return 0;
}

