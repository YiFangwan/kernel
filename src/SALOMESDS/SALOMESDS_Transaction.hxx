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
#include "SALOMESDS_Exception.hxx"
#include "SALOMESDS_DataServerManager.hxx"

#include <string>
#include <vector>

namespace SALOMESDS
{
  class SALOMESDS_EXPORT Transaction : public virtual POA_SALOME::Transaction
  {
  public:
    Transaction(DataScopeServerTransaction *dsct, const std::string& varName):_dsct(dsct),_var_name(varName) { if(!_dsct) throw Exception("Transaction constructor error !"); }
    std::string getVarName() const { return _var_name; }
    virtual void prepareRollBackInCaseOfFailure() = 0;
    virtual void perform() = 0;
    virtual void rollBack() = 0;
  public:
    static void FromByteSeqToVB(const SALOME::ByteVec& bsToBeConv, std::vector<unsigned char>& ret);
    static void FromVBToByteSeq(const std::vector<unsigned char>& bsToBeConv, SALOME::ByteVec& ret);
  protected:
    DataScopeServerTransaction *_dsct;
    std::string _var_name;
  };

  class TransactionVarCreate : public Transaction
  {
  public:
    TransactionVarCreate(DataScopeServerTransaction *dsct, const std::string& varName, const SALOME::ByteVec& constValue);
    void checkNotAlreadyExisting() { _dsct->checkNotAlreadyExistingVar(_var_name); }
    void prepareRollBackInCaseOfFailure();
    void rollBack();
  protected:
    std::vector<unsigned char> _data;
  };

  class TransactionRdOnlyVarCreate : public TransactionVarCreate
  {
  public:
    TransactionRdOnlyVarCreate(DataScopeServerTransaction *dsct, const std::string& varName, const SALOME::ByteVec& constValue):TransactionVarCreate(dsct,varName,constValue) { }
    void perform();
  };

  class TransactionRdExtVarCreate : public TransactionVarCreate
  {
  public:
    TransactionRdExtVarCreate(DataScopeServerTransaction *dsct, const std::string& varName, const SALOME::ByteVec& constValue):TransactionVarCreate(dsct,varName,constValue) { }
    void perform();
  };

  class TransactionRdWrVarCreate : public TransactionVarCreate
  {
  public:
    TransactionRdWrVarCreate(DataScopeServerTransaction *dsct, const std::string& varName, const SALOME::ByteVec& constValue):TransactionVarCreate(dsct,varName,constValue) { }
    void perform();
  };
}

#endif
