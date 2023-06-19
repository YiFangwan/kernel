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

#include "ResourcesManager.hxx" 

#include "SALOME_ResourcesCatalog_Handler.hxx"
#include <Basics_Utils.hxx>
#include <Basics_DirUtils.hxx>
#include "utilities.h"

#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef WIN32
#else
#include <unistd.h>
#endif

#include <algorithm>
#include <memory>

namespace
{
  const std::string DEFAULT_RESOURCE_NAME = "localhost";
  const char* ENV_APPLI = "APPLI";

  static LoadRateManagerFirst first;
  static LoadRateManagerCycl cycl;
  static LoadRateManagerAltCycl altcycl;

  template<class T> bool IsDefaultResource(const T& resource/*, const T& default_resource*/)
  {
    if ((resource.name == DEFAULT_RESOURCE_NAME || resource.name.empty()) && 
        (resource.hostname == DEFAULT_RESOURCE_NAME || resource.hostname == Kernel_Utils::GetHostname()))
    {
      return true;
    }

    return false;

    // Some of the properties of the default resource shouldn't be modified
    // TODO: check if it's a right set of properties to check
    // std::string check;
    // if (default_resource.hostname != resource.hostname)
    //   check += "The Hostname property of the default resource can not be modified.\n";
    // if (default_resource.applipath != resource.applipath)
    //   check += "The Applipath property of the default resource can not be modified.\n";
    // if (default_resource.protocol != resource.protocol)
    //   check += "The Protocol property of the default resource can not be modified.\n";
    // if (!check.empty())
    //   throw ResourcesException(check);
  }

  template<class T> void AddDefaultResourceInCatalogCommon(T& resource)
  {
    resource.name = DEFAULT_RESOURCE_NAME;
    // We can't use "localhost" for parameter hostname because the containers are registered in the
    // naming service with the real hostname, not "localhost"
    resource.hostname = Kernel_Utils::GetHostname();
    resource.protocol = sh;
    resource.batch = none;

    const char* appliStr = getenv(ENV_APPLI);

#ifndef WIN32
    struct stat statbuf;
    std::string aHomePath = Kernel_Utils::HomePath();
    
    if (aHomePath.length() && appliStr)
    {
      if (stat(appliStr, &statbuf) == 0 && S_ISREG(statbuf.st_mode))
      {
          // if $APPLI is a regular file, we asume it's a salome Launcher file
          resource.applipath = std::string(appliStr);
      }
      else
      {
          resource.applipath = aHomePath + "/" + appliStr;
      }
    }
#else
    const char* userprofileStr = getenv("USERPROFILE");
    if (userprofileStr && appliStr)
    {
      resource.applipath = std::string(userprofileStr) + "\\" + appliStr;
    }
#endif
  }

  template<class T, typename U> T GetResourceDefinition(const std::string& name, const U& resourceList)
  {
    const auto it = resourceList.find(name);
    if (it != resourceList.end())
      return (*it).second;

    throw ResourcesException("Resource with a given name does not exist: " + name);
  }

  template<typename T> void RemoveResourceCommon(const char* name, T& resourceList)
  {
    const auto it = resourceList.find(name);
    if (it == resourceList.end())
    {
      MESSAGE("You try to delete a resource that does not exist... : " << name);
      return;
    }

    if (IsDefaultResource((*it).second))
    {
      MESSAGE("Warning! You try to delete a default local resource: " << name);
      return;
    }

    resourceList.erase(name);

    // This is a previouse error handling. Leave here for reference:
    // if (DEFAULT_RESOURCE_NAME == name){
    // std::string error("Cannot delete default local resource \"" + DEFAULT_RESOURCE_NAME + "\"");
    // throw ResourcesException(error);
    // }
  }

  template<typename T> ResourceList GetResourcesByHostname(const std::string& hostnameIn, const T& resourceList)
  {
    if (hostnameIn.empty())
    {
      return {};
    }

    const std::string hostname = 
      hostnameIn == DEFAULT_RESOURCE_NAME ? Kernel_Utils::GetHostname() : hostnameIn;

    ResourceList result;
    for (const auto& res : resourceList)
    {
      if (res.second.hostname == hostname)
        result.push_back(res.first);
    }

    return result;
  }
}

//=============================================================================
/*!
 * just for test
 */ 
//=============================================================================

ResourcesManager_cpp::ResourcesManager_cpp(const char* xmlFilePath)
{
  _path_resources.push_back(xmlFilePath);
#if defined(_DEBUG_) || defined(_DEBUG)
  std::cerr << "ResourcesManager_cpp constructor" << std::endl;
#endif
  _resourceManagerMap["first"]=&first;
  _resourceManagerMap["cycl"]=&cycl;
  _resourceManagerMap["altcycl"]=&altcycl;
  _resourceManagerMap["best"]=&altcycl;
  _resourceManagerMap[""]=&altcycl;

  AddDefaultResourceInCatalog();
  ParseXmlFiles();
}

//=============================================================================
/*!
 *  Standard constructor, parse resource file.
 *  - if ${APPLI} exists in environment,
 *    look for ${HOME}/${APPLI}/CatalogResources.xml
 *  - else look for default:
 *    ${KERNEL_ROOT_DIR}/share/salome/resources/kernel/CatalogResources.xml
 *  - parse XML resource file.
 */ 
//=============================================================================

ResourcesManager_cpp::ResourcesManager_cpp()
{
  MESSAGE("ResourcesManager_cpp constructor");

  _resourceManagerMap["first"]=&first;
  _resourceManagerMap["cycl"]=&cycl;
  _resourceManagerMap["altcycl"]=&altcycl;
  _resourceManagerMap["best"]=&altcycl;
  _resourceManagerMap[""]=&altcycl;

  AddDefaultResourceInCatalog();

  bool default_catalog_resource = true;
  const char* userFileStr = getenv("USER_CATALOG_RESOURCES_FILE");
  if (userFileStr)
  {
    default_catalog_resource = false;
    const std::string user_file(userFileStr);

    std::ifstream ifile(user_file.c_str(), std::ifstream::in );
    if (ifile)
    {
      // The file exists, and is open for input
      _path_resources.push_back(user_file);
    }
    else
    {
      default_catalog_resource = false;
      MESSAGE("Warning: USER_CATALOG_RESOURCES_FILE is set and file cannot be found.")
      MESSAGE("Warning: That's why we try to create a new one.")
      std::ofstream user_catalog_file;
      user_catalog_file.open(user_file.c_str());
      if (user_catalog_file.fail())
      {
        MESSAGE("Error: cannot write in the user catalog resources files");
        MESSAGE("Error: using default CatalogResources.xml file");
        default_catalog_resource = true;
      }
      else
      {
        user_catalog_file << "<!-- File created by SALOME -->" << std::endl;
        user_catalog_file << "<!DOCTYPE ResourcesCatalog>" << std::endl;
        user_catalog_file << "<resources>" << std::endl;
        user_catalog_file << "   <machine name=\"localhost\" hostname=\"localhost\" />" << std::endl;
        user_catalog_file << "</resources>" << std::endl;
        user_catalog_file.close();
      }
    }
  }

  if (default_catalog_resource)
  {
    const char* appliStr = getenv("APPLI");
    if (appliStr)
    {
      std::string default_file = Kernel_Utils::HomePath();
      default_file += "/";
      default_file += appliStr;
      default_file += "/CatalogResources.xml";
      std::ifstream ifile(default_file.c_str(), std::ifstream::in);
      if (ifile)
      {
        // The file exists, and is open for input
        _path_resources.push_back(default_file);
        default_catalog_resource = false;
      }
    }
  }

  if (default_catalog_resource)
  {
    const char* kernelRootStr = getenv("KERNEL_ROOT_DIR");
    if(!kernelRootStr)
      throw ResourcesException("you must define KERNEL_ROOT_DIR environment variable!! -> cannot load a CatalogResources.xml");

    std::string default_file(kernelRootStr);
    default_file += "/share/salome/resources/kernel/CatalogResources.xml";
    _path_resources.push_back(default_file);
  }

  ParseXmlFiles();
  MESSAGE("ResourcesManager_cpp constructor end");
}

//=============================================================================
/*!
 *  Standard Destructor
 */ 
//=============================================================================

ResourcesManager_cpp::~ResourcesManager_cpp()
{
  MESSAGE("ResourcesManager_cpp destructor");
}

//! threadsafe
//=============================================================================
/*!
 *  Finds a container resource
 */ 
//=============================================================================
std::string ResourcesManager_cpp::Find(const std::string& policy,  const ResourceList& possibleContainerResources) const
{
  auto it = _resourceManagerMap.find(policy);
  if(it==_resourceManagerMap.end())
	{
	  it = _resourceManagerMap.find("");
	  return ((*it).second)->Find(possibleContainerResources, _resourcesListContainer);
	}

  return ((*it).second)->Find(possibleContainerResources, _resourcesListContainer);
}

//=============================================================================
//! Get the list of job resource names fitting constraints given by params
/*!
 * Steps:
 * 1: Restrict list with resourceList if defined
 * 2: If name is defined -> check resource list
 * 3: If not 2:, if hostname is defined -> check resource list
 */ 
//=============================================================================
ResourceList ResourcesManager_cpp::GetFittingResourcesJob(const resourceParamsJob& params)
{
  MESSAGE("GetFittingResourcesJob on computer: " << Kernel_Utils::GetHostname());

  ParseXmlFiles();

  // Steps:
  // 1: If name is defined -> check resource list
  // 2: Restrict list with resourceList if defined
  // 3: If not 2:, if hostname is defined -> check resource list

  // Step 1
  if (!params.name.empty())
  { 
    if (_resourcesListJob.find(params.name) != _resourcesListJob.end())
    {
      MESSAGE("Complete. Found a resource with a given name: " << params.name);
      return { params.name };
    }

    throw ResourcesException("Resource name was not found in resource list! Requested name: " + params.name);
  }

  // Step 3
  ResourceList result = GetResourcesByHostname(params.hostname, _resourcesListJob);

  // End
  // Send an exception if return list is empty...
  if (!result.size())
  {
    throw ResourcesException("ResourcesManager doesn't find any resource that fits to your parameters");
  }

  return result;
}

//=============================================================================
//! Get the list of container resource names fitting constraints given by params
/*!
 * Steps:
 * 1: Restrict list with resourceList if defined
 * 2: If name is defined -> check resource list
 * 3: If not 2:, if hostname is defined -> check resource list
 * 4: If not 3:, sort resource with nb_proc, etc...
 * 5: In all cases remove resource that does not correspond with OS
 * 6: And remove resource with componentList - if list is empty ignored it...
 */ 
//=============================================================================
ResourceList ResourcesManager_cpp::GetFittingResourcesContainer(const resourceParamsContainer& params)
{
  MESSAGE("GetFittingResourcesContainer on computer: " << Kernel_Utils::GetHostname());

  // Parse Again CalatogResource File
  ParseXmlFiles();

  // Step 1
  if (!params.name.empty())
  { 
    if (_resourcesListContainer.find(params.name) != _resourcesListContainer.end())
    {
      MESSAGE("Complete. Found a resource with a given name: " << params.name);
      return { params.name };
    }

    throw ResourcesException("Resource name was not found in resource list! Requested name: " + params.name);
  }

  // Step 3
  ResourceList result = GetResourcesByHostname(params.hostname, _resourcesListContainer);

  if (params.hostname.empty())
  {
    // Step 2
    SortResourcesTypeContainerByData(params, result);
  }

  // Step 5
  SelectOnlyResourcesWithOS(params.OS, result);

  // Step 6
  ResourceList vec_save(result);
  KeepOnlyResourcesWithComponent(params.componentList, result);
  if (!result.size())
    result = vec_save;

  // End
  // Send an exception if return list is empty...
  if (!result.size())
  {
    throw ResourcesException("ResourcesManager doesn't find any resource that fits to your parameters");
  }

  return result;
}

//! thread safe
//=============================================================================
/*!
 *  Get a job resource by given name
 */ 
//=============================================================================
ParserResourcesTypeJob ResourcesManager_cpp::GetResourceDefinitionJob(const std::string& name) const
{
  return GetResourceDefinition<ParserResourcesTypeJob, ParserResourcesTypeJob::TypeMap>(name, _resourcesListJob);
}

//=============================================================================
/*!
 *  Get a container resource by given name
 */ 
//=============================================================================
ParserResourcesTypeContainer ResourcesManager_cpp::GetResourceDefinitionContainer(const std::string& name) const
{
  return GetResourceDefinition<ParserResourcesTypeContainer, ParserResourcesTypeContainer::TypeMap>(name, _resourcesListContainer);
}

//=============================================================================
/*!
 *  Add a default job resource into catalog
 */ 
//=============================================================================
void ResourcesManager_cpp::AddDefaultResourceInCatalogJob()
{
  ParserResourcesTypeJob resource;
  AddDefaultResourceInCatalogCommon(resource);

#ifndef WIN32
  std::string tmpdir = "/tmp";
  const char* tmpdirStr = getenv("TMPDIR");
  if (tmpdirStr)
    tmpdir = std::string(tmpdirStr);

  resource.working_directory = tmpdir + "/salome_localres_workdir";
  const char* userStr = getenv("USER");
  if (userStr)
    resource.working_directory += std::string("_") + userStr;
#else
  std::string tmpdir = "C:\\tmp";
  const char* tmpdirStr = getenv("TEMP");
  if (tmpdirStr)
    tmpdir = std::string(tmpdirStr);

  resource.working_directory = tmpdir + "\\salome_localres_workdir";
  const char* usernameStr = getenv("USERNAME");
  if (usernameStr)
    resource.working_directory += std::string("_") + usernameStr;
#endif

  _resourcesListJob[resource.name] = resource;
}

//=============================================================================
/*!
 *  Add a default container resource into catalog
 */ 
//=============================================================================
void ResourcesManager_cpp::AddDefaultResourceInCatalogContainer()
{
  ParserResourcesTypeContainer resource;
  AddDefaultResourceInCatalogCommon(resource);

  resource.dataForSort.name = DEFAULT_RESOURCE_NAME;

  _resourcesListContainer[resource.name] = resource;
}

//=============================================================================
/*!
 *  Add all types of default resources into catalog
 */ 
//=============================================================================
void ResourcesManager_cpp::AddDefaultResourceInCatalog()
{
  AddDefaultResourceInCatalogJob();
  AddDefaultResourceInCatalogContainer();
}

//=============================================================================
/*!
 *  Add a job entry in the resources catalog
 */ 
//=============================================================================

void ResourcesManager_cpp::AddResourceJob(const ParserResourcesTypeJob& resource)
{
  if (IsDefaultResource(resource))
  {
    const auto it = _resourcesListJob.find(DEFAULT_RESOURCE_NAME);
    if (it == _resourcesListJob.end())
    {
      _resourcesListJob[DEFAULT_RESOURCE_NAME] = resource;
    }

    return;
  }

  if (!resource.name.empty())
  {
    MESSAGE("Added a job resource: " << resource.name);
    _resourcesListJob[resource.name] = resource;

    return;
  }

  MESSAGE("Warning! An attempt to add job resource with empty name. Using hostname as a key: " << resource.hostname);
  _resourcesListJob[resource.hostname] = resource;
}

//=============================================================================
/*!
 *  Add a container entry in the resources catalog
 */ 
//=============================================================================

void ResourcesManager_cpp::AddResourceContainer(const ParserResourcesTypeContainer& resource)
{
  if (IsDefaultResource(resource))
  {
    const auto it = _resourcesListContainer.find(DEFAULT_RESOURCE_NAME);
    if (it != _resourcesListContainer.end())
    {
      MESSAGE("An attempt to add default container resource. Using an old default object with overwritten DataForSort.");

      ParserResourcesTypeContainer& defaultContainer((*it).second);
      defaultContainer.dataForSort.nb_node = resource.dataForSort.nb_node;
      defaultContainer.dataForSort.nb_proc_per_node = resource.dataForSort.nb_proc_per_node;
      defaultContainer.dataForSort.cpu_clock = resource.dataForSort.cpu_clock;
      defaultContainer.dataForSort.mem_mb = resource.dataForSort.mem_mb;
    }
    else
    {
      MESSAGE("Warning! Added new default container resource: " << DEFAULT_RESOURCE_NAME);
      _resourcesListContainer[DEFAULT_RESOURCE_NAME] = resource;
    }

    return;
  }

  if (!resource.name.empty())
  {
    MESSAGE("Added a container resource: " << resource.name);
    _resourcesListContainer[resource.name] = resource;

    return;
  }

  MESSAGE("Warning! An attempt to add container resource with empty name. Using hostname as a key: " << resource.hostname);
  _resourcesListContainer[resource.hostname] = resource;
}

//=============================================================================
/*!
 *  Deletes a job resource from the catalog
 */ 
//=============================================================================
void ResourcesManager_cpp::RemoveResourceJob(const char* name)
{
  RemoveResourceCommon(name, _resourcesListJob);
}

//=============================================================================
/*!
 *  Deletes a container resource from the catalog
 */ 
//=============================================================================
void ResourcesManager_cpp::RemoveResourceContainer(const char* name)
{
  RemoveResourceCommon(name, _resourcesListContainer);
}

//=============================================================================
/*!
 *  Write the current data in memory in file.
 */ 
//=============================================================================

void ResourcesManager_cpp::WriteInXmlFile(std::string xml_file)
{
  MESSAGE("Start writing resource to XML file...");

  //MapOfParserResourcesType resourceListToSave(_resourcesList);
  if (_resourcesListJob.empty() || _resourcesListContainer.empty())
  {
    MESSAGE("Nothing to do, no resource to save!");
    return;
  }

  if (xml_file.empty())
  {
    if (_path_resources.empty())
    {
      MESSAGE("Warning! Empty path to resource file. Return.");
      return;
    }

    xml_file = _path_resources.front();
  }

  const char* aFilePath = xml_file.c_str();
  FILE* aFile = fopen(aFilePath, "w");
  if (aFile == NULL)
  {
    MESSAGE("Error opening file: " << xml_file);
    return;
  }
  
  xmlDocPtr aDoc = xmlNewDoc(BAD_CAST "1.0");
  xmlNewDocComment(aDoc, BAD_CAST "ResourcesCatalog");

  const std::unique_ptr<SALOME_ResourcesCatalog_Handler> handler(
    new SALOME_ResourcesCatalog_Handler(_resourcesListJob, _resourcesListContainer));
  handler->writeXmlDoc(aDoc);

  if (xmlSaveFormatFile(aFilePath, aDoc, 1) == -1)
  {
    MESSAGE("Error saving file: " << xml_file);
  }
  
  // Free the document
  xmlFreeDoc(aDoc);
  fclose(aFile);

  MESSAGE("Complete! Resources has written in XML file: " << xml_file);
}

bool ResourcesManager_cpp::IsNeedToParse()
{
  // Parse file only if its modification time is greater than lasttime (last registered modification time)
  bool to_parse = false;
  for(const auto& path :_path_resources)
  {
    struct stat statinfo;
    if (stat(path.c_str(), &statinfo) < 0)
    {
      MESSAGE("Resource file " << path << " does not exist");
      return false;
    }

    if(_lasttime == 0 || statinfo.st_mtime > _lasttime)
    {
      to_parse = true;
      _lasttime = statinfo.st_mtime;
    }
  }

  return to_parse;
}

void ResourcesManager_cpp::ClearResourcesCatalog()
{
  _resourcesListJob.clear();
  _resourcesListContainer.clear();
}

//=============================================================================
/*!
 *  Parse the data type catalog
 */ 
//=============================================================================

void ResourcesManager_cpp::ParseXmlFiles()
{
  if (!IsNeedToParse())
  {
    return;
  }

  ClearResourcesCatalog();
  AddDefaultResourceInCatalog();

  // Iterate all resource files
  for(const auto& path :_path_resources)
  {
    ParseResourceFile(path);
  }
}

//=============================================================================
/*!
 *  Read all resources from a given XML file
 */ 
//=============================================================================

void ResourcesManager_cpp::ParseResourceFile(const std::string& resFile)
{
  MESSAGE("Path to resource file to parse: " << resFile);

  ParserResourcesTypeJob::TypeMap _resourcesListJobTemp;
  ParserResourcesTypeContainer::TypeMap _resourcesListContainerTemp;
  std::unique_ptr<SALOME_ResourcesCatalog_Handler> handler(
    new SALOME_ResourcesCatalog_Handler(_resourcesListJobTemp, _resourcesListContainerTemp));

  const char* aFilePath((resFile).c_str());
  FILE* aFile = fopen(aFilePath, "r");
  if (!aFile)
  {
    std::cerr << "ResourcesManager_cpp: file " << aFilePath << " is not readable." << std::endl;
    return;
  }

  xmlDocPtr aDoc = xmlReadFile(aFilePath, NULL, XML_PARSE_NOBLANKS);
  if (!aDoc)
  {
    std::cerr << "ResourcesManager_cpp: could not parse file " << aFilePath << std::endl;
    return;
  }

  handler->readXmlDoc(aDoc);

  // Adding new resources to the catalog
  for (const auto& res : _resourcesListJobTemp)
  {
    AddResourceJob(res.second);
  }

  for (const auto& res : _resourcesListContainerTemp)
  {
    AddResourceContainer(res.second);
  }

  // Free the document
  xmlFreeDoc(aDoc);
  fclose(aFile);
}

//=============================================================================
/*!
 *  Sort container resources. It's a helper method for GetFittingResourcesContainer()
 */ 
//=============================================================================

void ResourcesManager_cpp::SortResourcesTypeContainerByData(
  const resourceParamsContainer& params, ResourceList& resourcesOut)
{
  ParserResourcesTypeContainer::TypeMap local_resourcesList = _resourcesListContainer;
  if (params.resourceList.size())
  {
    MESSAGE("Restricted resource list found!");
    local_resourcesList.clear();

    for (const auto& res : params.resourceList)
    {
      if (_resourcesListContainer.find(res) != _resourcesListContainer.end())
        local_resourcesList[res] = _resourcesListContainer[res];
    }
  }

  // Search for available resources sorted by priority
  for (const auto& res : local_resourcesList)
  {
    resourcesOut.push_back(res.first);
  }

  // Set wanted parameters
  ResourceDataToSort::nb_proc_wanted = params.nb_proc;
  ResourceDataToSort::nb_node_wanted = params.nb_node;
  ResourceDataToSort::nb_proc_per_node_wanted = params.nb_proc_per_node;
  ResourceDataToSort::cpu_clock_wanted = params.cpu_clock;
  ResourceDataToSort::mem_mb_wanted = params.mem_mb;
  // --- end of set
      
  // Sort
  std::list<ResourceDataToSort> li;
  for (const auto& res : resourcesOut)
  {
    li.push_back(local_resourcesList[res].dataForSort);
  }
  li.sort();

  resourcesOut.clear();
  for (const auto& data : li)
  {
    resourcesOut.push_back(data.name);
  }
}

//=============================================================================
/*!
 *   Consult the content of the job list
 */ 
//=============================================================================
const ParserResourcesTypeJob::TypeMap& ResourcesManager_cpp::ListAllResourcesInCatalogJob() const
{
  return _resourcesListJob;
}

//=============================================================================
/*!
 *   Consult the content of the container list
 */ 
//=============================================================================
const ParserResourcesTypeContainer::TypeMap& ResourcesManager_cpp::ListAllResourcesInCatalogContainer() const
{
  return _resourcesListContainer;
}

//=============================================================================
/*!
 *  Gives a sublist of resources with matching OS.
 *  If parameter OS is empty, gives the complete list of resources
 */ 
//=============================================================================
void ResourcesManager_cpp::SelectOnlyResourcesWithOS(const std::string& OS, ResourceList& resourcesOut) const
{
  // A computer list is given : take only resources with OS on those computers
  if (OS.empty())
  {
    return;
  }

  // This predicate returns true if an element should be removed
  auto doesntHaveOs = [&] (const std::string& resName) -> bool
  {    
    const auto it = _resourcesListContainer.find(resName);
    return it == _resourcesListContainer.end() || (*it).second.OS != OS;
  };

  // Erase elements without OS in place
  resourcesOut.erase(std::remove_if(resourcesOut.begin(), resourcesOut.end(), doesntHaveOs), resourcesOut.end());
}


//=============================================================================
/*!
 *  Gives a sublist of machines on which the component is known.
 */ 
//=============================================================================
void ResourcesManager_cpp::KeepOnlyResourcesWithComponent(const ResourceList& componentList, ResourceList& resourcesOut) const
{
  // This predicate returns true if an element should be removed
  auto hasMismatchComponent = [&] (const std::string& resName) -> bool
  {
    const auto& curComponentList = _resourcesListContainer.at(resName).componentList;

    // If a resource  doesn't have any module or component defined in the catalog,
    // it means that it has every module and component and it fits for any request.
    if (!componentList.size() || !curComponentList.size())
    {
      return false;
    }

    // We expect the names in list to be unique, so the number of elements in a given list
    // must be less or equal than in list from current resource.
    if (componentList.size() > curComponentList.size())
    {
      return true;
    }

    for (const auto& component : componentList)
    {
       const auto it = std::find(curComponentList.begin(), curComponentList.end(), component);
       if (it == curComponentList.end())
       {
          return true;
       }
    }

    return false;
  };

  // Erase elements without given components in place
  resourcesOut.erase(std::remove_if(resourcesOut.begin(), resourcesOut.end(), hasMismatchComponent), resourcesOut.end());
}
