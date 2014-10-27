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

DataScopeServer::DataScopeServer(const std::string& scopeName):_name(scopeName)
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
  return tmp->_this();
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
  return tmp->_this();
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
