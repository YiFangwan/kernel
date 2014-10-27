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

#ifndef __SALOMESDS_DATASERVERMANAGER_HXX__
#define __SALOMESDS_DATASERVERMANAGER_HXX__

#include "SALOME_SDS.hh"
//#include CORBA_SERVER_HEADER(SALOME_SDS)

#include "SALOMESDS_AutoRefCountPtr.hxx"
#include "SALOMESDS_DataScopeServer.hxx"

#include <list>
#include <string>

namespace SALOMESDS
{
  class DataScopeServer;
  
  class DataServerManager : public virtual POA_SALOME::DataServerManager
  {
  public:
    DataServerManager();
    SALOME::DataScopeServer_ptr getDefaultScope();
    SALOME::DataScopeServer_ptr createDataScope(const char *scopeName);
    SALOME::DataScopeServer_ptr retriveDataScope(const char *scopeName);
    SALOME::DataScopeServer_ptr removeDataScope(const char *scopeName);
  private:
    AutoRefCountPtr<DataScopeServer> _dft_scope;
    std::list< AutoRefCountPtr<DataScopeServer> > _scopes;
  };
}

#endif
