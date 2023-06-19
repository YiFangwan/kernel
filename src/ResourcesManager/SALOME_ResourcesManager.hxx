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

#ifndef __SALOME_RESOURCESMANAGER_HXX__
#define __SALOME_RESOURCESMANAGER_HXX__

#include "Utils_SALOME_Exception.hxx"
#include "utilities.h"
#include <SALOMEconfig.h>
#include "SALOME_NamingService_Abstract.hxx"
#include CORBA_CLIENT_HEADER(SALOME_ContainerManager)
#include <fstream>
#include <memory>
#include "ResourcesManager.hxx"

#include "SALOME_ResourcesManager_Common.hxx"

#ifdef WIN32
#pragma warning(disable:4275) // Disable warning interface non dll
#endif

// --- WARNING ---
// The call of BuildTempFileToLaunchRemoteContainer and RmTmpFile must be done
// in a critical section to be sure to be clean.
// Only one thread should use the SALOME_ResourcesManager class in a SALOME
// session.

class SALOMERESOURCESMANAGER_EXPORT SALOME_ResourcesManager:
  public POA_Engines::ResourcesManager,
  public PortableServer::RefCountServantBase
  {

  public:

    SALOME_ResourcesManager(CORBA::ORB_ptr orb, PortableServer::POA_var poa, SALOME_NamingService_Abstract *ns, const char *xmlFilePath);
    SALOME_ResourcesManager(CORBA::ORB_ptr orb, PortableServer::POA_var poa, SALOME_NamingService_Abstract *ns);

    ~SALOME_ResourcesManager();

    // CORBA Methods
    // ======================================================================================

    char* FindFirst(const Engines::ResourceList& possibleContainerResources);
    char* Find(const char *policy, const Engines::ResourceList& possibleContainerResources);

    Engines::ResourceList* GetFittingResourcesJob(const Engines::ResourceParametersJob& params);
    Engines::ResourceList* GetFittingResourcesContainer(const Engines::ResourceParametersContainer& params);
    
    Engines::ResourceDefinitionJob* GetResourceDefinitionJob(const char* name);
    Engines::ResourceDefinitionContainer* GetResourceDefinitionContainer(const char* name);

    void AddResourceJob(const Engines::ResourceDefinitionJob& new_resource, CORBA::Boolean write, const char* xml_file);
    void AddResourceContainer(const Engines::ResourceDefinitionContainer& new_resource, CORBA::Boolean write, const char* xml_file);

    void RemoveResourceJob(const char* resource_name, CORBA::Boolean write, const char* xml_file);
    void RemoveResourceContainer(const char* resource_name, CORBA::Boolean write, const char* xml_file);

    char* getMachineFile(const char* resource_name, CORBA::Long nb_procs, const char* parallelLib);

    void ListAllAvailableResourcesContainer(Engines::ResourceList_out machines, Engines::IntegerList_out nbProcsOfMachines);
    
    Engines::ResourceList* ListAllResourcesInCatalogJob();
    Engines::ResourceList* ListAllResourcesInCatalogContainer();

    // Cpp Methods
    // ======================================================================================

    void Shutdown();
    std::shared_ptr<ResourcesManager_cpp>& GetImpl() { return _rm; }

  public:
    static const char *_ResourcesManagerNameInNS;

  protected:
    SALOME_NamingService_Abstract *_NS;
    CORBA::ORB_var _orb;
    PortableServer::POA_var _poa;
    std::shared_ptr<ResourcesManager_cpp> _rm;
  };

#endif // RESSOURCESCATALOG_IMPL_H
