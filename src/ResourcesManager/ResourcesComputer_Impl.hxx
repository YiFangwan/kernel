//  SALOME ResourcesComputer_Impl
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
//  File   : ResourcesComputer_Impl.hxx
//  Author : Jean Rahuel
//  Module : SALOME
//  $Header: 

#ifndef RESOURCESCOMPUTER_IMPL_HXX
#define RESOURCESCOMPUTER_IMPL_HXX

#include "utilities.h"
#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER( ResourcesManager )

class Computer_Impl : public POA_Resources::Computer ,
		      public PortableServer::RefCountServantBase {
  private:

    CORBA::ORB_ptr             _Orb ;
    PortableServer::POA_ptr    _Poa ;
    PortableServer::ObjectId * _Id  ;

    Resources::ComputerParameters  * _ComputerParameters ;
    Resources::ComputerEnvironment * _ComputerEnvironment ;

  public:

    Computer_Impl() ;

    Computer_Impl( CORBA::ORB_ptr orb ,
		   PortableServer::POA_ptr poa ,
                   Resources::ComputerParameters  * aComputerParameters ,
                   Resources::ComputerEnvironment * aComputerEnvironment ) ;

    ~Computer_Impl() ;

    PortableServer::ObjectId * getId() ;

    virtual bool ping() ;

    virtual char * FullName() ;

    virtual char * Alias() ;

    virtual bool IsAlive() ;

    virtual bool SshAccess() ;

    virtual bool RshAccess() ;

    virtual bool Interactive() ;

    virtual bool Batch() ;

    virtual char * UserName() ;

    virtual Containers::System Os() ;

    virtual CORBA::Long Memory() ;

    virtual CORBA::Long Swap() ;

    virtual CORBA::Long CpuClock() ;

    virtual CORBA::Long NbProc() ;

    virtual CORBA::Long NbNode() ;

    virtual Resources::ComputerParameters * Parameters() ;

    virtual Resources::ComputerEnvironment * Environment() ;

} ;

#endif
