// Copyright (C) 2005  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
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
#include "SALOME_ResourcesCatalog_Handler.hxx"
#include "SALOME_LoadRateManager.hxx"
#include "SALOME_NamingService.hxx"
#include CORBA_CLIENT_HEADER(SALOME_ContainerManager)
#include <string>
#include <fstream>
#include <vector>

#if defined RESOURCESMANAGER_EXPORTS
#if defined WIN32
#define RESOURCESMANAGER_EXPORT __declspec( dllexport )
#else
#define RESOURCESMANAGER_EXPORT
#endif
#else
#if defined WNT
#define RESOURCESMANAGER_EXPORT __declspec( dllimport )
#else
#define RESOURCESMANAGER_EXPORT
#endif
#endif

// --- WARNING ---
// The call of BuildTempFileToLaunchRemoteContainer and RmTmpFile must be done
// in a critical section to be sure to be clean.
// Only one thread should use the SALOME_ResourcesManager class in a SALOME
// session.

class RESOURCESMANAGER_EXPORT SALOME_ResourcesManager:
  public POA_Engines::ResourcesManager,
  public PortableServer::RefCountServantBase
  {

  public:

    SALOME_ResourcesManager(CORBA::ORB_ptr orb, PortableServer::POA_var poa, SALOME_NamingService *ns, const char *xmlFilePath);
    SALOME_ResourcesManager(CORBA::ORB_ptr orb, PortableServer::POA_var poa, SALOME_NamingService *ns);

    ~SALOME_ResourcesManager();

    Engines::MachineList *
    GetFittingResources(const Engines::MachineParameters& params,
                        const Engines::CompoList& componentList);
//     throw(SALOME_Exception);

    char* FindFirst(const Engines::MachineList& listOfMachines);
    std::string FindNext(const Engines::MachineList& listOfMachines);
    std::string FindBest(const Engines::MachineList& listOfMachines);

    int AddResourceInCatalog
    (const Engines::MachineParameters& paramsOfNewResources,
     const std::vector<std::string>& modulesOnNewResources,
     const char *alias,
     const char *userName,
     AccessModeType mode,
     AccessProtocolType prot)
    throw(SALOME_Exception);

    void DeleteResourceInCatalog(const char *hostname);

    void WriteInXmlFile();

    const MapOfParserResourcesType& ParseXmlFile();

    const MapOfParserResourcesType& GetList() const;

    Engines::MachineParameters* GetMachineParameters(const char *hostname);

    void Shutdown();

    static const char *_ResourcesManagerNameInNS;

    ParserResourcesType GetResourcesList(const std::string& machine);

  protected:
    
    SALOME_NamingService *_NS;
    CORBA::ORB_var _orb;
    PortableServer::POA_var _poa;

    void SelectOnlyResourcesWithOS(std::vector<std::string>& hosts,
				   const char *OS) const
      throw(SALOME_Exception);

    void KeepOnlyResourcesWithModule(std::vector<std::string>& hosts,
				     const Engines::CompoList& componentList) const
      throw(SALOME_Exception);

    //! will contain the path to the ressources catalog
    std::string _path_resources;

    //! will contain the informations on the data type catalog(after parsing)
    MapOfParserResourcesType _resourcesList;

    SALOME_LoadRateManager _dynamicResourcesSelecter;

    //! different behaviour if $APPLI exists (SALOME Application) 
    bool _isAppliSalomeDefined;
  };

#endif // RESSOURCESCATALOG_IMPL_H
