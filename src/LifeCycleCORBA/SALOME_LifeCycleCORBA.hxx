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
//  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 
//
//
//
//  File   : SALOME_LifeCycleCORBA.hxx
//  Author : Paul RASCLE, EDF - MARC TAJCHMAN, CEA
//  Module : SALOME
//  $Header$

#ifndef _SALOME_LIFECYCLECORBA_HXX_
#define _SALOME_LIFECYCLECORBA_HXX_

#include <stdlib.h>
#include <unistd.h>
#include <string>

#include <SALOMEconfig.h>
#include CORBA_CLIENT_HEADER(SALOME_Component)

#include CORBA_CLIENT_HEADER( ContainersManager )

class SALOME_NamingService;

class SALOME_LifeCycleCORBA
{
public:
  SALOME_LifeCycleCORBA();
  SALOME_LifeCycleCORBA(SALOME_NamingService *ns);
  virtual ~SALOME_LifeCycleCORBA();

  Engines::Container_var FindContainer( const char *containerName ) ;
  Engines::Container_var FindContainer( Containers::MachineParameters & MyParams ) ;
  Engines::ListOfContainers_var FindContainers( Containers::MachineParameters & MyParams ) ;
  Engines::Component_var FindComponent( Containers::MachineParameters & MyParams ,
                                        const char * ComponentName ) ;
  Engines::ListOfComponents_var FindComponents( Containers::MachineParameters & MyParams ,
                                                const char * ComponentName ) ;
  Engines::Component_var FindOrLoad_Component( const char *containerName ,
					       const char *componentName ,
					       const char *implementationPath ) ;
  Engines::Component_var FindOrLoad_Component( Containers::MachineParameters & MyParams ,
					       const char *componentName ,
					       const char *implementationPath ) ;
  Engines::Component_var FindOrLoad_Component( const char *containerName ,
					       const char *componentName ) ;
  Engines::Component_var FindOrLoad_Component( Containers::MachineParameters & MyParams ,
					       const char *componentName ) ;
protected:
  Containers::Manager_var _MyContainersMgr ;
  SALOME_NamingService  * _NamingService ;
  Engines::Container_var  _FactoryServer ;

private:
  std::string ContainerName( const char * aComputerContainer ,
                             std::string * theComputer ,
                             std::string * theContainer ) ;
//  std::string ComputerPath( const char * theComputer ) ;
//  Engines::Container_var FindOrStartContainer(const std::string aComputerContainer ,
//                                              const std::string theComputer ,
//                                              const std::string theContainer ) ;

} ;

#endif
