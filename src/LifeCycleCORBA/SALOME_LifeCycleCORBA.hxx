// Copyright (C) 2007-2023  CEA, EDF, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

//  File   : SALOME_LifeCycleCORBA.hxx
//  Author : Paul RASCLE, EDF - MARC TAJCHMAN, CEA
//  Module : SALOME

#ifndef _SALOME_LIFECYCLECORBA_HXX_
#define _SALOME_LIFECYCLECORBA_HXX_

#include <stdlib.h>
#ifndef WIN32
#include <unistd.h>
#endif
#include <string>

#include <SALOMEconfig.h>
#include <Utils_SALOME_Exception.hxx>
#include CORBA_CLIENT_HEADER(SALOME_ContainerManager)
#include CORBA_CLIENT_HEADER(SALOME_Component)
#include <iostream>

#ifdef WIN32
# if defined LIFECYCLECORBA_EXPORTS || defined SalomeLifeCycleCORBA_EXPORTS
#  define LIFECYCLECORBA_EXPORT __declspec( dllexport )
# else
#  define LIFECYCLECORBA_EXPORT __declspec( dllimport )
# endif
#else
# define LIFECYCLECORBA_EXPORT
#endif


class SALOME_NamingService_Abstract;

class LIFECYCLECORBA_EXPORT IncompatibleComponent : public SALOME_Exception
{
public :
  IncompatibleComponent(void);
  IncompatibleComponent(const IncompatibleComponent &ex);
};

class LIFECYCLECORBA_EXPORT SALOME_LifeCycleCORBA
{
public:
  SALOME_LifeCycleCORBA(SALOME_NamingService_Abstract *ns = 0);
  virtual ~SALOME_LifeCycleCORBA();

  Engines::EngineComponent_ptr 
  FindComponent(const Engines::ContainerParameters& params,
                const char *componentName);

  Engines::EngineComponent_ptr
  LoadComponent(const Engines::ContainerParameters& params,
                const char *componentName);

  Engines::EngineComponent_ptr 
  FindOrLoad_Component(const Engines::ContainerParameters& params,
                       const char *componentName);

  Engines::EngineComponent_ptr
  FindOrLoad_Component(const char *containerName,
                       const char *componentName);
  
  // Parallel extension
  Engines::EngineComponent_ptr 
    Load_ParallelComponent(const Engines::ContainerParameters& params,
                           const char *componentName);

  bool isKnownComponentClass(const char *componentName);

  int NbProc(const Engines::ContainerParameters& params);

  static void preSet(Engines::ResourceParametersContainer& outparams);
  static void preSet(Engines::ContainerParameters& outparams);

  Engines::ContainerManager_ptr getContainerManager();
  Engines::ResourcesManager_ptr getResourcesManager();
  SALOME_NamingService_Abstract * namingService();
  CORBA::ORB_ptr orb();
  void copyFile(const char* hostSrc, const char* fileSrc, const char* hostDest, const char* fileDest);

  void shutdownServers(bool shutdownLauncher=true);
  static void killOmniNames();

protected:

  /*! Establish if a component called "componentName" in a container called
   *  "containerName"
   *  exists among the list of resources in "listOfMachines".
   *  This method uses Naming Service to find the component.
   */
  Engines::EngineComponent_ptr 
  _FindComponent(const Engines::ContainerParameters& params,
                 const char *componentName,
                 const Engines::ResourceList& listOfResources);

  Engines::EngineComponent_ptr
  _LoadComponent(const Engines::ContainerParameters& params,
                 const char *componentName);

  SALOME_NamingService_Abstract *_NS;
  SALOME_NamingService_Abstract *_NSnew;
  Engines::ContainerManager_var _ContManager;
  Engines::ResourcesManager_var _ResManager;
  
} ;

class LIFECYCLECORBA_EXPORT SALOME_LifeCycleCORBASSL : public SALOME_LifeCycleCORBA
{
public:
  SALOME_LifeCycleCORBASSL();
};

#endif
