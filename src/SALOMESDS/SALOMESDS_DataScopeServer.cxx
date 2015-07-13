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

#include "SALOMESDS_DataScopeServer.hxx"
#include "SALOMESDS_DataServerManager.hxx"
#include "SALOMESDS_PickelizedPyObjRdOnlyServer.hxx"
#include "SALOMESDS_PickelizedPyObjRdExtServer.hxx"
#include "SALOMESDS_PickelizedPyObjRdWrServer.hxx"
#include "SALOMESDS_Transaction.hxx"
#include "SALOME_NamingService.hxx"
#include "SALOMESDS_Exception.hxx"

#include <sstream>
#include <iterator>
#include <algorithm>

// agy : awful, to be factorized with ContainerManager.
#ifndef WIN32
#include <unistd.h>
#else
#include <process.h>
#define getpid _getpid
#endif

using namespace SALOMESDS;

std::size_t DataScopeServerBase::COUNTER=0;

DataScopeServerBase::DataScopeServerBase(CORBA::ORB_ptr orb, const std::string& scopeName):_globals(0),_locals(0),_pickler(0),_orb(CORBA::ORB::_duplicate(orb)),_name(scopeName)
{
}

DataScopeServerBase::DataScopeServerBase(const DataScopeServerBase& other):_globals(0),_locals(0),_pickler(0),_name(other._name),_vars(other._vars)
{
}

DataScopeServerBase::~DataScopeServerBase()
{
  // _globals is borrowed ref -> do nothing
  Py_XDECREF(_locals);
  Py_XDECREF(_pickler);
}

/*!
 * Called remotely -> to protect against throw
 */
void DataScopeServerBase::ping()
{
}

/*!
 * Called remotely -> to protect against throw
 */
char *DataScopeServerBase::getScopeName()
{
  return CORBA::string_dup(_name.c_str());
}

/*!
 * Called remotely -> to protect against throw
 */
SALOME::StringVec *DataScopeServerBase::listVars()
{
  SALOME::StringVec *ret(new SALOME::StringVec);
  std::size_t sz(_vars.size());
  ret->length(sz);
  std::list< std::pair< SALOME::BasicDataServer_var, BasicDataServer * > >::iterator it(_vars.begin());
  for(std::size_t i=0;i<sz;it++,i++)
    {
      BasicDataServer *obj((*it).second);
      std::string name(obj->getVarNameCpp());
      (*ret)[i]=CORBA::string_dup(name.c_str());
    }
  return ret;
}

CORBA::Boolean DataScopeServerBase::existVar(const char *varName)
{
  std::list< std::pair< SALOME::BasicDataServer_var, BasicDataServer * > >::const_iterator it(_vars.begin());
  for(;it!=_vars.end();it++)
    if((*it).second->getVarNameCpp()==varName)
      return true;
  return false;
}

SALOME::BasicDataServer_ptr DataScopeServerBase::retrieveVar(const char *varName)
{
  std::string varNameCpp(varName);
  std::vector<std::string> allNames(getAllVarNames());
  std::vector<std::string>::iterator it(std::find(allNames.begin(),allNames.end(),varNameCpp));
  if(it==allNames.end())
    {
      std::ostringstream oss; oss << "DataScopeServerBase::retrieveVar : name \"" << varNameCpp << "\" does not exists ! Possibilities are :";
      std::copy(allNames.begin(),allNames.end(),std::ostream_iterator<std::string>(oss,", "));
      throw Exception(oss.str());
    }
  std::size_t pos(std::distance(allNames.begin(),it));
  std::list< std::pair< SALOME::BasicDataServer_var, BasicDataServer * > >::iterator it0(_vars.begin());
  for(std::size_t i=0;i<pos;i++,it0++);
  return SALOME::BasicDataServer::_duplicate((*it0).first);
}

void DataScopeServerBase::deleteVar(const char *varName)
{
  std::string varNameCpp(varName);
  std::vector<std::string> allNames(getAllVarNames());
  std::vector<std::string>::iterator it(std::find(allNames.begin(),allNames.end(),varNameCpp));
  if(it==allNames.end())
    {
      std::ostringstream oss; oss << "DataScopeServerBase::deleteVar : name \"" << varNameCpp << "\" does not exists ! Possibilities are :";
      std::copy(allNames.begin(),allNames.end(),std::ostream_iterator<std::string>(oss,", "));
      throw Exception(oss.str());
    }
  std::size_t pos(std::distance(allNames.begin(),it));
  std::list< std::pair< SALOME::BasicDataServer_var, BasicDataServer * > >::iterator it0(_vars.begin());
  (*it0).first->UnRegister();
  _vars.erase(it0);
}

void DataScopeServerBase::shutdownIfNotHostedByDSM()
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
    {
      PortableServer::ObjectId_var oid(_poa->servant_to_id(this));
      _poa->deactivate_object(oid);
      ret->_remove_ref();
    }
}

void DataScopeServerBase::initializePython(int argc, char *argv[])
{
  Py_Initialize();
  PySys_SetArgv(argc,argv);
  PyObject *mainmod(PyImport_AddModule("__main__"));
  _globals=PyModule_GetDict(mainmod);
  if(PyDict_GetItemString(_globals, "__builtins__") == NULL)
    {
      PyObject *bimod(PyImport_ImportModule("__builtin__"));
      if (bimod == NULL || PyDict_SetItemString(_globals, "__builtins__", bimod) != 0)
        Py_FatalError("can't add __builtins__ to __main__");
      Py_XDECREF(bimod);
    }
  _locals=PyDict_New();
  PyObject *tmp(PyList_New(0));
  _pickler=PyImport_ImportModuleLevel(const_cast<char *>("cPickle"),_globals,_locals,tmp,-1);
}

void DataScopeServerBase::registerToSalomePiDict() const
{
  PyObject *mod(PyImport_ImportModule("addToKillList"));
  if(!mod)
    return;
  PyObject *meth(PyObject_GetAttrString(mod,"addToKillList"));
  if(!meth)
    { Py_XDECREF(mod); return ; }
  PyObject *args(PyTuple_New(2));
  PyTuple_SetItem(args,0,PyInt_FromLong(getpid()));
  PyTuple_SetItem(args,1,PyString_FromString("SALOME_DataScopeServerBase"));
  PyObject *res(PyObject_CallObject(meth,args));
  Py_XDECREF(args);
  Py_XDECREF(res);
  Py_XDECREF(mod);
}

/*!
 * \a ptr has been activated by the POA \a poa.
 */
void DataScopeServerBase::setPOAAndRegister(PortableServer::POA_var poa, SALOME::DataScopeServerBase_ptr ptr)
{
  _poa=poa;
  std::string fullScopeName(SALOMESDS::DataServerManager::CreateAbsNameInNSFromScopeName(_name));
  SALOME_NamingService ns(_orb);
  ns.Register(ptr,fullScopeName.c_str());
}

std::string DataScopeServerBase::BuildTmpVarNameFrom(const std::string& varName)
{
  std::ostringstream oss;
  oss << varName << "@" << COUNTER++;
  return oss.str();
}

std::vector< std::string > DataScopeServerBase::getAllVarNames() const
{
  std::size_t sz(_vars.size());
  std::vector<std::string> ret(sz);
  std::list< std::pair< SALOME::BasicDataServer_var, BasicDataServer * > >::const_iterator it(_vars.begin());
  for(std::size_t i=0;i<sz;it++,i++)
    ret[i]=(*it).second->getVarNameCpp();
  return ret;
}

CORBA::Object_var DataScopeServerBase::activateWithDedicatedPOA(BasicDataServer *ds)
{
  PortableServer::ObjectId_var id(_poa->activate_object(ds));
  CORBA::Object_var ret(_poa->id_to_reference(id));
  return ret;
}

void DataScopeServerBase::checkNotAlreadyExistingVar(const std::string& varName)
{
  std::vector<std::string> allNames(getAllVarNames());
  std::vector<std::string>::iterator it(std::find(allNames.begin(),allNames.end(),varName));
  if(it!=allNames.end())
    {
      std::ostringstream oss; oss << "DataScopeServerBase::checkNotAlreadyExistingVar : name \"" << varName << "\" already exists !";
      throw Exception(oss.str());
    }
}

///////

DataScopeServer::DataScopeServer(CORBA::ORB_ptr orb, const std::string& scopeName):DataScopeServerBase(orb,scopeName)
{
}

DataScopeServer::DataScopeServer(const DataScopeServer& other):DataScopeServerBase(other)
{
}

SALOME::PickelizedPyObjRdOnlyServer_ptr DataScopeServer::createRdOnlyVar(const char *varName, const SALOME::ByteVec& constValue)
{
  std::string varNameCpp(varName);
  checkNotAlreadyExistingVar(varNameCpp);
  PickelizedPyObjRdOnlyServer *tmp(new PickelizedPyObjRdOnlyServer(this,varNameCpp,constValue));
  CORBA::Object_var ret(activateWithDedicatedPOA(tmp));
  std::pair< SALOME::BasicDataServer_var, BasicDataServer * > p(SALOME::BasicDataServer::_narrow(ret),tmp);
  _vars.push_back(p);
  return SALOME::PickelizedPyObjRdOnlyServer::_narrow(ret);
}

SALOME::PickelizedPyObjRdExtServer_ptr DataScopeServer::createRdExtVar(const char *varName, const SALOME::ByteVec& constValue)
{
  std::string varNameCpp(varName);
  checkNotAlreadyExistingVar(varNameCpp);
  PickelizedPyObjRdExtServer *tmp(new PickelizedPyObjRdExtServer(this,varNameCpp,constValue));
  CORBA::Object_var ret(activateWithDedicatedPOA(tmp));
  std::pair< SALOME::BasicDataServer_var, BasicDataServer * > p(SALOME::BasicDataServer::_narrow(ret),tmp);
  _vars.push_back(p);
  return SALOME::PickelizedPyObjRdExtServer::_narrow(ret);
}

/*!
 * Called remotely -> to protect against throw
 */
SALOME::PickelizedPyObjRdWrServer_ptr DataScopeServer::createRdWrVar(const char *typeName, const char *varName)
{
  std::string varNameCpp(varName),typeNameCpp(typeName);
  checkNotAlreadyExistingVar(varNameCpp);
  PickelizedPyObjRdWrServer *tmp(new PickelizedPyObjRdWrServer(this,typeNameCpp,varNameCpp));
  CORBA::Object_var ret(activateWithDedicatedPOA(tmp));
  std::pair< SALOME::BasicDataServer_var, BasicDataServer * > p(SALOME::BasicDataServer::_narrow(ret),tmp);
  _vars.push_back(p);
  return SALOME::PickelizedPyObjRdWrServer::_narrow(ret);
}

DataScopeServer::~DataScopeServer()
{
}

////////

DataScopeServerTransaction::DataScopeServerTransaction(CORBA::ORB_ptr orb, const std::string& scopeName):DataScopeServerBase(orb,scopeName)
{
}

DataScopeServerTransaction::DataScopeServerTransaction(const DataScopeServerTransaction& other):DataScopeServerBase(other)
{
}

void DataScopeServerTransaction::createRdOnlyVarInternal(const std::string& varName, const SALOME::ByteVec& constValue)
{
  checkNotAlreadyExistingVar(varName);
  PickelizedPyObjRdOnlyServer *tmp(new PickelizedPyObjRdOnlyServer(this,varName,constValue));
  CORBA::Object_var ret(activateWithDedicatedPOA(tmp));
  std::pair< SALOME::BasicDataServer_var, BasicDataServer * > p(SALOME::BasicDataServer::_narrow(ret),tmp);
  _vars.push_back(p);
}

void DataScopeServerTransaction::createRdExtVarInternal(const std::string& varName, const SALOME::ByteVec& constValue)
{
  checkNotAlreadyExistingVar(varName);
  PickelizedPyObjRdExtServer *tmp(new PickelizedPyObjRdExtServer(this,varName,constValue));
  CORBA::Object_var ret(activateWithDedicatedPOA(tmp));
  std::pair< SALOME::BasicDataServer_var, BasicDataServer * > p(SALOME::BasicDataServer::_narrow(ret),tmp);
  _vars.push_back(p);
}

void DataScopeServerTransaction::createRdWrVarInternal(const std::string& varName, const SALOME::ByteVec& constValue)
{
  checkNotAlreadyExistingVar(varName);
  PickelizedPyObjRdWrServer *tmp(new PickelizedPyObjRdWrServer(this,varName,constValue));
  CORBA::Object_var ret(activateWithDedicatedPOA(tmp));
  std::pair< SALOME::BasicDataServer_var, BasicDataServer * > p(SALOME::BasicDataServer::_narrow(ret),tmp);
  _vars.push_back(p);
}

SALOME::Transaction_ptr DataScopeServerTransaction::createRdOnlyVarTransac(const char *varName, const SALOME::ByteVec& constValue)
{
  TransactionRdOnlyVarCreate *ret(new TransactionRdOnlyVarCreate(this,varName,constValue));
  ret->checkNotAlreadyExisting();
  PortableServer::ObjectId_var id(_poa->activate_object(ret));
  CORBA::Object_var obj(_poa->id_to_reference(id));
  return SALOME::Transaction::_narrow(obj);
}

SALOME::Transaction_ptr DataScopeServerTransaction::createRdExtVarTransac(const char *varName, const SALOME::ByteVec& constValue)
{
  TransactionRdExtVarCreate *ret(new TransactionRdExtVarCreate(this,varName,constValue));
  ret->checkNotAlreadyExisting();
  PortableServer::ObjectId_var id(_poa->activate_object(ret));
  CORBA::Object_var obj(_poa->id_to_reference(id));
  return SALOME::Transaction::_narrow(obj);
}

SALOME::Transaction_ptr DataScopeServerTransaction::createRdWrVarTransac(const char *varName, const SALOME::ByteVec& constValue)
{
  TransactionRdWrVarCreate *ret(new TransactionRdWrVarCreate(this,varName,constValue));
  ret->checkNotAlreadyExisting();
  PortableServer::ObjectId_var id(_poa->activate_object(ret));
  CORBA::Object_var obj(_poa->id_to_reference(id));
  return SALOME::Transaction::_narrow(obj);
}

class TrustTransaction
{
public:
  TrustTransaction():_must_rollback(0),_ptr(0) { }
  void setTransaction(Transaction *t, bool *mustRollback) { if(!t || !mustRollback) throw Exception("TrustTransaction Error #1"); _must_rollback=mustRollback; t->prepareRollBackInCaseOfFailure(); }
  void operate() { _ptr->perform(); }
  ~TrustTransaction() { if(!_ptr) return ; if(*_must_rollback) _ptr->rollBack(); }
private:
  bool *_must_rollback;
  Transaction *_ptr;
};

void DataScopeServerTransaction::atomicApply(const SALOME::ListOfTransaction& transactions)
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
          std::ostringstream oss; oss << "DataScopeServerTransaction::atomicApply : the elt #" << i << " is invalid !";
          throw Exception(oss.str());
        }
      if(!elt)
        {
          std::ostringstream oss; oss << "DataScopeServerTransaction::atomicApply : the elt #" << i << " is null ?";
          throw Exception(oss.str());
        }
      elt->_remove_ref();
      transactionsCpp[i]=elt;
      transactionsCpp[i].setHolder(this);
    }
  {// important do not merge loops ! 
    std::vector<TrustTransaction> transactions2(sz);
    bool mustRollback(true);
    for(std::size_t i=0;i<sz;i++)
      transactions2[i].setTransaction(transactionsCpp[i],&mustRollback);
    for(std::size_t i=0;i<sz;i++)
      transactions2[i].operate();
    mustRollback=false;
  }
}

DataScopeServerTransaction::~DataScopeServerTransaction()
{
}

