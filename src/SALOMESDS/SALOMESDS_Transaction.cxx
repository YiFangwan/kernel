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
#include "SALOMESDS_DataServerManager.hxx"
#include "SALOMESDS_Exception.hxx"

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

TransactionVarCreate::TransactionVarCreate(const std::string& varName, const SALOME::ByteVec& constValue):Transaction(varName)
{
  FromByteSeqToVB(constValue,_data);
}

void TransactionVarCreate::prepareRollBackInCaseOfFailure()
{//nothing it is not a bug
}
