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

#include "SALOMESDS_StringDataServer.hxx"
#include "SALOMESDS_DataScopeServer.hxx"
#include "SALOMESDS_Exception.hxx"

#include <sstream>

using namespace SALOMESDS;

StringDataServer::StringDataServer(DataScopeServer *father, const std::string& varName):BasicDataServer(father,varName)
{
}

StringDataServer::~StringDataServer()
{
}

/*!
 * Called remotely -> to protect against throw
 */
void StringDataServer::setValueOf(const SALOME::ByteVec& newValue)
{
  checkReadOnlyStatusRegardingConstness("StringDataServer::setValueOf : read only var !");
  FromByteSeqToCpp(newValue,_data);
}

/*!
 * Called remotely -> to protect against throw
 */
SALOME::ByteVec *StringDataServer::getValueOf()
{
  return FromCppToByteSeq(_data);
}

/*!
 * Called remotely -> to protect against throw
 */
SALOME::ByteVec *StringDataServer::invokePythonMethodOn(const char *method, const SALOME::ByteVec& args)
{
  PyObject *self(getPyObjFromPickled(_data));
  std::string argsCpp;
  FromByteSeqToCpp(args,argsCpp);
  PyObject *argsPy(getPyObjFromPickled(argsCpp));
  //
  PyObject *selfMeth(PyObject_GetAttrString(self,method));
  if(!selfMeth)
    {
      std::ostringstream oss; oss << "StringDataServer::invokePythonMethodOn : Method \"" << method << "\" is not available !";
      throw Exception(oss.str());
    }
  PyObject *res(PyObject_CallObject(selfMeth,argsPy));
  _data=pickelize(self);// if it is a non const method !
  std::string retCpp(pickelize(res));
  // to test : res and self
  Py_XDECREF(selfMeth);
  //
  Py_XDECREF(argsPy);
  return FromCppToByteSeq(retCpp);
}

void StringDataServer::FromByteSeqToCpp(const SALOME::ByteVec& bsToBeConv, std::string& ret)
{
  std::size_t sz(bsToBeConv.length());
  ret.resize(sz,' ');
  char *buf(const_cast<char *>(ret.c_str()));
  for(std::size_t i=0;i<sz;i++)
    buf[i]=bsToBeConv[i];
}

SALOME::ByteVec *StringDataServer::FromCppToByteSeq(const std::string& strToBeConv)
{
  SALOME::ByteVec *ret(new SALOME::ByteVec);
  const char *buf(strToBeConv.c_str());
  std::size_t sz(strToBeConv.size());
  ret->length(sz);
  for(std::size_t i=0;i<sz;i++)
    (*ret)[i]=buf[i];
  return ret;
}

//! New reference returned
PyObject *StringDataServer::getPyObjFromPickled(const std::string& pickledData)
{
  std::size_t sz(pickledData.size());
  PyObject *pickledDataPy(PyString_FromStringAndSize(NULL,sz));// agy : do not use PyString_FromString because std::string hides a vector of byte.
  char *buf(PyString_AsString(pickledDataPy));// this buf can be used thanks to python documentation.
  const char *inBuf(pickledData.c_str());
  std::copy(inBuf,inBuf+sz,buf);
  PyObject *selfMeth(PyObject_GetAttrString(_father->getPickler(),"loads"));
  PyObject *args(PyTuple_New(1)); PyTuple_SetItem(args,0,pickledDataPy);
  PyObject *ret(PyObject_CallObject(selfMeth,args));
  Py_XDECREF(args);
  Py_XDECREF(selfMeth);
  return ret;
}

//! obj is consumed by this method.
std::string StringDataServer::pickelize(PyObject *obj)
{
  PyObject *args(PyTuple_New(2));
  PyTuple_SetItem(args,0,obj);
  PyTuple_SetItem(args,1,PyInt_FromLong(2));// because "assert(cPickle.HIGHEST_PROTOCOL is 2)"
  PyObject *selfMeth(PyObject_GetAttrString(_father->getPickler(),"dumps"));
  PyObject *retPy(PyObject_CallObject(selfMeth,args));
  Py_XDECREF(selfMeth);
  Py_XDECREF(args);
  std::size_t sz(PyString_Size(retPy));
  std::string ret(sz,'\0');
  const char *buf(PyString_AsString(retPy));
  char *inBuf(const_cast<char *>(ret.c_str()));
  for(std::size_t i=0;i<sz;i++)
    inBuf[i]=buf[i];
  Py_XDECREF(retPy);
  return ret;
}
