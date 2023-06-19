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

#include "SALOME_ResourcesManager_Client.hxx"
#include "SALOME_ResourcesManager.hxx"
#include "SALOME_NamingService_Abstract.hxx"


SALOME_ResourcesManager_Client::SALOME_ResourcesManager_Client(SALOME_NamingService_Abstract *ns)
  : _rm(Engines::ResourcesManager::_nil())
{
  CORBA::Object_var objRM = ns->Resolve(SALOME_ResourcesManager::_ResourcesManagerNameInNS);
  if (!CORBA::is_nil(objRM))
    _rm = Engines::ResourcesManager::_narrow(objRM);
  if (CORBA::is_nil(_rm))
    throw SALOME_Exception("Cannot get SALOME ResourcesManager");
}

SALOME_ResourcesManager_Client::~SALOME_ResourcesManager_Client()
{
  
}

std::string SALOME_ResourcesManager_Client::Find(const std::string& policy, const ResourceList& possibleContainerResources) const
{
  Engines::ResourceList_var corba_rl = resourceList_CPPtoCORBA(possibleContainerResources);
  CORBA::String_var corba_res = _rm->Find(policy.c_str(), corba_rl);

  return corba_res.in();
}

ResourceList SALOME_ResourcesManager_Client::GetFittingResourcesJob(const resourceParamsJob& params) const
{
  Engines::ResourceParametersJob_var corba_params = resourceParametersJob_CPPtoCORBA(params);
  Engines::ResourceList_var corba_rl = _rm->GetFittingResourcesJob(corba_params);

  return resourceList_CORBAtoCPP(corba_rl);
}

ResourceList SALOME_ResourcesManager_Client::GetFittingResourcesContainer(const resourceParamsContainer& params) const
{
  Engines::ResourceParametersContainer_var corba_params = resourceParametersContainer_CPPtoCORBA(params);
  Engines::ResourceList_var corba_rl = _rm->GetFittingResourcesContainer(corba_params);

  return resourceList_CORBAtoCPP(corba_rl);
}

ParserResourcesTypeJob SALOME_ResourcesManager_Client::GetResourceDefinitionJob(const std::string& name) const
{
  const auto corba_res = _rm->GetResourceDefinitionJob(name.c_str());
  return resourceDefinitionJob_CORBAtoCPP(*corba_res);
}

ParserResourcesTypeContainer SALOME_ResourcesManager_Client::GetResourceDefinitionContainer(const std::string & name) const
{
  const auto corba_res = _rm->GetResourceDefinitionContainer(name.c_str());
  return resourceDefinitionContainer_CORBAtoCPP(*corba_res);
}

std::string SALOME_ResourcesManager_Client::getMachineFile(const std::string& resource_name,
                                                      long nb_procs,
                                                      const std::string& parallelLib)
{
  CORBA::String_var corba_res = _rm->getMachineFile(resource_name.c_str(),
                                                    nb_procs,
                                                    parallelLib.c_str());
  return corba_res.in();
}
