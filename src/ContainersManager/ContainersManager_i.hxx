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
//  File   : ContainersManager_i.hxx
//  Author : Jean Rahuel
//  Module : SALOME
//  $Header:

#ifndef __CONTAINERSMANAGER_I_HXX__
#define __CONTAINERSMANAGER_I_HXX__

// std C++ headers
#include <sstream>
#include <iostream.h>
#include <fstream>
#include <stdlib.h>
#include <unistd.h>
#include <string>

#include "utilities.h"
#include "OpUtil.hxx"

#include <ServiceUnreachable.hxx>

#include "SALOME_NamingService.hxx"

// IDL headers
#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(ContainersManager)
#include CORBA_SERVER_HEADER(ResourcesManager)
#include CORBA_CLIENT_HEADER(SALOME_Component)



class Manager_i : public POA_Containers::Manager ,
		  public PortableServer::RefCountServantBase {
  private:

    CORBA::ORB_ptr                    _Orb ;
    SALOME_NamingService            * _NamingService ;
    Resources::Manager_var            _ResourcesManager ;
    string                            _NamingServiceHostName ;
    long                              _NamingServicePort ;
    pthread_mutex_t                   _MutexManager ;
    bool                              _StartContainer ;
    string                            _HostName ;
    string                            _FullHostName ;
    string                            _ContainerName ;
    string                            _FullContainerName ;
    Engines::ContainerType            _ContainerType ;
    string                            _ComponentName ;
    string                            _FullComponentName ;
    Resources::ListOfComputers_var    _ListOfComputers ;
    Resources::Computer_var           _ResourcesComputer ;
    Resources::ComputerParameters_var _ComputerParameters ;
    Engines::ListOfContainers_var     _ListOfContainers ;
    CORBA::Object_var                 _ContainerObject ;
    Engines::Container_var            _EnginesContainer ;
    Engines::ListOfComponents_var     _ListOfComponents ;
    CORBA::Object_var                 _ComponentObject ;
    Engines::Component_var            _EnginesComponent ;

    Engines::Container_ptr StartContainer( const Containers::MachineParameters & myParams ) ;

    Engines::Container_ptr FindOrStartContainerLocked( Containers::MachineParameters & MyParams ,
                                                       const char * ComponentName ) ;

  public:

    Manager_i() ;

    Manager_i( CORBA::ORB_ptr orb ,
               SALOME_NamingService * NamingService ,
               int argc ,
               char** argv ) ;

    virtual ~Manager_i() ;

    virtual bool ping() ;

    virtual Containers::MachineParameters * Parameters() ;

    virtual Engines::Container_ptr FindContainer( const Containers::MachineParameters & MyParams ) ;

    virtual Engines::ListOfContainers * FindContainers( const Containers::MachineParameters & MyParams ) ;

    virtual Engines::Container_ptr FindOrStartContainer( const Containers::MachineParameters & MyParams ) ;

    virtual Engines::Component_ptr FindComponent( const Containers::MachineParameters & MyParams ,
                                                  const char * ComponentName ) ;

    virtual Engines::ListOfComponents * FindComponents( const Containers::MachineParameters & MyParams ,
                                                        const char * ComponentName ) ;

    virtual Engines::Component_ptr FindOrLoad_ComponentPath( const Containers::MachineParameters & MyParams ,
                                                             const char * ComponentName ,
                                                             const char * ImplementationPath ) ;

    virtual Engines::Component_ptr FindOrLoad_Component( const Containers::MachineParameters & MyParams ,
                                                         const char * ComponentName ) ;

} ;

#endif
