//  SALOME ResourcesManager_Impl
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
//  File   : ResourcesManager_Impl.hxx
//  Author : Jean Rahuel
//  Module : SALOME
//  $Header: 

#ifndef RESOURCESMANAGER_IMPL_HXX
#define RESOURCESMANAGER_IMPL_HXX

#include <vector.h>
#include <map.h>

#include <ServiceUnreachable.hxx>

#include "SALOME_NamingService.hxx"

#include "utilities.h"
#include "ResourcesManager_Handler.hxx"

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER( ResourcesManager )
#include CORBA_SERVER_HEADER( ContainersManager )

class Manager_Impl : public POA_Resources::Manager ,
	             public PortableServer::RefCountServantBase {

  private:

    CORBA::ORB_ptr                 _Orb ;
    PortableServer::POA_ptr        _Poa ;
    SALOME_NamingService         * _NamingService ;
    PortableServer::ObjectId     * _Id  ;

    char                         * _CatalogPath ;

    vector< Computer_Impl * >      _ComputersVector ;
    Resources::ListOfComputers_var _ComputersList ;
    map< string , int >            _MapOfComputers ;

//    virtual Resources::ListOfComputers_var ParseXmlFile( const char * xmlfile ) ;
    virtual void ParseXmlFile( const char * xmlfile ) ;

    virtual bool ParseArguments( int argc , char ** argv , char ** path ) ;

  public:

    Manager_Impl( CORBA::ORB_ptr orb ,
		  PortableServer::POA_ptr poa ,
                  SALOME_NamingService * NamingService ,
                  int argc ,
                  char** argv ) ;

    virtual ~Manager_Impl();

    virtual long ping() ;

    virtual Resources::Computer_ptr TestComputer() ;

    virtual long SshAccess( const char * aComputerName ) ;

    virtual Resources::ListOfComputers * AllComputers() ;

    virtual Resources::ListOfComputers * GetComputers( const Containers::MachineParameters & aMachineParameters ) ;

    virtual Resources::Computer_ptr SelectComputer( const Containers::MachineParameters & aMachineParameters ) ;

    virtual Resources::Computer_ptr GetComputer( const Resources::ListOfComputers & aListOfComputers ) ;

    virtual Resources::Computer_ptr SearchComputer( const char * aComputerName ) ;

} ;

#endif
