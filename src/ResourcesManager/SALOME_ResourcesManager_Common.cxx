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
// See https://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "SALOME_ResourcesManager_Common.hxx"

template <class T>
ResourceList strvec_CORBAtoCPP(const T& strvecCorba)
{
  ResourceList strvecCpp;
  for (unsigned int i = 0; i < strvecCorba.length(); i++)
    strvecCpp.push_back(std::string(strvecCorba[i]));

  return strvecCpp;
}

template <class T>
typename T::_var_type strvec_CPPtoCORBA(const ResourceList& strvecCpp)
{
  typename T::_var_type strvecCorba = new T;

  strvecCorba->length((CORBA::ULong)strvecCpp.size());
  for (unsigned int i = 0; i < strvecCpp.size(); i++)
    strvecCorba[i] = strvecCpp[i].c_str();

  return strvecCorba;
}

template <class T, class U>
U resourceParameters_CORBAtoCPP(const T& params)
{
  U p;

  p.name = params.name;
  p.hostname = params.hostname;
  p.resourceList = resourceList_CORBAtoCPP(params.resList);

  return p;

  // return
  // {{ // resourceParams base class members:
  //     params.name,
  //     params.hostname,
  //     resourceList_CORBAtoCPP(params.resList)
  // }};
}

resourceParamsJob resourceParametersJob_CORBAtoCPP(const Engines::ResourceParametersJob& params)
{
  return resourceParameters_CORBAtoCPP<Engines::ResourceParametersJob, resourceParamsJob>(params);
}

resourceParamsContainer resourceParametersContainer_CORBAtoCPP(const Engines::ResourceParametersContainer& params)
{
  resourceParamsContainer p = resourceParameters_CORBAtoCPP<Engines::ResourceParametersContainer, resourceParamsContainer>(params);

  // Specific containers' params
  p.OS = params.OS;
  p.nb_proc = params.nb_proc;
  p.nb_node = params.nb_node;
  p.nb_proc_per_node = params.nb_proc_per_node;
  p.cpu_clock = params.cpu_clock;
  p.mem_mb = params.mem_mb;
  p.componentList = strvec_CORBAtoCPP<Engines::CompoList>(params.componentList);

  return p;
}

template <class T, class U>
void resourceParameters_CPPtoCORBA(const T& params, U& p)
{
  p->name = params.name.c_str();
  p->hostname = params.hostname.c_str();
  p->resList = resourceList_CPPtoCORBA(params.resourceList);
}

Engines::ResourceParametersJob_var resourceParametersJob_CPPtoCORBA(const resourceParamsJob& params)
{
  Engines::ResourceParametersJob_var p = new Engines::ResourceParametersJob;

  resourceParameters_CPPtoCORBA<resourceParamsJob, Engines::ResourceParametersJob_var>(params, p);

  return p;
}

Engines::ResourceParametersContainer_var resourceParametersContainer_CPPtoCORBA(const resourceParamsContainer& params)
{
  Engines::ResourceParametersContainer_var p = new Engines::ResourceParametersContainer;

  // Common params
  resourceParameters_CPPtoCORBA<resourceParamsContainer, Engines::ResourceParametersContainer_var>(params, p);

  // Specific containers' params
  p->OS = params.OS.c_str();
  p->nb_proc = params.nb_proc;
  p->nb_node = params.nb_node;
  p->nb_proc_per_node = params.nb_proc_per_node;
  p->cpu_clock = params.cpu_clock;
  p->mem_mb = params.mem_mb;
  p->componentList = strvec_CPPtoCORBA<Engines::CompoList>(params.componentList);

  return p;
}

ResourceList resourceList_CORBAtoCPP(const Engines::ResourceList& resList)
{
  return strvec_CORBAtoCPP<Engines::ResourceList>(resList);
}

Engines::ResourceList_var resourceList_CPPtoCORBA(const ResourceList& resList)
{
  return strvec_CPPtoCORBA<Engines::ResourceList>(resList);
}

template <class T, class U>
U resourceDefinition_CORBAtoCPP(const T& resDef)
{
  U resource;

  resource.name = resDef.name;
  resource.hostname = resDef.hostname;
  resource.setAccessProtocolTypeStr(resDef.protocol.in());
  resource.username = resDef.username;
  resource.applipath = resDef.applipath;
  resource.setBatchTypeStr(resDef.batch.in());

  return resource;
}

ParserResourcesTypeJob resourceDefinitionJob_CORBAtoCPP(const Engines::ResourceDefinitionJob& resDef)
{
  ParserResourcesTypeJob resource =
      resourceDefinition_CORBAtoCPP<Engines::ResourceDefinitionJob, ParserResourcesTypeJob>(resDef);

  // Specific job's params
  resource.setMpiImplTypeStr(resDef.mpiImpl.in());
  resource.setClusterInternalProtocolStr(resDef.iprotocol.in());
  resource.working_directory = resDef.working_directory;

  return resource;
}

ParserResourcesTypeContainer resourceDefinitionContainer_CORBAtoCPP(const Engines::ResourceDefinitionContainer& resDef)
{
  ParserResourcesTypeContainer resource =
      resourceDefinition_CORBAtoCPP<Engines::ResourceDefinitionContainer, ParserResourcesTypeContainer>(resDef);

  // Specific container's params
  resource.OS = resDef.OS;
  resource.dataForSort.name = resDef.name;
  resource.dataForSort.mem_mb = resDef.mem_mb;
  resource.dataForSort.cpu_clock = resDef.cpu_clock;
  resource.dataForSort.nb_node = resDef.nb_node;
  resource.dataForSort.nb_proc_per_node = resDef.nb_proc_per_node;
  resource.componentList = strvec_CORBAtoCPP<Engines::CompoList>(resDef.componentList);

  return resource;
}

template <class T, class U>
void resourceDefinition_CPPtoCORBA(const T& resource, U& resCorba)
{
  // Common params
  resCorba->name = resource.name.c_str();
  resCorba->hostname = resource.hostname.c_str();
  resCorba->protocol = resource.getAccessProtocolTypeStr().c_str();
  resCorba->username = resource.username.c_str();
  resCorba->applipath = resource.applipath.c_str();
  resCorba->batch = resource.getBatchTypeStr().c_str();
}

Engines::ResourceDefinitionJob_var resourceDefinitionJob_CPPtoCORBA(const ParserResourcesTypeJob& resource)
{
  Engines::ResourceDefinitionJob_var resCorba = new Engines::ResourceDefinitionJob;

  // Common params
  resourceDefinition_CPPtoCORBA(resource, resCorba);

  // Specific job's params
  resCorba->mpiImpl = resource.getMpiImplTypeStr().c_str();
  resCorba->iprotocol = resource.getClusterInternalProtocolStr().c_str();
  resCorba->working_directory = resource.working_directory.c_str();

  return resCorba;
}

Engines::ResourceDefinitionContainer_var resourceDefinitionContainer_CPPtoCORBA(const ParserResourcesTypeContainer& resource)
{
  Engines::ResourceDefinitionContainer_var resCorba = new Engines::ResourceDefinitionContainer;

  // Common params
  resourceDefinition_CPPtoCORBA(resource, resCorba);

  // Specific container's params
  resCorba->componentList = strvec_CPPtoCORBA<Engines::CompoList>(resource.componentList);
  resCorba->OS = resource.OS.c_str();
  resCorba->mem_mb = resource.dataForSort.mem_mb;
  resCorba->cpu_clock = resource.dataForSort.cpu_clock;
  resCorba->nb_proc_per_node = resource.dataForSort.nb_proc_per_node;
  resCorba->nb_node = resource.dataForSort.nb_node;

  return resCorba;
}
