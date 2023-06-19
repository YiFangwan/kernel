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

//  SALOME ResourcesCatalog : implementation of catalog resources parsing (SALOME_ModuleCatalog.idl)
//  File   : SALOME_ResourcesCatalog_Handler.cxx
//  Author : Estelle Deville, Konstantin Leontev
//  Module : SALOME
//$Header$
//
#include "SALOME_ResourcesCatalog_Handler.hxx"

#include "Basics_Utils.hxx"
#include "utilities.h"

namespace
{
  // XML tags
  // ----------------------------------------------------

  // Doc's root
  const xmlChar* tag_root = BAD_CAST "resources";

  // Resources types roots
  const xmlChar* tag_resources_job = BAD_CAST "resources_job";
  const xmlChar* tag_resources_container = BAD_CAST "resources_container";

  // Tags for each resource
  const xmlChar* tag_machine = BAD_CAST "machine";
  const xmlChar* tag_cluster = BAD_CAST "cluster";
  const xmlChar* tag_members = BAD_CAST "members";

  // Tags for container resources
  const xmlChar* tag_component = BAD_CAST "component";
  const xmlChar* tag_modules = BAD_CAST "modules"; // module == component

  // Attributes
  // ----------------------------------------------------

  // Common attributes for each type of resource
  const xmlChar* attr_name = BAD_CAST "name";
  const xmlChar* attr_hostname = BAD_CAST "hostname";
  const xmlChar* attr_protocol = BAD_CAST "protocol";
  const xmlChar* attr_username = BAD_CAST "username";
  const xmlChar* attr_applipath = BAD_CAST "applipath";
  const xmlChar* attr_batch = BAD_CAST "batch";

  // Job's attributes
  const xmlChar* attr_mpiImpl = BAD_CAST "mpi";
  const xmlChar* attr_iprotocol = BAD_CAST "iprotocol";
  const xmlChar* attr_working_directory = BAD_CAST "workingDirectory";

  // Container's attributes
  const xmlChar* attr_component_name = BAD_CAST "name";
  const xmlChar* attr_os = BAD_CAST "OS";
  const xmlChar* attr_mem_mb = BAD_CAST "memInMB";
  const xmlChar* attr_cpu_clock = BAD_CAST "CPUFreqMHz";
  const xmlChar* attr_nb_node = BAD_CAST "nbOfNodes";
  const xmlChar* attr_nb_proc_per_node = BAD_CAST "nbOfProcPerNode";
  const xmlChar* attr_nb_of_proc = BAD_CAST "nbOfProc";
  const xmlChar* attr_module_name = BAD_CAST "moduleName";

  // for compatibility
  const xmlChar* tag_batch_queue = BAD_CAST "batchQueue";
  const xmlChar* tag_user_commands = BAD_CAST "userCommands";
  const xmlChar* tag_use = BAD_CAST "use";


  // Generic functions for common processing of all resources' types
  // ----------------------------------------------------

  // Reads common attributes from machine tag
  template <class T> bool readMachineCommon(xmlNodePtr node, T& resource)
  {
    MESSAGE("Read machine...");

    // This alias is only to use short name for readAttr() method
    auto readAttr = &SALOME_ResourcesCatalog_Handler::readAttr;

    resource.name = readAttr(node, attr_name);

    resource.hostname = readAttr(node, attr_hostname);
    if (resource.hostname.empty())
    {
      MESSAGE("Warning! Hostname is empty. This machine will not be added.");
      return false;
    }

    // Use the hostname as a name if the name wasn't provided
    if (resource.name.empty())
    {
      MESSAGE("Resourece name is empty. Using hostname as a name: " << resource.hostname);
      resource.name = resource.hostname;
    }

    resource.username = readAttr(node, attr_username);
    resource.applipath = readAttr(node, attr_applipath);

    try
    {
      resource.setAccessProtocolTypeStr(readAttr(node, attr_protocol));
    }
    catch(const ResourcesException& e)
    {
      MESSAGE("Warning! " << e.msg << " Using default value.");
    }

    try
    {
      resource.setBatchTypeStr(readAttr(node, attr_batch));
    }
    catch(const ResourcesException& e)
    {
      MESSAGE("Warning! " << e.msg << " Using default value.");
    }

    // Check if those below are from a common part
    resource.batchQueue = readAttr(node, tag_batch_queue);
    resource.userCommands = readAttr(node, tag_user_commands);
    resource.use = readAttr(node, tag_use);

    return true;
  }

  // Add resource to a given map.
  template <class T, typename U> void addResourceToMap(T& resource, U& resourceMap)
  {
    if(resource.hostname == "localhost")
    {
      resource.hostname = Kernel_Utils::GetHostname();
    }

    const auto iter = resourceMap.find(resource.name);
    if (iter != resourceMap.end())
    {
      MESSAGE("Warning! Resource " << resource.name << " already added, keep last resource found!");
    }

    resourceMap[resource.name] = resource;
    MESSAGE("Added resource: " << resource.name);
  }

  // Writes common attributes to machine tag
  template <class T> void writeMachineCommon(xmlNodePtr node, const T& resource)
  {
    MESSAGE("Write machine...");

    xmlNewProp(node, attr_name, BAD_CAST resource.name.c_str());
    xmlNewProp(node, attr_hostname, BAD_CAST resource.hostname.c_str());
    xmlNewProp(node, attr_protocol, BAD_CAST resource.getAccessProtocolTypeStr().c_str());
    xmlNewProp(node, attr_username, BAD_CAST resource.username.c_str());
    xmlNewProp(node, attr_applipath, BAD_CAST resource.applipath.c_str());
    xmlNewProp(node, attr_batch, BAD_CAST resource.getBatchTypeStr().c_str());

    // Check if those below are from a common part
    xmlNewProp(node, tag_batch_queue, BAD_CAST resource.batchQueue.c_str());
    xmlNewProp(node, tag_user_commands, BAD_CAST resource.userCommands.c_str());
    xmlNewProp(node, tag_use, BAD_CAST resource.use.c_str());
  }
}

//=============================================================================
/*!
 *  Constructor
 *  \param resourcesJob: map of ParserResourcesTypeJob to fill when parsing
 *  \param resourcesContainer: map of ParserResourcesTypeContainer to fill when parsing
 */ 
//=============================================================================

SALOME_ResourcesCatalog_Handler::SALOME_ResourcesCatalog_Handler(
  ParserResourcesTypeJob::TypeMap& resourcesJob,
  ParserResourcesTypeContainer::TypeMap& resourcesContainer) :
  resourcesJob(resourcesJob),
  resourcesContainer(resourcesContainer)
{
}

//=============================================================================
/*!
 *  Destructor
 */ 
//=============================================================================

SALOME_ResourcesCatalog_Handler::~SALOME_ResourcesCatalog_Handler()
{
  //  cout << "SALOME_ResourcesCatalog_Handler destruction") << endl;
}

//=============================================================================
/*!
 *  Retrieves job resources map after the file parse.
 */ 
//=============================================================================

const ParserResourcesTypeJob::TypeMap&
SALOME_ResourcesCatalog_Handler::getResourcesJob() const
{
  return resourcesJob;
}

//=============================================================================
/*!
 *  Retrieves container resources map after the file parse.
 */ 
//=============================================================================

const ParserResourcesTypeContainer::TypeMap&
SALOME_ResourcesCatalog_Handler::getResourcesContainer() const
{
  return resourcesContainer;
}

//=============================================================================
/*!
 *  Processes XML document and fills the list of resources
 *  \param theDoc: a pointer to XML doc to read from
 */ 
//=============================================================================

void SALOME_ResourcesCatalog_Handler::readXmlDoc(xmlDocPtr theDoc)
{
  // Empty private elements
  resourcesJob.clear();
  resourcesContainer.clear();

  // Get the document root node
  xmlNodePtr node = xmlDocGetRootElement(theDoc);
  if (!node)
  {
    MESSAGE("Warning! Couldn't find a root element in XML doc. Reading of resources was stopped.");
    return;
  }

  if (xmlStrcmp(node->name, tag_root))
  {
    MESSAGE("Warning! Wrong name of the root element in XML doc. Expected: " << tag_root << ", but given: " << node->name);
  }

  node = node->xmlChildrenNode;
 
  // Processing the document nodes
  while(node)
  {
    // Check if we have different type of resources here
    if (!xmlStrcmp(node->name, tag_resources_job))
    {
      readNodeResourcesJob(node);
    }
    else if(!xmlStrcmp(node->name, tag_resources_container))
    {
      readNodeResourcesContainer(node);
    }
    // else if(!xmlStrcmp(node->name, tag_cluster))
    // {
    //   readNodeCluster(node);
    // }
    else
    {
      // We don't expect other types of elements on this level
      MESSAGE("Warning! Unexpected name of the element under root level. Skip element: " << node->name);
    }

    node = node->next;
  }

// Commented code below leaved temprarly for previouse implementation reference.

//     // Declaration of a single machine or a frontal node for a cluster managed by a batch manager
//     if (!xmlStrcmp(node->name, tag_machine))
//     {
//       ParserResourcesType resource;
//       bool Ok = ProcessMachine(aCurNode, resource);
//       if (Ok)
//       {
//         // Adding a resource
//         if(resource.HostName == "localhost")
//         {
//           resource.HostName = Kernel_Utils::GetHostname();
//         }
//         std::map<std::string, ParserResourcesType>::const_iterator iter = _resources_list.find(resource.Name);
//         if (iter != _resources_list.end())
//           MESSAGE("Warning! Resource " << resource.Name << " already added, keep last resource found!");
//         _resources_list[resource.Name] = resource;
//       }
//     }
//     // Declaration of a cluster
//     // Here, a cluster is NOT the frontal node of a cluster managed by a batch manager (classical
//     // usage of a cluster). It is a group of machines intended to be used for a parallel container.
//     // The methods ProcessCluster and ProcessMember are only used in the context of parallel
//     // containers. They are not used in classical Salome usage scenarios.
//     if (!xmlStrcmp(aCurNode->name, tag_cluster))
//     {
//       ParserResourcesType resource;
//       if(ProcessCluster(aCurNode, resource))
//       {
//         std::map<std::string, ParserResourcesType>::const_iterator iter = _resources_list.find(resource.Name);
//         if (iter != _resources_list.end())
//           MESSAGE("Warning! Resource " << resource.Name << " already added, keep last resource found!");
//         _resources_list[resource.Name] = resource;
//       }
//     }
//     aCurNode = aCurNode->next;
//   }

// #ifdef _DEBUG_
//   for (std::map<std::string, ParserResourcesType>::const_iterator iter = _resources_list.begin();
//        iter != _resources_list.end();
//        iter++)
//   {
//     MESSAGE("************************************************");
//     MESSAGE("Resource " << (*iter).first << " found:");
//     MESSAGE( (*iter).second );
//     MESSAGE("************************************************");
//   }
// #endif
}

//=============================================================================
/*!
 *  Fill the document tree in xml file, used to write in an xml file.
 *  \param theDoc document to fill.
 */ 
//=============================================================================

void SALOME_ResourcesCatalog_Handler::writeXmlDoc(xmlDocPtr theDoc) const
{
  // Set root
  xmlNodePtr root_node = xmlNewNode(NULL, tag_root);
  xmlDocSetRootElement(theDoc, root_node);

  // Write resources
  writeNodeResourcesJob(root_node);
  writeNodeResourcesContainer(root_node);
}

//=============================================================================
/*!
 *  Reads all machines inside job resources tag.
 *  \param node: a pointer to node to read from
 */ 
//=============================================================================
void SALOME_ResourcesCatalog_Handler::readNodeResourcesJob(xmlNodePtr node)
{
  MESSAGE("Read job resources...");

  node = node->xmlChildrenNode;
  while(node)
  {
    if (!xmlStrcmp(node->name, tag_machine))
    {
      ParserResourcesTypeJob resource;
      if (readMachineJob(node, resource))
      {
        addResourceToMap(resource, resourcesJob);
      }
    }
    else
    {
      MESSAGE("Warning! Unexpected name of the child element: " << node->name);
    }

    node = node->next;
  }
}

//=============================================================================
/*!
 *  Reads all machines inside container resources tag.
 *  \param node: a pointer to node to read from
 */ 
//=============================================================================
void SALOME_ResourcesCatalog_Handler::readNodeResourcesContainer(xmlNodePtr node)
{
  MESSAGE("Read container resources...");

  node = node->xmlChildrenNode;
  while(node)
  {
    if (!xmlStrcmp(node->name, tag_machine))
    {
      ParserResourcesTypeContainer resource;
      if (readMachineContainer(node, resource))
      {
        addResourceToMap(resource, resourcesContainer);
      }
    }
    else
    {
      MESSAGE("Warning! Unexpected name of the child element: " << node->name);
    }

    node = node->next;
  }
}

//=============================================================================
/*!
 *  Reads all attributes of job resource machine.
 *  \param node: a pointer to node to read modules from
 *  \param resource: a reference to job resource type object
 */ 
//=============================================================================
bool SALOME_ResourcesCatalog_Handler::readMachineJob(xmlNodePtr node, ParserResourcesTypeJob& resource) const
{
  // Read common attributes
  if (!readMachineCommon(node, resource))
  {
    return false;
  }

  // Read specific job attributes
  try
  {
    resource.setMpiImplTypeStr(readAttr(node, attr_mpiImpl));
  }
  catch (const ResourcesException& e)
  {
    MESSAGE(e.msg << " Using default value.");
  }

  try
  {
    resource.setClusterInternalProtocolStr(readAttr(node, attr_iprotocol));
  }
  catch (const ResourcesException& e)
  {
    MESSAGE(e.msg << " Using default value.");
  }


  resource.working_directory = readAttr(node, attr_working_directory);

  return true;
}

//=============================================================================
/*!
 *  Reads all attributes of container resource machine.
 *  It's a single machine or a frontal node for a cluster managed by a batch manager
 *  \param node: a pointer to node to read modules from
 *  \param resource: a reference to container resource type object
 */ 
//=============================================================================
bool SALOME_ResourcesCatalog_Handler::readMachineContainer(xmlNodePtr node, ParserResourcesTypeContainer& resource) const
{
  // Read common attributes
  if (!readMachineCommon(node, resource))
  {
    return false;
  }

  // Read specific job attributes
  // TODO: decide if we accept empty values here
  resource.OS = readAttr(node, attr_os);

  // Overrides a given default value only if we can get a valid conversion to int
  auto readAttrInt = [&](const xmlChar* attrStr, unsigned int& attrValueOut) -> void
  {
    const std::string attrValueStr = readAttr(node, attrStr);
    if (attrValueStr.empty())
    {
      return;
    }

    try
    {
      attrValueOut = std::stoi(attrValueStr);
    }
    catch(const std::exception& e)
    {
      MESSAGE(e.what());
    }
  };

  resource.dataForSort.name = resource.name;
  readAttrInt(attr_mem_mb, resource.dataForSort.mem_mb);
  readAttrInt(attr_cpu_clock, resource.dataForSort.cpu_clock);
  readAttrInt(attr_nb_node, resource.dataForSort.nb_node);
  readAttrInt(attr_nb_proc_per_node, resource.dataForSort.nb_proc_per_node);
  readAttrInt(attr_nb_of_proc, resource.nbOfProc);

  // Read child nodes to get components and modules if we have any
  node = node->xmlChildrenNode;
  while(node)
  {
    readComponent(node, resource);
    readModule(node, resource);

    node = node->next;
  }

  return true;
}

//=============================================================================
/*!
 *  Reads all components of container resource machine.
 *  \param node: a pointer to node to read modules from
 *  \param resource: a reference to container resource type object
 */ 
//=============================================================================
void SALOME_ResourcesCatalog_Handler::readComponent(xmlNodePtr node, ParserResourcesTypeContainer& resource) const
{
  if (!xmlStrcmp(node->name, tag_component))
  {
    // If a component is given, it is in a module with the same name
    // except if the module name is given
    const std::string componentName = readAttr(node, attr_component_name);
    if (!componentName.empty())
    {
      resource.componentList.push_back(componentName);

      const std::string moduleName = readAttr(node, attr_module_name);
      if (!moduleName.empty())
      {
        resource.ModulesList.push_back(moduleName);
      }
      else
      {
        resource.ModulesList.push_back(componentName);
      }            
    }
  }
}

//=============================================================================
/*!
 *  Reads all modules of container resource machine.
 *  \param node: a pointer to node to read modules from
 *  \param resource: a reference to container resource type object
 */ 
//=============================================================================
void SALOME_ResourcesCatalog_Handler::readModule(xmlNodePtr node, ParserResourcesTypeContainer& resource) const
{
  if ( !xmlStrcmp(node->name, tag_modules) )
  {
    // If a module is given, we create an entry in componentsList and modulesList
    // with the same name (module == component)
    const std::string moduleName = readAttr(node, attr_module_name);

    if (!moduleName.empty())
    {
      resource.componentList.push_back(moduleName);
      resource.ModulesList.push_back(moduleName);
    }
  }
}

// //=============================================================================
// /*!
//  * Here, a cluster is NOT the frontal node of a cluster managed by a batch manager (classical
//  * usage of a cluster). It is a group of machines intended to be used for a parallel container.
//  * The methods readNodeClaster and readNodeClasterMember are only used in the context of parallel
//  * containers. They are not used in classical Salome usage scenarios.
//  *  \param node: a pointer to node to read from
//  */ 
// //=============================================================================
// void SALOME_ResourcesCatalog_Handler::readNodeCluster(xmlNodePtr node)
// {
//   // TODO: change implementation after we know if we need to process clusters as job or containers or both
// }

// //=============================================================================
// /*!
//  * Reads a member of a cluster
//  *  \param node: a pointer to node to read from
//  *  \param resource: a reference to container resource type object
//  */ 
// //=============================================================================
// bool SALOME_ResourcesCatalog_Handler::readNodeClusterMember(xmlNodePtr node, ParserResourcesTypeContainer& resource)
// {
//   // TODO: add implementation
//   return true;
// }

//=============================================================================
/*!
 *  Reads an attribute from XML node. Rases a warning if there's not any.
 *  It's on the caller side to decide if an attribute is mandatory or not.
 *  \param node: a pointer to node to read attribute from
 *  \param attr: an attribute's name
 */ 
//=============================================================================
std::string SALOME_ResourcesCatalog_Handler::readAttr(xmlNodePtr node, const xmlChar* attr)
{
  if (xmlHasProp(node, attr))
  {
    xmlChar* attrValue = xmlGetProp(node, attr);
    const std::string attrValueStr((const char*)attrValue);
    xmlFree(attrValue);

    return attrValueStr;
  }

  MESSAGE("Couldn't find an attribute: " << attr);
  return {};
}

//=============================================================================
/*!
 *  Write job resources as a child of a given node.
 *  \param node: a pointer to the parent node
 */ 
//=============================================================================
void SALOME_ResourcesCatalog_Handler::writeNodeResourcesJob(xmlNodePtr node) const
{
  node = xmlNewChild(node, NULL, tag_resources_job, NULL);
  for (const auto& res : resourcesJob)
  {
    writeMachineJob(node, res.second);
  }
}

//=============================================================================
/*!
 *  Write container resources as a child of a given node.
 *  \param node: a pointer to the parent node
 */ 
//=============================================================================
void SALOME_ResourcesCatalog_Handler::writeNodeResourcesContainer(xmlNodePtr node) const
{
  node = xmlNewChild(node, NULL, tag_resources_container, NULL);
  for (const auto& res : resourcesContainer)
  {
    writeMachineContainer(node, res.second);
  }  
}

//=============================================================================
/*!
 *  Write a given job resource as a child of a given node.
 *  \param node: a pointer to the parent node
 *  \param resource: a reference to the job resource type object
 */ 
//=============================================================================
void SALOME_ResourcesCatalog_Handler::writeMachineJob(xmlNodePtr node, const ParserResourcesTypeJob& resource) const
{
  // Common part
  MESSAGE("Add resource name = " << resource.name);

  node = xmlNewChild(node, NULL, tag_machine, NULL);
  writeMachineCommon(node, resource);

  // Specific job's attributes
  xmlNewProp(node, attr_mpiImpl, BAD_CAST resource.getMpiImplTypeStr().c_str());
  xmlNewProp(node, attr_iprotocol, BAD_CAST resource.getClusterInternalProtocolStr().c_str());
  xmlNewProp(node, attr_working_directory, BAD_CAST resource.working_directory.c_str());
}

//=============================================================================
/*!
 *  Write a given container resource as a child of a given node.
 *  \param node: a pointer to the parent node
 *  \param resource: a reference to the container resource type object
 */ 
//=============================================================================
void SALOME_ResourcesCatalog_Handler::writeMachineContainer(xmlNodePtr node, const ParserResourcesTypeContainer& resource) const
{
  // Common part
  MESSAGE("Add resource name = " << resource.name);

  node = xmlNewChild(node, NULL, tag_machine, NULL);
  writeMachineCommon(node, resource);

  // Specific container's attributes
  xmlNewProp(node, attr_os, BAD_CAST resource.OS.c_str());
  xmlNewProp(node, attr_mem_mb, BAD_CAST std::to_string(resource.dataForSort.mem_mb).c_str());
  xmlNewProp(node, attr_cpu_clock, BAD_CAST std::to_string(resource.dataForSort.cpu_clock).c_str());
  xmlNewProp(node, attr_nb_node, BAD_CAST std::to_string(resource.dataForSort.nb_node).c_str());
  xmlNewProp(node, attr_nb_proc_per_node, BAD_CAST std::to_string(resource.dataForSort.nb_proc_per_node).c_str());
  xmlNewProp(node, attr_nb_of_proc, BAD_CAST std::to_string(resource.nbOfProc).c_str());

  writeComponents(node, resource);
}

//=============================================================================
/*!
 *  Write all the components of a given container resource as children of a given node.
 *  \param node: a pointer to the parent node
 *  \param resource: a reference to the container resource type object
 */ 
//=============================================================================
void SALOME_ResourcesCatalog_Handler::writeComponents(xmlNodePtr node, const ParserResourcesTypeContainer& resource) const
{
  for (const auto& component : resource.componentList)
  {
    xmlNodePtr nodeComp = xmlNewChild(node, NULL, tag_component, NULL);
    xmlNewProp(nodeComp, attr_component_name, BAD_CAST component.c_str());
  }
}


// All the commented code below should be deleted after decision on cluster reading.
// Leaved here for implementation reference. 

// bool
// SALOME_ResourcesCatalog_Handler::ProcessCluster(xmlNodePtr cluster_descr, ParserResourcesType & resource)
// {
//   // Ajout d'un cluster
//   // hostname, use et nbOfProc sont obligatoires
//   if (xmlHasProp(cluster_descr, attr_hostname))
//   {
//     xmlChar* hostname = xmlGetProp(cluster_descr, attr_hostname);
//     resource.HostName = (const char*)hostname;
//     xmlFree(hostname);
//   }
//   else
//   {
//     MESSAGE("Warning! Found a cluster without a hostname. This cluster will not be added.");
//     return false;
//   }

//   if (xmlHasProp(cluster_descr, attr_name))
//   {
//     xmlChar* name = xmlGetProp(cluster_descr, attr_name);
//     resource.Name = (const char*)name;
//     resource.DataForSort._Name = (const char*)name;
//     xmlFree(name);
//   }
//   else
//   {
//     resource.Name = resource.HostName;
//     resource.DataForSort._Name = resource.HostName;
//     MESSAGE("Warning! No Name found use Hostname for resource: " << resource.Name );
//   }

//   if (xmlHasProp(cluster_descr, tag_use))
//   {
//     xmlChar* use = xmlGetProp(cluster_descr, tag_use);
//     resource.use = (const char*)use;
//     xmlFree(use);
//   }
//   else
//   {
//     MESSAGE("Warning! Found a cluster without a use. This cluster will not be added.");
//     return false;
//   }

//   if (xmlHasProp(cluster_descr, attr_nb_of_proc))
//   {
//     xmlChar* nb_of_proc = xmlGetProp(cluster_descr, attr_nb_of_proc);
//     resource.nbOfProc = atoi((const char*)nb_of_proc);
//     xmlFree(nb_of_proc);
//   }
//   else
//   {
//     MESSAGE("Warning! Found a cluster without a nbOfProc. This cluster will not be added.");
//     return false;
//   }

//   if (xmlHasProp(cluster_descr, attr_mpiImpl))
//   {
//     xmlChar* mpi = xmlGetProp(cluster_descr, attr_mpiImpl);
//     std::string anMpi = (const char*)mpi;
//     xmlFree(mpi);
//     resource.setMpiImplTypeStr(anMpi);
//   }

//   // Parsing des membres du cluster 
//   xmlNodePtr aCurSubNode = cluster_descr->xmlChildrenNode;
//   while(aCurSubNode != NULL)
//   {
//     if (!xmlStrcmp(aCurSubNode->name, tag_members))
//     {
//        xmlNodePtr members = aCurSubNode->xmlChildrenNode;
//        while (members != NULL)
//        {
//          // Process members
//          if (!xmlStrcmp(members->name, tag_machine))
//          {
//            ParserResourcesType new_member;
//            if (ProcessMember(members, new_member))
//              resource.ClusterMembersList.push_back(new_member);
//          }
//          members = members->next;
//        }
//     }
//     aCurSubNode = aCurSubNode->next;
//   }

//   // Test: Il faut au moins un membre pour que le cluster soit correct !
//   if (resource.ClusterMembersList.empty())
//   {
//     MESSAGE("Warning! Found a cluster without a member. This cluster will not be added.");
//     return false;
//   }
//   return true;
// }

// bool
// SALOME_ResourcesCatalog_Handler::ProcessMember(xmlNodePtr member_descr, ParserResourcesType & resource)
// {
//   if (xmlHasProp(member_descr, attr_hostname))
//   {
//     xmlChar* hostname = xmlGetProp(member_descr, attr_hostname);
//     resource.HostName = (const char*)hostname;
//     xmlFree(hostname);
//   }
//   else
//   {
//     MESSAGE("Warning! Found a machine without a hostname. This machine will not be added.");
//     return false;
//   }

//   if (xmlHasProp(member_descr, attr_protocol))
//   {
//     xmlChar* protocol= xmlGetProp(member_descr, attr_protocol);
//     try
//     {
//       resource.setAccessProtocolTypeStr((const char *)protocol);
//     }
//     catch (const ResourcesException & )
//     {
//       MESSAGE("Warning! Found a machine with a bad protocol. This machine will not be added.");
//       return false;
//     }
//     xmlFree(protocol);
//   }
//   else
//   {
//     MESSAGE("Warning! Found a machine without a protocol. This machine will not be added.");
//     return false;
//   }

//   if (xmlHasProp(member_descr, attr_iprotocol))
//   {
//     xmlChar* iprotocol= xmlGetProp(member_descr, attr_iprotocol);
//     try
//     {
//       resource.setClusterInternalProtocolStr((const char *)iprotocol);
//     }
//     catch (const ResourcesException &)
//     {
//       MESSAGE("Warning! Found a machine with a bad protocol. This machine will not be added.");
//       return false;
//     }
//     xmlFree(iprotocol);
//   }
//   else
//   {
//     MESSAGE("Warning! Found a machine without a protocol. This machine will not be added.");
//     return false;
//   }

//   if (xmlHasProp(member_descr, attr_username))
//   {
//     xmlChar* user_name= xmlGetProp(member_descr, attr_username);
//     resource.UserName = (const char*)user_name;
//     xmlFree(user_name);
//   }
//   else
//   {
//     MESSAGE("Warning! Found a machine without a user name. This machine will not be added.");
//     return false;
//   }

//   if (xmlHasProp(member_descr, attr_nb_node))
//   {
//     xmlChar* nb_of_nodes = xmlGetProp(member_descr, attr_nb_node);
//     resource.DataForSort._nbOfNodes = atoi((const char*)nb_of_nodes);
//     xmlFree(nb_of_nodes);
//   }
//   else
//   {
//     MESSAGE("Warning! Found a machine without a nbOfNodes. This machine will not be added.");
//     return false;
//   }

//   if (xmlHasProp(member_descr, attr_nb_proc_per_node))
//   {
//     xmlChar* nb_of_proc_per_node = xmlGetProp(member_descr, attr_nb_proc_per_node);
//     resource.DataForSort._nbOfProcPerNode = atoi((const char*)nb_of_proc_per_node);
//     xmlFree(nb_of_proc_per_node);
//   }
//   else
//   {
//     MESSAGE("Warning! Found a machine without a nbOfProcPerNode. This machine will not be added.");
//     return false;
//   }

//   if (xmlHasProp(member_descr, attr_applipath))
//   {
//     xmlChar* appli_path = xmlGetProp(member_descr, attr_applipath);
//     resource.AppliPath = (const char*)appli_path;
//     xmlFree(appli_path);
//   }
//   else
//   {
//     MESSAGE("Warning! Found a machine without a AppliPath. This machine will not be added.");
//     return false;
//   }
//   return true;
// }
