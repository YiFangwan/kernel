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

#ifndef __RESOURCESMANAGER_HXX__
#define __RESOURCESMANAGER_HXX__

#include "ResourcesManager_Defs.hxx"
#include "SALOME_ParserResourcesTypeJob.hxx"
#include "SALOME_ParserResourcesTypeContainer.hxx"
#include "SALOME_LoadRateManager.hxx"

#include <list>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef WIN32
#else
#include <unistd.h>
#endif

#ifdef WIN32
#pragma warning(disable:4251) // Warning DLL Interface ...
//#pragma warning(disable:4290) // Warning Exception ...
#endif

using ResourceList = std::vector<std::string>;

// --- WARNING ---
// The call of BuildTempFileToLaunchRemoteContainer and RmTmpFile must be done
// in a critical section to be sure to be clean.
// Only one thread should use the SALOME_ResourcesManager class in a SALOME
// session.
struct RESOURCESMANAGER_EXPORT resourceParams
{
  std::string name;
  std::string hostname;

  ResourceList resourceList;
};

struct RESOURCESMANAGER_EXPORT resourceParamsJob : resourceParams
{
};

struct RESOURCESMANAGER_EXPORT resourceParamsContainer : resourceParams
{
  std::string OS;

  long nb_proc = -1;
  long nb_node = 0;
  long nb_proc_per_node = -1;
  long cpu_clock = -1;
  long mem_mb = -1;

  ResourceList componentList;
};

class RESOURCESMANAGER_EXPORT ResourcesManager_cpp
{
public:
  ResourcesManager_cpp(const char* xmlFilePath);
  ResourcesManager_cpp();

  virtual ~ResourcesManager_cpp();

  std::string Find(const std::string& policy, const ResourceList& possibleContainerResources) const;

  ResourceList GetFittingResourcesJob(const resourceParamsJob& params);
  ResourceList GetFittingResourcesContainer(const resourceParamsContainer& params);

  ParserResourcesTypeJob GetResourceDefinitionJob(const std::string& name) const;
  ParserResourcesTypeContainer GetResourceDefinitionContainer(const std::string& name) const;

  void AddResourceJob(const ParserResourcesTypeJob& resource);
  void AddResourceContainer(const ParserResourcesTypeContainer& resource);

  void RemoveResourceJob(const char* name);
  void RemoveResourceContainer(const char* name);

  const ParserResourcesTypeJob::TypeMap& ListAllResourcesInCatalogJob() const;
  const ParserResourcesTypeContainer::TypeMap& ListAllResourcesInCatalogContainer() const;
  
  void WriteInXmlFile(std::string xml_file);
  void ParseXmlFiles();

protected:
  bool IsNeedToParse();
  void ClearResourcesCatalog();
  void ParseResourceFile(const std::string& resFile);
  void SortResourcesTypeContainerByData(const resourceParamsContainer& params, ResourceList& resourcesOut);

  void SelectOnlyResourcesWithOS(const std::string& OS, ResourceList& resourcesOut) const;
  void KeepOnlyResourcesWithComponent(const ResourceList& componentList, ResourceList& resourcesOut) const;

  /**
   * Add the default local resource in the catalog
   */
  void AddDefaultResourceInCatalogJob();
  void AddDefaultResourceInCatalogContainer();
  void AddDefaultResourceInCatalog();

  //! will contain the path to the resources catalog
  ResourceList _path_resources;

  //! will contain the information on the data type catalog(after parsing)
  ParserResourcesTypeJob::TypeMap _resourcesListJob;
  ParserResourcesTypeContainer::TypeMap _resourcesListContainer;

  //! a map that contains all the available load rate managers (the key is the name)
  std::map<std::string, LoadRateManager*> _resourceManagerMap;

  //! contain the time where resourcesList was created
  time_t _lasttime = 0;
};

#endif // __RESOURCESMANAGER_HXX__
