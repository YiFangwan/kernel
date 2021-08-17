// Copyright (C) 2021  CEA/DEN, EDF R&D
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

%module KernelSDS

%include "std_string.i"

%{
#include "KernelSDS.hxx"
#include "Utils_SALOME_Exception.hxx"
%}


namespace SALOME
{
  class SALOME_Exception
  {
    public:
    %extend
    {
      std::string __str__() const
      {
        return std::string(self->what());
      }
    }
  };
}

using namespace SALOME;

%exception {
  try {
    $action
  }
  catch (SALOME::SALOME_Exception& _e) {
    // Reraise with SWIG_Python_Raise
    SWIG_Python_Raise(SWIG_NewPointerObj((new SALOME::SALOME_Exception(static_cast< const SALOME::SALOME_Exception& >(_e))),SWIGTYPE_p_INTERP_KERNEL__Exception,SWIG_POINTER_OWN), "INTERP_KERNEL::Exception", SWIGTYPE_p_INTERP_KERNEL__Exception);
    SWIG_fail;
  }
}

%inline
{
  std::string GetDSMInstanceInternal(PyObject *argv)
  {
    if(!PyList_Check(argv))
      THROW_SALOME_EXCEPTION("Not a pylist");
    Py_ssize_t sz=PyList_Size(pyLi);
    std::vector<std::string> argvCpp(sz);
    for(Py_ssize_t i = 0 ; i < sz ; ++i)
    {
      PyObject *obj = PyList_GetItem(pyLi,i);
      if(!PyUnicode_Check(obj))
        THROW_SALOME_EXCEPTION("Not a pylist of strings");
      {
        Py_ssize_t dummy;
        argvCpp[i] = PyUnicode_AsUTF8AndSize(obj,&dummy);
      }
    }
    GetDSMInstanceInternal(argvCpp);
  }
}

%pythoncode %{
def GetDSMInstance(argv):
  import SALOME
  import CORBA
  orb=CORBA.ORB_init([''])
  return orb.string_to_object(GetDSMInstanceInternal(argv))
%}
