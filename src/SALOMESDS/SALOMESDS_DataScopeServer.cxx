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
#include "SALOMESDS_DataServerManager.hxx"
#include "SALOMESDS_StringDataServer.hxx"
#include "SALOMESDS_AnyDataServer.hxx"
#include "SALOME_NamingService.hxx"
#include "SALOMESDS_Exception.hxx"

#include <sstream>
#include <iterator>
#include <algorithm>

using namespace SALOMESDS;

DataScopeServer::DataScopeServer(CORBA::ORB_ptr orb, const std::string& scopeName):_orb(CORBA::ORB::_duplicate(orb)),_name(scopeName)
{
}

DataScopeServer::DataScopeServer(const DataScopeServer& other):_name(other._name),_vars(other._vars)
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
SALOME::StringVec *DataScopeServer::listVars()
{
  SALOME::StringVec *ret(new SALOME::StringVec);
  std::size_t sz(_vars.size());
  ret->length(sz);
  std::list< std::pair< SALOME::BasicDataServer_var, AutoRefCountPtr<BasicDataServer> > >::iterator it(_vars.begin());
  for(std::size_t i=0;i<sz;it++,i++)
    {
      BasicDataServer *obj((*it).second);
      std::string name(obj->getVarNameCpp());
      (*ret)[i]=CORBA::string_dup(name.c_str());
    }
  return ret;
}

/*!
 * Called remotely -> to protect against throw
 */
SALOME::BasicDataServer_ptr DataScopeServer::retrieveVar(const char *varName)
{
  std::string varNameCpp(varName);
  std::vector<std::string> allNames(getAllVarNames());
  std::vector<std::string>::iterator it(std::find(allNames.begin(),allNames.end(),varNameCpp));
  if(it==allNames.end())
    {
      std::ostringstream oss; oss << "DataScopeServer::retrieveVar : name \"" << varNameCpp << "\" does not exists ! Possibilities are :";
      std::copy(allNames.begin(),allNames.end(),std::ostream_iterator<std::string>(oss,", "));
      throw Exception(oss.str());
    }
  std::size_t pos(std::distance(allNames.begin(),it));
  std::list< std::pair< SALOME::BasicDataServer_var, AutoRefCountPtr<BasicDataServer> > >::iterator it0(_vars.begin());
  for(std::size_t i=0;i<pos;i++,it0++);
  return SALOME::BasicDataServer::_duplicate((*it0).first);
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
  CORBA::Object_var ret(activateWithDedicatedPOA(tmp));
  std::pair< SALOME::BasicDataServer_var, AutoRefCountPtr<BasicDataServer> > p(SALOME::BasicDataServer::_narrow(ret),DynamicCastSafe<StringDataServer,BasicDataServer>(tmp));
  _vars.push_back(p);
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
  CORBA::Object_var ret(activateWithDedicatedPOA(tmp));
  std::pair< SALOME::BasicDataServer_var, AutoRefCountPtr<BasicDataServer> > p(SALOME::BasicDataServer::_narrow(ret),DynamicCastSafe<AnyDataServer,BasicDataServer>(tmp));
  _vars.push_back(p);
  return SALOME::AnyDataServer::_narrow(ret);
}

void DataScopeServer::shutdownIfNotHostedByDSM()
{
  SALOME_NamingService ns(_orb);
  CORBA::Object_var obj(ns.Resolve(DataServerManager::NAME_IN_NS));
  SALOME::DataServerManager_var dsm(SALOME::DataServerManager::_narrow(obj));
  if(CORBA::is_nil(dsm))
    return ;
  // destroy ref in the naming service
  std::string fullScopeName(SALOMESDS::DataServerManager::CreateAbsNameInNSFromScopeName(_name));
  ns.Destroy_Name(fullScopeName.c_str());
  // establish if dsm and this shared the same POA. If yes dsm and this are collocated !
  PortableServer::ServantBase *ret(0);
  try
    {
      ret=_poa->reference_to_servant(dsm);
    }
  catch(...) { ret=0; }
  //
  if(!ret)
    _orb->shutdown(0);
  else
    ret->_remove_ref();
}

/*!
 * \a ptr has been activated by the POA \a poa.
 */
void DataScopeServer::setPOAAndRegister(PortableServer::POA_var poa, SALOME::DataScopeServer_ptr ptr)
{
  _poa=poa;
  std::string fullScopeName(SALOMESDS::DataServerManager::CreateAbsNameInNSFromScopeName(_name));
  SALOME_NamingService ns(_orb);
  ns.Register(ptr,fullScopeName.c_str());
}

std::vector< std::string > DataScopeServer::getAllVarNames() const
{
  std::size_t sz(_vars.size());
  std::vector<std::string> ret(sz);
  std::list< std::pair< SALOME::BasicDataServer_var, AutoRefCountPtr<BasicDataServer> > >::const_iterator it(_vars.begin());
  for(std::size_t i=0;i<sz;it++,i++)
    ret[i]=(*it).second->getVarNameCpp();
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
  std::string name(ds->getVarNameCpp());
  std::ostringstream oss; oss << "POA@" << name;
  std::string zePOAName(oss.str());
  PortableServer::POA_var poa(rootPoa->create_POA(zePOAName.c_str(),pman,policies));
  threadPol->destroy();
  //
  PortableServer::ObjectId_var id(poa->activate_object(ds));
  CORBA::Object_var ret(poa->id_to_reference(id));
  return ret;
}
