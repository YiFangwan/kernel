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

#include "SALOMESDS_TransactionFactory.hxx"
#include "SALOMESDS_DataServerManager.hxx"
#include "SALOMESDS_AutoRefCountPtr.hxx"
#include "SALOMESDS_Transaction.hxx"
#include "SALOME_NamingService.hxx"
#include "SALOMESDS_Exception.hxx"

#include <sstream>

using namespace SALOMESDS;

const char TransactionFactory::NAME_IN_NS[]="/TransactionFactory";

TransactionFactory::TransactionFactory(DataServerManager *dsm):_dsm(dsm)
{
  if(!_dsm)
    throw Exception("TransactionFactory : null pointer !");
  CORBA::ORB_var orb(_dsm->getORB());
  CORBA::Object_var obj0(orb->resolve_initial_references("RootPOA"));
  _poa=PortableServer::POA::_narrow(obj0);
  //
  PortableServer::ObjectId_var id(_poa->activate_object(this));
  CORBA::Object_var obj(_poa->id_to_reference(id));
  SALOME::TransactionFactory_var obj2(SALOME::TransactionFactory::_narrow(obj));
  // publish Data server manager in NS
  SALOME_NamingService ns(orb);
  ns.Register(obj2,NAME_IN_NS);
}

SALOME::Transaction_ptr TransactionFactory::createRdOnlyVar(const char *varName, const char *scopeName, const SALOME::ByteVec& constValue)
{
  TransactionRdOnlyVarCreate *ret(new TransactionRdOnlyVarCreate(this,varName,scopeName,constValue));
  ret->checkAliveAndKicking();
  PortableServer::ObjectId_var id(_poa->activate_object(ret));
  CORBA::Object_var obj(_poa->id_to_reference(id));
  return SALOME::Transaction::_narrow(obj);
}

void TransactionFactory::atomicApply(const SALOME::ListOfTransaction& transactions)
{
  std::size_t sz(transactions.length());
  if(sz==0)
    return ;
  std::vector< AutoServantPtr<Transaction> > transactionsCpp(sz);
  for(std::size_t i=0;i<sz;i++)
    {
      PortableServer::ServantBase *eltBase(0);
      Transaction *elt(0);
      try
        {
          eltBase=_poa->reference_to_servant(transactions[i]);
          elt=dynamic_cast<Transaction *>(eltBase);
        }
      catch(...)
        {
          std::ostringstream oss; oss << "TransactionFactory::atomicApply : the elt #" << i << " is invalid !";
          throw Exception(oss.str());
        }
      if(!elt)
        {
          std::ostringstream oss; oss << "TransactionFactory::atomicApply : the elt #" << i << " is null ?";
          throw Exception(oss.str());
        }
      elt->_remove_ref();
      transactionsCpp[i]=elt;
      transactionsCpp[i].setHolder(this);
    }
  std::string scopeName(transactionsCpp[0]->getScopeName());
  for(std::size_t i=0;i<sz;i++)
    if(transactionsCpp[i]->getScopeName()!=scopeName)
      throw Exception("TransactionFactory::atomicApply : all transactions must refer the same scope !");
  for(std::size_t i=0;i<sz;i++)
    transactionsCpp[i]->checkAliveAndKicking();
}
