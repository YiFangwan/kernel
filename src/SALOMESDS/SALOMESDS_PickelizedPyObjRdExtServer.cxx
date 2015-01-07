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

#include "SALOMESDS_PickelizedPyObjRdExtServer.hxx"
#include "SALOMESDS_DataScopeServer.hxx"
#include "SALOMESDS_Exception.hxx"

#include <iostream>
#include <sstream>

using namespace SALOMESDS;

PickelizedPyObjRdExtServer::PickelizedPyObjRdExtServer(DataScopeServer *father, const std::string& varName, const SALOME::ByteVec& value):PickelizedPyObjServer(father,varName,value)
{
}

//! obj is consumed
PickelizedPyObjRdExtServer::PickelizedPyObjRdExtServer(DataScopeServer *father, const std::string& varName, PyObject *obj):PickelizedPyObjServer(father,varName,obj)
{
}

PickelizedPyObjRdExtServer::~PickelizedPyObjRdExtServer()
{
}

/*!
 * Called remotely -> to protect against throw
 */
SALOME::PickelizedPyObjRdExtServer_ptr PickelizedPyObjRdExtServer::invokePythonMethodOn(const char *method, const SALOME::ByteVec& args)
{
  if(!_self)
    throw Exception("PickelizedPyObjRdExtServer::invokePythonMethodOn : self is NULL !");
  std::string argsCpp;
  FromByteSeqToCpp(args,argsCpp);
  PyObject *argsPy(getPyObjFromPickled(argsCpp));
  //
  PyObject *selfMeth(PyObject_GetAttrString(_self,method));
  if(!selfMeth)
    {
      std::ostringstream oss; oss << "PickelizedPyObjRdExtServer::invokePythonMethodOn : Method \"" << method << "\" is not available !";
      throw Exception(oss.str());
    }
  PyObject *res(PyObject_CallObject(selfMeth,argsPy));// self can have been modified by this call !
  Py_XDECREF(selfMeth);
  Py_XDECREF(argsPy);
  if(!res)
    {
      std::ostringstream oss; oss << "PickelizedPyObjRdExtServer::invokePythonMethodOn : Problem during invokation serverside of Method \"" << method << "\" !";
      throw Exception(oss.str());
    }
  PickelizedPyObjRdExtServer *ret(new PickelizedPyObjRdExtServer(_father,DataScopeServer::BuildTmpVarNameFrom(getVarNameCpp()),res));
  PortableServer::POA_var poa(_father->getPOA());
  ret->setPOA(poa);
  PortableServer::ObjectId_var id(poa->activate_object(ret));
  CORBA::Object_var obj(poa->id_to_reference(id));
  return SALOME::PickelizedPyObjRdExtServer::_narrow(obj);
}
