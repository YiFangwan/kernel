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

#include "SALOMESDS_DataScopeServer.hxx"
#include "SALOMESDS_StringDataServer.hxx"
#include "SALOMESDS_AnyDataServer.hxx"
#include "SALOMESDS_Exception.hxx"

#include <sstream>
#include <algorithm>

using namespace SALOMESDS;

DataScopeServer::DataScopeServer(CORBA::ORB_ptr orb, const std::string& scopeName):_orb(CORBA::ORB::_duplicate(orb)),_name(scopeName)
{
}

DataScopeServer::DataScopeServer(const DataScopeServer& other):RefCountServ(other),_name(other._name),_vars(other._vars)
{
}

/*!
 * Called remotely -> to protect against throw
 */
char *DataScopeServer::getScopeName()
{
  return CORBA::string_dup(_name.c_str());
}

/*!
 * Called remotely -> to protect against throw
 */
SALOME::StringDataServer_ptr DataScopeServer::createGlobalStringVar(const char *varName)
{
  std::string varNameCpp(varName);
  std::vector<std::string> allNames(getAllVarNames());
  std::vector<std::string>::iterator it(std::find(allNames.begin(),allNames.end(),varNameCpp));
  if(it!=allNames.end())
    {
      std::ostringstream oss; oss << "DataScopeServer::createGlobalStringVar : name \"" << varNameCpp << "\" already exists !";
      throw Exception(oss.str());
    }
  AutoRefCountPtr<StringDataServer> tmp(new StringDataServer(varNameCpp));
  _vars.push_back(DynamicCastSafe<StringDataServer,BasicDataServer>(tmp));
  CORBA::Object_var ret(activateWithDedicatedPOA(tmp));
  return SALOME::StringDataServer::_narrow(ret);
}

/*!
 * Called remotely -> to protect against throw
 */
SALOME::AnyDataServer_ptr DataScopeServer::createGlobalAnyVar(const char *varName)
{
  std::string varNameCpp(varName);
  std::vector<std::string> allNames(getAllVarNames());
  std::vector<std::string>::iterator it(std::find(allNames.begin(),allNames.end(),varNameCpp));
  if(it!=allNames.end())
    {
      std::ostringstream oss; oss << "DataScopeServer::createGlobalAnyVar : name \"" << varNameCpp << "\" already exists !";
      throw Exception(oss.str());
    }
  AutoRefCountPtr<AnyDataServer> tmp(new AnyDataServer(varNameCpp));
  _vars.push_back(DynamicCastSafe<AnyDataServer,BasicDataServer>(tmp));
  CORBA::Object_var ret(activateWithDedicatedPOA(tmp));
  return SALOME::AnyDataServer::_narrow(ret);
}

std::vector< std::string > DataScopeServer::getAllVarNames() const
{
  std::size_t sz(_vars.size());
  std::vector<std::string> ret(sz);
  std::list< AutoRefCountPtr<BasicDataServer> >::const_iterator it(_vars.begin());
  for(std::size_t i=0;i<sz;it++,i++)
    ret[i]=(*it)->getVarNameCpp();
  return ret;
}

CORBA::Object_var DataScopeServer::activateWithDedicatedPOA(BasicDataServer *ds)
{
  CORBA::Object_var obj(_orb->resolve_initial_references("RootPOA"));
  PortableServer::POA_var rootPoa;
  if(!CORBA::is_nil(obj))
    rootPoa=PortableServer::POA::_narrow(obj);
  PortableServer::POAManager_var pman(rootPoa->the_POAManager());
  CORBA::PolicyList policies;
  policies.length(1);
  PortableServer::ThreadPolicy_var threadPol(rootPoa->create_thread_policy(PortableServer::SINGLE_THREAD_MODEL));
  policies[0]=PortableServer::ThreadPolicy::_duplicate(threadPol);
  PortableServer::POA_var poa(rootPoa->create_POA("SingleThPOA4SDS",pman,policies));
  threadPol->destroy();
  //
  PortableServer::ObjectId_var id(poa->activate_object(ds));
  CORBA::Object_var ret(poa->id_to_reference(id));
  return ret;
}
