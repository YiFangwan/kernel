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

#ifndef __SALOMESDS_TRANSACTION_HXX__
#define __SALOMESDS_TRANSACTION_HXX__

#include "SALOMEconfig.h"
#include CORBA_SERVER_HEADER(SALOME_SDS)

#include "SALOMESDS_Defines.hxx"

#include <string>
#include <vector>

namespace SALOMESDS
{
  class TransactionFactory;

  class SALOMESDS_EXPORT Transaction : public virtual POA_SALOME::Transaction
  {
  public:
    Transaction(TransactionFactory *tf, const std::string& varName, const std::string& scopeName):_tf(tf),_var_name(varName),_scope_name(scopeName) { }
    std::string getScopeName() const { return _scope_name; }
    std::string getVarName() const { return _var_name; }
    virtual void checkAliveAndKicking();
    virtual void prepareRollBackInCaseOfFailure() = 0;
  public:
    static void FromByteSeqToVB(const SALOME::ByteVec& bsToBeConv, std::vector<unsigned char>& ret);
  protected:
    TransactionFactory *_tf;
    std::string _var_name;
    std::string _scope_name;
  };

  class TransactionVarCreate : public Transaction
  {
  public:
    TransactionVarCreate(TransactionFactory *tf, const std::string& varName, const std::string& scopeName, const SALOME::ByteVec& constValue);
    void checkAliveAndKicking();
    void prepareRollBackInCaseOfFailure();
  protected:
    std::vector<unsigned char> _data;
  };

  class TransactionRdOnlyVarCreate : public TransactionVarCreate
  {
  public:
    TransactionRdOnlyVarCreate(TransactionFactory *tf, const std::string& varName, const std::string& scopeName, const SALOME::ByteVec& constValue):TransactionVarCreate(tf,varName,scopeName,constValue) { }
  };
}

#endif
