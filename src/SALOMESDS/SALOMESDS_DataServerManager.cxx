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
  _scopes.push_back(_dft_scope);
  //
  std::string fullNameInNS(CreateAbsNameInNSFromScopeName(DFT_SCOPE_NAME_IN_NS));
  ns.Register(_ptr_dft_scope,fullNameInNS.c_str());
}

SALOME::DataScopeServer_ptr DataServerManager::getDefaultScope()
{
  return SALOME::DataScopeServer::_duplicate(_ptr_dft_scope);
}

SALOME::DataScopeServer_ptr DataServerManager::createDataScope(const char *scopeName)
{
}

SALOME::DataScopeServer_ptr DataServerManager::retriveDataScope(const char *scopeName)
{
}

SALOME::DataScopeServer_ptr DataServerManager::removeDataScope(const char *scopeName)
{
}

std::string DataServerManager::CreateAbsNameInNSFromScopeName(const std::string& scopeName)
{
  std::ostringstream oss; oss << NAME_IN_NS << "/" << scopeName;
  return oss.str();
}
