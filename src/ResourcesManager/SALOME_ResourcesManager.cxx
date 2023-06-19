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

#include "SALOME_ResourcesManager.hxx" 
#include "Utils_ExceptHandlers.hxx"
#include "Utils_CorbaException.hxx"
#include "OpUtil.hxx"

#include <stdlib.h>
#include <stdio.h>
#ifndef WIN32
#include <unistd.h>
#else
#include <io.h>
#include <process.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include "utilities.h"

const char *SALOME_ResourcesManager::_ResourcesManagerNameInNS = "/ResourcesManager";

namespace
{
  template <typename T>
  Engines::ResourceList* ListAllResourcesInCatalog(const T& zeList)
  {
    Engines::ResourceList* ret(new Engines::ResourceList);
    ret->length(zeList.size());
    CORBA::ULong i(0);
    for(const auto it : zeList)
    {
      // TODO: it was a hostname here, but we use a name to keep and search resources.
      // So, changed to name to have the same approach. Need to check though.
      (*ret)[i++] = CORBA::string_dup(it.second.name.c_str() );
    }
    return ret;
  }
}

//=============================================================================
/*!
 * just for test
 */ 
//=============================================================================

SALOME_ResourcesManager::SALOME_ResourcesManager(CORBA::ORB_ptr orb,
                                                 PortableServer::POA_var poa,
                                                 SALOME_NamingService_Abstract *ns,
                                                 const char *xmlFilePath)
: _rm(new ResourcesManager_cpp(xmlFilePath))
{
  MESSAGE("SALOME_ResourcesManager constructor");
  _NS = ns;
  _orb = CORBA::ORB::_duplicate(orb) ;
  //
  PortableServer::POAManager_var pman = poa->the_POAManager();
  CORBA::PolicyList policies;
  policies.length(1);
  PortableServer::ThreadPolicy_var threadPol(poa->create_thread_policy(PortableServer::SINGLE_THREAD_MODEL));
  policies[0] = PortableServer::ThreadPolicy::_duplicate(threadPol);
  _poa = poa->create_POA("SingleThreadPOA",pman,policies);
  threadPol->destroy();
  //
  PortableServer::ObjectId_var id = _poa->activate_object(this);
  CORBA::Object_var obj = _poa->id_to_reference(id);
  Engines::ResourcesManager_var refContMan = Engines::ResourcesManager::_narrow(obj);
  _NS->Register(refContMan,_ResourcesManagerNameInNS);
  MESSAGE("SALOME_ResourcesManager constructor end");
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

SALOME_ResourcesManager::SALOME_ResourcesManager(CORBA::ORB_ptr orb, 
                                                 PortableServer::POA_var poa, 
                                                 SALOME_NamingService_Abstract *ns) : _rm(new ResourcesManager_cpp())
{
  MESSAGE("SALOME_ResourcesManager constructor");
  _NS = ns;
  _orb = CORBA::ORB::_duplicate(orb) ;
  //
  PortableServer::POAManager_var pman = poa->the_POAManager();
  CORBA::PolicyList policies;
  policies.length(1);
  PortableServer::ThreadPolicy_var threadPol(poa->create_thread_policy(PortableServer::SINGLE_THREAD_MODEL));
  policies[0] = PortableServer::ThreadPolicy::_duplicate(threadPol);
  _poa = poa->create_POA("SingleThreadPOA",pman,policies);
  threadPol->destroy();
  //
  PortableServer::ObjectId_var id = _poa->activate_object(this);
  CORBA::Object_var obj = _poa->id_to_reference(id);
  Engines::ResourcesManager_var refContMan = Engines::ResourcesManager::_narrow(obj);
  if(_NS)
    _NS->Register(refContMan,_ResourcesManagerNameInNS);

  MESSAGE("SALOME_ResourcesManager constructor end");
}

//=============================================================================
/*!
 *  Standard Destructor
 */ 
//=============================================================================

SALOME_ResourcesManager::~SALOME_ResourcesManager()
{
  MESSAGE("SALOME_ResourcesManager destructor");
}

//=============================================================================
/*!
 *  dynamically obtains the first machines
 */ 
//=============================================================================

char *
SALOME_ResourcesManager::FindFirst(const Engines::ResourceList& possibleContainerResources)
{
  // CORBA -> C++
  const auto rl = resourceList_CORBAtoCPP(possibleContainerResources);

  return CORBA::string_dup(_rm->Find("first", rl).c_str());
}

char *
SALOME_ResourcesManager::Find(const char* policy, const Engines::ResourceList& possibleContainerResources)
{
  // CORBA -> C++
  const auto rl = resourceList_CORBAtoCPP(possibleContainerResources);

  return CORBA::string_dup(_rm->Find(policy, rl).c_str());
}

//=============================================================================
//! get the name of job resources fitting the specified constraints (params)
/*!
 *  Look at ResourcesManager_cpp class for implementation 
 */ 
//=============================================================================

Engines::ResourceList *
SALOME_ResourcesManager::GetFittingResourcesJob(const Engines::ResourceParametersJob& params)
{
  Engines::ResourceList_var ret;

  // CORBA -> C++
  const resourceParamsJob p = resourceParametersJob_CORBAtoCPP(params);

  try
  {
    // Call C++ ResourceManager
    const auto vec = _rm->GetFittingResourcesJob(p);

    // C++ -> CORBA
    ret = resourceList_CPPtoCORBA(vec);
  }
  catch(const ResourcesException &ex)
  {
    INFOS("Caught exception in GetFittingResourcesJob C++:  " << ex.msg);
    THROW_SALOME_CORBA_EXCEPTION(ex.msg.c_str(),SALOME::BAD_PARAM);
  }  

  return ret._retn();
}

//=============================================================================
//! get the name of container resources fitting the specified constraints (params)
/*!
 *  Look at ResourcesManager_cpp class for implementation 
 */ 
//=============================================================================

Engines::ResourceList *
SALOME_ResourcesManager::GetFittingResourcesContainer(const Engines::ResourceParametersContainer& params)
{
  Engines::ResourceList_var ret;

  // CORBA -> C++
  const resourceParamsContainer p = resourceParametersContainer_CORBAtoCPP(params);

  try
  {
    // Call C++ ResourceManager
    const auto vec = _rm->GetFittingResourcesContainer(p);

    // C++ -> CORBA
    ret = resourceList_CPPtoCORBA(vec);
  }
  catch(const ResourcesException& ex)
  {
    INFOS("Caught exception in GetFittingResourcesContainer C++:  " << ex.msg);
    THROW_SALOME_CORBA_EXCEPTION(ex.msg.c_str(),SALOME::BAD_PARAM);
  }  

  return ret._retn();
}

Engines::ResourceDefinitionJob*
SALOME_ResourcesManager::GetResourceDefinitionJob(const char* name)
{
  Engines::ResourceDefinitionJob_var resDef;
  try
  {
    const auto resource = _rm->GetResourceDefinitionJob(name);
    resDef = resourceDefinitionJob_CPPtoCORBA(resource);
  }
  catch (const ResourcesException& ex)
  {
    INFOS("Caught exception in GetResourceDefinitionJob: " << ex.msg);
    THROW_SALOME_CORBA_EXCEPTION(ex.msg.c_str(), SALOME::BAD_PARAM);
  }

  return resDef._retn();
}

Engines::ResourceDefinitionContainer*
SALOME_ResourcesManager::GetResourceDefinitionContainer(const char* name)
{
  Engines::ResourceDefinitionContainer_var resDef;
  try
  {
    const auto resource = _rm->GetResourceDefinitionContainer(name);
    resDef = resourceDefinitionContainer_CPPtoCORBA(resource);
  }
  catch (const ResourcesException& ex)
  {
    INFOS("Caught exception in GetResourceDefinitionContainer: " << ex.msg);
    THROW_SALOME_CORBA_EXCEPTION(ex.msg.c_str(), SALOME::BAD_PARAM);
  }

  return resDef._retn();
}

void SALOME_ResourcesManager::AddResourceJob(
  const Engines::ResourceDefinitionJob& new_resource,
  CORBA::Boolean write,
  const char* xml_file)
{
  try
  {
    const auto resource = resourceDefinitionJob_CORBAtoCPP(new_resource);
    _rm->AddResourceJob(resource);

    if (write)
    {
      _rm->WriteInXmlFile(std::string(xml_file));
      _rm->ParseXmlFiles();
    }
  }
  catch (const SALOME_Exception& e)
  {
    INFOS("Error in AddResourceJob: " << e);
    THROW_SALOME_CORBA_EXCEPTION(e.what(), SALOME::BAD_PARAM);
  }
  catch (const ResourcesException& e)
  {
    INFOS("Error in AddResourceJob: " << e.msg);
    THROW_SALOME_CORBA_EXCEPTION(e.msg.c_str(), SALOME::BAD_PARAM);
  }
}

void SALOME_ResourcesManager::AddResourceContainer(
  const Engines::ResourceDefinitionContainer& new_resource,
  CORBA::Boolean write,
  const char* xml_file)
{
  try
  {
    const auto resource = resourceDefinitionContainer_CORBAtoCPP(new_resource);
    _rm->AddResourceContainer(resource);

    if (write)
    {
      _rm->WriteInXmlFile(std::string(xml_file));
      _rm->ParseXmlFiles();
    }
  }
  catch (const SALOME_Exception& e)
  {
    INFOS("Error in AddResourceContainer: " << e);
    THROW_SALOME_CORBA_EXCEPTION(e.what(), SALOME::BAD_PARAM);
  }
  catch (const ResourcesException& e)
  {
    INFOS("Error in AddResourceContainer: " << e.msg);
    THROW_SALOME_CORBA_EXCEPTION(e.msg.c_str(), SALOME::BAD_PARAM);
  }
}

void SALOME_ResourcesManager::RemoveResourceJob(
  const char* resource_name,
  CORBA::Boolean write,
  const char* xml_file)
{
  try
  {
    _rm->RemoveResourceJob(resource_name);
  }
  catch (const SALOME_Exception& e)
  {
    INFOS("Error in RemoveResourceJob: " << e);
    THROW_SALOME_CORBA_EXCEPTION(e.what(), SALOME::BAD_PARAM);
  }

  if (write)
  {
    _rm->WriteInXmlFile(std::string(xml_file));
    _rm->ParseXmlFiles();
  }
}

void SALOME_ResourcesManager::RemoveResourceContainer(
  const char* resource_name,
  CORBA::Boolean write,
  const char* xml_file)
{
  try
  {
    _rm->RemoveResourceContainer(resource_name);
  }
  catch (const SALOME_Exception& e)
  {
    INFOS("Error in RemoveResourceContainer: " << e);
    THROW_SALOME_CORBA_EXCEPTION(e.what(), SALOME::BAD_PARAM);
  }

  if (write)
  {
    _rm->WriteInXmlFile(std::string(xml_file));
    _rm->ParseXmlFiles();
  }
}

char *
SALOME_ResourcesManager::getMachineFile(const char* resource_name,
                                        CORBA::Long nb_procs, 
                                        const char* parallelLib)
{
  std::string machine_file_name;

  if (std::string(parallelLib) == "Dummy")
  {
    MESSAGE("[getMachineFile] parallelLib is Dummy");
    const auto& resourcesList = _rm->ListAllResourcesInCatalogContainer();

    if (resourcesList.find(std::string(resource_name)) != resourcesList.end())
    {
      ParserResourcesTypeContainer resource = resourcesList.at(std::string(resource_name));

      // Check if resource is cluster or not
      if (resource.ClusterMembersList.empty())
      {
        //It is not a cluster so we create a cluster with one machine
        ParserResourcesTypeContainer fake_node;
        fake_node.hostname = resource.hostname;
        fake_node.protocol = resource.protocol;
        //fake_node.ClusterInternalProtocol = resource.ClusterInternalProtocol;
        fake_node.username = resource.username;
        fake_node.applipath = resource.applipath;
        fake_node.dataForSort = resource.dataForSort;

        resource.ClusterMembersList.push_front(fake_node);
      }

      // Creating list of machines for creating the machine file
      std::list<std::string> list_of_machines;
      std::list<ParserResourcesTypeContainer>::iterator cluster_it = resource.ClusterMembersList.begin();
      while (cluster_it != resource.ClusterMembersList.end())
      {
        // For each member of the cluster we add a nbOfNodes * nbOfProcPerNode in the list
        unsigned int number_of_proc = (*cluster_it).dataForSort.nb_node * 
                                      (*cluster_it).dataForSort.nb_proc_per_node;
        for (unsigned int i = 0; i < number_of_proc; i++)
          list_of_machines.push_back((*cluster_it).hostname);
        cluster_it++;
      }

      // Creating machine file
      machine_file_name = tmpnam(NULL);
      std::ofstream machine_file(machine_file_name.c_str(), std::ios_base::out);

      CORBA::Long machine_number = 0;
      std::list<std::string>::iterator it = list_of_machines.begin();
      while (machine_number != nb_procs)
      {
        // Adding a new node to the machine file
        machine_file << *it << std::endl;

        // counting...
        it++;
        if (it == list_of_machines.end())
          it = list_of_machines.begin();
        machine_number++;
      }
    }
    else
      INFOS("[getMachineFile] Error resource_name not found in resourcesList -> " << resource_name);
  }
  // TODO: check if we need to rewrite commented code below:
  // else if (std::string(parallelLib) == "Mpi")
  // {
  //   MESSAGE("[getMachineFile] parallelLib is Mpi");
  //   const auto& resourcesList = _rm->ListAllResourcesInCatalogContainer();

  //   if (resourcesList.find(std::string(resource_name)) != resourcesList.end())
  //   {
  //     ParserResourcesTypeContainer resource = resourcesList.at(std::string(resource_name));
  //     // Check if resource is cluster or not
  //     if (resource.ClusterMembersList.empty())
  //     {
  //       //It is not a cluster so we create a cluster with one machine
  //       ParserResourcesTypeContainer fake_node;
  //       fake_node.hostname = resource.hostname;
  //       fake_node.protocol = resource.protocol;
  //       //fake_node.ClusterInternalProtocol = resource.ClusterInternalProtocol;
  //       fake_node.username = resource.username;
  //       fake_node.applipath = resource.applipath;
  //       fake_node.dataForSort = resource.dataForSort;

  //       resource.ClusterMembersList.push_front(fake_node);
  //     }

  //     // Choose mpi implementation -> each MPI implementation has is own machinefile...
  //     if (resource.mpi == lam)
  //     {
  //       // Creating machine file
  //       machine_file_name = tmpnam(NULL);
  //       std::ofstream machine_file(machine_file_name.c_str(), std::ios_base::out);

  //       // We add all cluster machines to the file
  //       std::list<ParserResourcesTypeContainer>::iterator cluster_it = resource.ClusterMembersList.begin();
  //       while (cluster_it != resource.ClusterMembersList.end())
  //       {
  //         unsigned int number_of_proc = (*cluster_it).dataForSort.nb_node * 
  //           (*cluster_it).dataForSort.nb_proc_per_node;
  //         machine_file << (*cluster_it).hostname << " cpu=" << number_of_proc << std::endl;
  //         cluster_it++;
  //       }
  //     }
  //     else if ((resource.mpi == openmpi) || (resource.mpi == ompi))
  //     {
  //       // Creating machine file
  //       machine_file_name = tmpnam(NULL);
  //       std::ofstream machine_file(machine_file_name.c_str(), std::ios_base::out);

  //       // We add all cluster machines to the file
  //       std::list<ParserResourcesTypeContainer>::iterator cluster_it = resource.ClusterMembersList.begin();
  //       while (cluster_it != resource.ClusterMembersList.end())
  //       {
  //         unsigned int number_of_proc = (*cluster_it).dataForSort.nb_node *
  //           (*cluster_it).dataForSort.nb_proc_per_node;
  //         machine_file << (*cluster_it).hostname << " slots=" << number_of_proc << std::endl;
  //         cluster_it++;
  //       }
  //     }
  //     else if (resource.mpi == nompi)
  //     {
  //       INFOS("[getMachineFile] Error resource_name MPI implementation was defined for " << resource_name);
  //     }
  //     else
  //       INFOS("[getMachineFile] Error resource_name MPI implementation not currently handled for " << resource_name);
  //   }
  //   else
  //     INFOS("[getMachineFile] Error resource_name not found in resourcesList -> " << resource_name);
  // }
  else
    INFOS("[getMachineFile] Error parallelLib is not handled -> " << parallelLib);

  return CORBA::string_dup(machine_file_name.c_str());
}

/*!
 * Return list of container resources available (regarding content of CatalogResources.xml).
 * And for each resource the number of proc available of it.
 * 
 * \sa SALOME_ResourcesManager::ListAllResourcesInCatalogContainer
 */
void SALOME_ResourcesManager::ListAllAvailableResourcesContainer(Engines::ResourceList_out machines, Engines::IntegerList_out nbProcsOfMachines)
{
  const auto& zeList = _rm->ListAllResourcesInCatalogContainer();
  std::vector<std::string> ret0;
  std::vector<int> ret1;

  for(const auto& resource : zeList)
  {
    const auto& elt = resource.second;

    ret0.push_back(elt.hostname);
    ret1.push_back(elt.dataForSort.nb_node * elt.dataForSort.nb_proc_per_node);
  }

  machines = new Engines::ResourceList;
  nbProcsOfMachines = new Engines::IntegerList;
  std::size_t sz(ret0.size());
  machines->length((CORBA::ULong)sz);
  nbProcsOfMachines->length((CORBA::ULong)sz);

  for(std::size_t j = 0; j < sz; j++)
  {
    (*machines)[(CORBA::ULong)j] = CORBA::string_dup(ret0[j].c_str());
    (*nbProcsOfMachines)[(CORBA::ULong)j] = ret1[j];
  }
}

/*!
 * Return list of job resources available (regarding content of CatalogResources.xml).
 * 
 * \sa SALOME_ResourcesManager::ListAllAvailableResources
 */
Engines::ResourceList *SALOME_ResourcesManager::ListAllResourcesInCatalogJob()
{
  const auto& zeList = _rm->ListAllResourcesInCatalogJob();
  return ListAllResourcesInCatalog(zeList);
}

/*!
 * Return list of container resources available (regarding content of CatalogResources.xml).
 * 
 * \sa SALOME_ResourcesManager::ListAllAvailableResources
 */
Engines::ResourceList *SALOME_ResourcesManager::ListAllResourcesInCatalogContainer()
{
  const auto& zeList = _rm->ListAllResourcesInCatalogContainer();
  return ListAllResourcesInCatalog(zeList);
}


//=============================================================================
/*! CORBA method:
 *  shutdown all the containers, then the ContainerManager servant
 */
//=============================================================================

void SALOME_ResourcesManager::Shutdown()
{
  MESSAGE("Shutdown");
  if(!_NS)
    return ;
  _NS->Destroy_Name(_ResourcesManagerNameInNS);
  PortableServer::ObjectId_var oid = _poa->servant_to_id(this);
  _poa->deactivate_object(oid);
}
