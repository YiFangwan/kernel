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

%module NamingService

%{
#include "SALOME_Fake_NamingService.hxx"
#include "SALOME_KernelORB.hxx"
%}

%include "std_string.i"
%include "std_vector.i"

%rename(NamingService) SALOME_Fake_NamingService ;

%template(svec) std::vector<std::string>;

class SALOME_Fake_NamingService
{
public:
  SALOME_Fake_NamingService();
  std::vector< std::string > repr();
  %extend {
    std::string _ResolveInternal(const char *Path)
    {
      CORBA::Object_var obj = self->Resolve(Path);
      CORBA::ORB_ptr orb = KERNEL::getORB();
      CORBA::String_var ior = orb->object_to_string(obj);
      return std::string(ior);
    }
    void _RegisterInternal(const char *ior, const char* Path)
    {
      CORBA::ORB_ptr orb = KERNEL::getORB();
      CORBA::Object_var obj = orb->string_to_object(ior);
      self->Register(obj,Path);
    }
  }
};

%pythoncode %{
def NamingService_Resolve(self,Path):
  ret = self._ResolveInternal(Path)
  import CORBA
  orb=CORBA.ORB_init([''])
  return orb.string_to_object(ret)
def NamingService_Register(self,obj,Path):
  import CORBA
  orb=CORBA.ORB_init([''])
  self._RegisterInternal( orb.object_to_string(obj) , Path)
NamingService.Resolve = NamingService_Resolve
NamingService.Register = NamingService_Register
%}
