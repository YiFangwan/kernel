// Copyright (C) 2007-2015  CEA/DEN, EDF R&D, OPEN CASCADE
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

#ifndef __SALOMESDS_TRANSACTIONFACTORY_HXX__
#define __SALOMESDS_TRANSACTIONFACTORY_HXX__

#include "SALOMEconfig.h"
#include CORBA_SERVER_HEADER(SALOME_SDS)
#include "SALOMESDS_Defines.hxx"
#include "SALOMESDS_AutoRefCountPtr.hxx"

#include <string>
#include <vector>

namespace SALOMESDS
{
  class DataServerManager;

  class SALOMESDS_EXPORT TransactionFactory : public virtual POA_SALOME::TransactionFactory, POAHolder
  {
  public:
    TransactionFactory(DataServerManager *dsm);
    DataServerManager *getDSM() const { return _dsm; }
    PortableServer::POA_var getPOA() const { return _poa; }
  public:
    SALOME::Transaction_ptr createRdOnlyVar(const char *varName, const char *scopeName, const SALOME::ByteVec& constValue);
    SALOME::Transaction_ptr createRdExtVar(const char *varName, const char *scopeName, const SALOME::ByteVec& constValue);
    SALOME::Transaction_ptr createRdWrVar(const char *varName, const char *scopeName, const SALOME::ByteVec& constValue);
    SALOME::Transaction_ptr addKeyValueInVarErrorIfAlreadyExisting(const char *varName, const char *scopeName, const SALOME::ByteVec& key, const SALOME::ByteVec& value);
    SALOME::Transaction_ptr addKeyValueInVarHard(const char *varName, const char *scopeName, const SALOME::ByteVec& key, const SALOME::ByteVec& value);
    SALOME::Transaction_ptr removeKeyInVarErrorIfNotAlreadyExisting(const char *varName, const char *scopeName, const SALOME::ByteVec& key);
    SALOME::ByteVec *waitForKeyInVar(const char *varName, const char *scopeName, const SALOME::ByteVec& constKey);
    SALOME::ByteVec *waitForKeyInVarAndKillIt(const char *varName, const char *scopeName, const SALOME::ByteVec& constKey, SALOME::Transaction_out transaction);
    void atomicApply(const SALOME::ListOfTransaction& transactions);
  public:
    static const char NAME_IN_NS[];
  private:
    DataServerManager *_dsm;
    //! multi thread poa
    PortableServer::POA_var _poa;
  };
}

#endif
