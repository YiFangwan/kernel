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

#include "SALOMESDS_KeyWaiter.hxx"
#include "SALOMESDS_DataScopeServer.hxx"
#include "SALOMESDS_PickelizedPyObjServer.hxx"

using namespace SALOMESDS;

KeyWaiter::KeyWaiter(DataScopeServerTransaction *dst, const SALOME::ByteVec& keyVal):_dst(dst),_ze_key(0),_ze_value(0)
{
  std::string st;
  PickelizedPyObjServer::FromByteSeqToCpp(keyVal,st);
  _ze_key=PickelizedPyObjServer::GetPyObjFromPickled(st,_dst);
  PyObject *selfMeth(PyObject_GetAttrString(_dst->getPickler(),"__contains__"));
  PyObject *args(PyTuple_New(1));
  PyTuple_SetItem(args,0,_ze_key); Py_XINCREF(_ze_key); // _ze_key is stolen by PyTuple_SetItem
  PyObject *retPy(PyObject_CallObject(selfMeth,args));
  Py_XDECREF(args);
  Py_XDECREF(selfMeth);
  //
  if(retPy!=Py_False && retPy!=Py_True)
    throw Exception("KeyWaiter constructor : unexpected return of dict.__contains__ !");
  
  Py_XDECREF(retPy);
}

KeyWaiter::~KeyWaiter()
{
  Py_XDECREF(_ze_key);
  if(_ze_value)
    Py_XDECREF(_ze_value);
}

PortableServer::POA_var KeyWaiter::getPOA() const
{
  return _dst->getPOA4KeyWaiter();
}

SALOME::ByteVec *KeyWaiter::waitFor()
{
  return 0;
}
