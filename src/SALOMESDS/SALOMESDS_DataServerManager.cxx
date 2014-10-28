// Copyright (C) 2007-2014  CEA/DEN, EDF R&D, OPEN CASCADE
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
// Author : Anthony GEAY (EDF R&D)

#include "SALOMESDS_DataServerManager.hxx"
#include "SALOMESDS_Exception.hxx"

#include "SALOME_ContainerManager.hxx"
#include "SALOME_NamingService.hxx"

#include <sstream>

using namespace SALOMESDS;

const char DataServerManager::NAME_IN_NS[]="/DataServerManager";

const char DataServerManager::DFT_SCOPE_NAME_IN_NS[]="Default";

DataServerManager::DataServerManager(CORBA::ORB_ptr orb, PortableServer::POA_ptr poa):_dft_scope(new DataScopeServer(orb,DFT_SCOPE_NAME_IN_NS)),_orb(CORBA::ORB::_duplicate(orb))
{
  PortableServer::POAManager_var pman(poa->the_POAManager());
  CORBA::PolicyList policies;
  policies.length(1);
  PortableServer::ThreadPolicy_var threadPol(poa->create_thread_policy(PortableServer::SINGLE_THREAD_MODEL));
  policies[0]=PortableServer::ThreadPolicy::_duplicate(threadPol);
  _poa=poa->create_POA("SingleThPOA4SDS",pman,policies);
  threadPol->destroy();
  // activate this to be ready to be usable from NS.
  PortableServer::ObjectId_var id(_poa->activate_object(this));
  CORBA::Object_var obj(_poa->id_to_reference(id));
  SALOME::DataServerManager_var obj2(SALOME::DataServerManager::_narrow(obj));
  // publish Data server manager in NS
  SALOME_NamingService ns(orb);
  ns.Register(obj2,NAME_IN_NS);
  // the default DataScopeServer object is the only one hosted by the current process
  id=_poa->activate_object(_dft_scope);
  obj=_poa->id_to_reference(id);
  _ptr_dft_scope=SALOME::DataScopeServer::_narrow(obj);
  _scopes.push_back(_ptr_dft_scope);
  //
  std::string fullNameInNS(CreateAbsNameInNSFromScopeName(DFT_SCOPE_NAME_IN_NS));
  ns.Register(_ptr_dft_scope,fullNameInNS.c_str());
}

SALOME::StringVec *DataServerManager::listScopes()
{
  SALOME::StringVec *ret(new SALOME::StringVec);
  std::size_t sz(_scopes.size());
  ret->length(sz);
  std::list< SALOME::DataScopeServer_var >::iterator it(_scopes.begin());
  for(std::size_t i=0;i<sz;it++,i++)
    {
      SALOME::DataScopeServer_var obj(*it);
      char *name(obj->getScopeName());
      (*ret)[i]=CORBA::string_dup(name);
      CORBA::string_free(name);
    }
  return ret;
}

SALOME::DataScopeServer_ptr DataServerManager::getDefaultScope()
{
  return SALOME::DataScopeServer::_duplicate(_ptr_dft_scope);
}

SALOME::DataScopeServer_ptr DataServerManager::createDataScope(const char *scopeName)
{
  std::string scopeNameCpp(scopeName);
  std::size_t sz(_scopes.size());
  std::list< SALOME::DataScopeServer_var >::iterator it(_scopes.begin());
  for(std::size_t i=0;i<sz;it++,i++)
    {
      CORBA::String_var zeName((*it)->getScopeName());
      if(scopeNameCpp==(const char *)zeName)
        {
          std::ostringstream oss; oss << "DataServerManager::createDataScope : scope name \"" << scopeName << "\" already exists !";
          throw Exception(oss.str());
        }
    }
  //
  SALOME_NamingService ns(_orb);
  std::string fullScopeName(CreateAbsNameInNSFromScopeName(scopeName));
  std::ostringstream oss; oss << "SALOME_DataScopeServer" << " " << scopeName;
  SALOME_ContainerManager::AddOmninamesParams(oss,&ns);
  std::string command(oss.str());
  SALOME_ContainerManager::MakeTheCommandToBeLaunchedASync(command);
  int status(SALOME_ContainerManager::SystemThreadSafe(command.c_str()));
  int count(SALOME_ContainerManager::GetTimeOutToLoaunchServer());
  SALOME::DataScopeServer_var ret(SALOME::DataScopeServer::_nil());
  while (CORBA::is_nil(ret) && count)
    {
      CORBA::Object_var obj(ns.Resolve(fullScopeName.c_str()));
      ret=SALOME::DataScopeServer::_narrow(obj);
    }
  if(!CORBA::is_nil(ret))
    {
      _scopes.push_back(ret);
    }
  return SALOME::DataScopeServer::_duplicate(ret);
}

SALOME::DataScopeServer_ptr DataServerManager::retriveDataScope(const char *scopeName)
{
  std::list< SALOME::DataScopeServer_var >::iterator it(getScopePtrGivenName(scopeName));
  return SALOME::DataScopeServer::_duplicate(*it);
}

SALOME::DataScopeServer_ptr DataServerManager::removeDataScope(const char *scopeName)
{
  std::list< SALOME::DataScopeServer_var >::iterator it(getScopePtrGivenName(scopeName));
  return SALOME::DataScopeServer::_duplicate(*it);
}

std::string DataServerManager::CreateAbsNameInNSFromScopeName(const std::string& scopeName)
{
  std::ostringstream oss; oss << NAME_IN_NS << "/" << scopeName;
  return oss.str();
}

std::list< SALOME::DataScopeServer_var >::iterator DataServerManager::getScopePtrGivenName(const std::string& scopeName)
{
  std::size_t sz(_scopes.size());
  std::list< SALOME::DataScopeServer_var >::iterator it(_scopes.begin());
  bool found(false);
  for(std::size_t i=0;i<sz;it++,i++)
    {
      CORBA::String_var zeName((*it)->getScopeName());
      found=(scopeName==(const char *)zeName);
      if(found)
        break;
    }
  if(!found)
    {
      std::ostringstream oss; oss << "DataServerManager::getScopePtrGivenName : scope name \"" << scopeName << "\" does not exist !";
      throw Exception(oss.str());
    }
  return it;
}
