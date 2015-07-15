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

#include "SALOMESDS_Transaction.hxx"
#include "SALOMESDS_Exception.hxx"
#include "SALOMESDS_PickelizedPyObjServer.hxx"

#include <sstream>

using namespace SALOMESDS;

void Transaction::FromByteSeqToVB(const SALOME::ByteVec& bsToBeConv, std::vector<unsigned char>& ret)
{
  std::size_t sz(bsToBeConv.length());
  ret.resize(sz);
  unsigned char *buf(const_cast<unsigned char *>(&ret[0]));
  for(std::size_t i=0;i<sz;i++)
    buf[i]=bsToBeConv[i];
}

void Transaction::FromVBToByteSeq(const std::vector<unsigned char>& bsToBeConv, SALOME::ByteVec& ret)
{
  std::size_t sz(bsToBeConv.size());
  ret.length(sz);
  for(std::size_t i=0;i<sz;i++)
    ret[i]=bsToBeConv[i];
}

Transaction::~Transaction()
{
}

TransactionVarCreate::TransactionVarCreate(DataScopeServerTransaction *dsct, const std::string& varName, const SALOME::ByteVec& constValue):Transaction(dsct,varName)
{
  FromByteSeqToVB(constValue,_data);
}

void TransactionVarCreate::prepareRollBackInCaseOfFailure()
{//nothing it is not a bug
  checkNotAlreadyExisting();
}

void TransactionVarCreate::rollBack()
{
  if(_dsct->existVar(_var_name.c_str()))
    _dsct->deleteVar(_var_name.c_str());
}

/*!
 TODO : To be implemented.
 */
void TransactionVarCreate::notify()
{
}

void TransactionRdOnlyVarCreate::perform()
{
  SALOME::ByteVec data2;
  FromVBToByteSeq(_data,data2);
  _dsct->createRdOnlyVarInternal(_var_name,data2);
}

void TransactionRdExtVarCreate::perform()
{
  SALOME::ByteVec data2;
  FromVBToByteSeq(_data,data2);
  _dsct->createRdExtVarInternal(_var_name,data2);
}

void TransactionRdWrVarCreate::perform()
{
  SALOME::ByteVec data2;
  FromVBToByteSeq(_data,data2);
  _dsct->createRdWrVarInternal(_var_name,data2);
}

TransactionAddKeyValueHard::TransactionAddKeyValueHard(DataScopeServerTransaction *dsct, const std::string& varName, const SALOME::ByteVec& key, const SALOME::ByteVec& value):Transaction(dsct,varName),_varc(0)
{
  FromByteSeqToVB(key,_key);
  FromByteSeqToVB(value,_value);
}

void TransactionAddKeyValueHard::prepareRollBackInCaseOfFailure()
{
  _varc=checkVarExistingAndDict();
  //
  _zeDataBefore.clear();
  SALOME::ByteVec *zeDataBefore(_varc->fetchSerializedContent());
  PickelizedPyObjServer::FromByteSeqToCpp(*zeDataBefore,_zeDataBefore);
}

void TransactionAddKeyValueHard::perform()
{
  _varc->addKeyValueHard(_key,_value);
}

void TransactionAddKeyValueHard::rollBack()
{
  PyObject *obj(_varc->getPyObjFromPickled(_zeDataBefore));
  _varc->setNewPyObj(obj);
  _zeDataBefore.clear();
}

/*!
 TODO : To be implemented.
 */
void TransactionAddKeyValueHard::notify()
{
}

TransactionAddKeyValueHard::~TransactionAddKeyValueHard()
{
}
