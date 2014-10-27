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

#ifndef __SALOMESDS_DATASCOPEERVER_HXX__
#define __SALOMESDS_DATASCOPEERVER_HXX__

#include "SALOMEconfig.h"
#include CORBA_SERVER_HEADER(SALOME_SDS)

#include "SALOMESDS_AutoRefCountPtr.hxx"
#include "SALOMESDS_RefCountServ.hxx"
#include "SALOMESDS_BasicDataServer.hxx"

#include <string>
#include <vector>
#include <list>

namespace SALOMESDS
{
  class DataScopeServer : public RefCountServ, public virtual POA_SALOME::DataScopeServer
  {
  public:
    DataScopeServer(CORBA::ORB_ptr orb, const std::string& scopeName);
    DataScopeServer(const DataScopeServer& other);
    char *getScopeName();
    SALOME::StringDataServer_ptr createGlobalStringVar(const char *varName);
    SALOME::AnyDataServer_ptr createGlobalAnyVar(const char *varName);
  private:
    std::vector< std::string> getAllVarNames() const;
    CORBA::Object_var activateWithDedicatedPOA(BasicDataServer *ds);
  private:
    CORBA::ORB_var _orb;
    std::string _name;
    std::list< AutoRefCountPtr<BasicDataServer> > _vars;
  };
}

#endif
