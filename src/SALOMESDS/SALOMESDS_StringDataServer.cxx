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

const char StringDataServer::FAKE_VAR_NAME_FOR_WORK[]="_tmp8739023zP";

StringDataServer::StringDataServer(DataScopeServer *father, const std::string& varName):BasicDataServer(father,varName),_code_for_pickle(0)
{
  std::ostringstream codeStr; codeStr << " ; cPickle.dumps(" << FAKE_VAR_NAME_FOR_WORK << ",cPickle.HIGHEST_PROTOCOL)";
  _code_for_pickle=(PyCodeObject*)Py_CompileString(codeStr.str().c_str(),getVarNameCpp().c_str(),Py_eval_input);//Py_file_input Py_eval_input
}

StringDataServer::~StringDataServer()
{
  Py_XDECREF(_code_for_pickle);
}

/*!
 * Called remotely -> to protect against throw
 */
void StringDataServer::setValueOf(const char *newValue)
{
  checkReadOnlyStatusRegardingConstness("StringDataServer::setValueOf : read only var !");
  _data=newValue;
}

/*!
 * Called remotely -> to protect against throw
 */
char *StringDataServer::getValueOf()
{
  return CORBA::string_dup(_data.c_str());
}

/*!
 * Called remotely -> to protect against throw
 */
char *StringDataServer::invokePythonMethodOn(const char *method, const char *args)
{
  PyObject *self(getPyObjFromPickled(_data));
  PyObject *argsPy(getPyObjFromPickled(args));
  //
  PyObject *selfMeth(PyObject_GetAttrString(self,method));
  if(!selfMeth)
    {
      std::ostringstream oss; oss << "StringDataServer::invokePythonMethodOn : Method \"" << method << "\" is not available !";
      throw Exception(oss.str());
    }
  PyObject *res(PyObject_CallObject(self,argsPy));
  _data=pickelize(self);// if it is a non const method !
  std::string ret(pickelize(res));
  // to test : res and self
  Py_XDECREF(selfMeth);
  //
  Py_XDECREF(argsPy);
  return CORBA::string_dup(ret.c_str());
}

//! New reference returned
PyObject *StringDataServer::getPyObjFromPickled(const std::string& pickledData)
{
  PyObject *pickledDataPy(PyString_FromString(pickledData.c_str()));
  PyObject *selfMeth(PyObject_GetAttrString(_father->getPickler(),"loads"));
  PyObject *ret(PyObject_CallObject(selfMeth,pickledDataPy));
  Py_XDECREF(pickledDataPy);
  Py_XDECREF(selfMeth);
  return ret;
}

//! obj is consumed by this method.
std::string StringDataServer::pickelize(PyObject *obj)
{
  PyObject *key(PyString_FromString(FAKE_VAR_NAME_FOR_WORK));
  PyDict_SetItem(_father->getGlobals(),key,obj);
  PyObject *retPy(PyEval_EvalCode(_code_for_pickle,_father->getGlobals(),_father->getLocals()));
  if(!PyString_Check(retPy))
    throw Exception("StringDataServer::pickelize : internal error #0 !");
  std::string ret(PyString_AsString(retPy));
  return ret;
}
