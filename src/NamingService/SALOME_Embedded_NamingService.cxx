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

#include "SALOME_Embedded_NamingService.hxx"
#include "SALOME_Embedded_NamingService_Common.hxx"
#include "SALOME_Fake_NamingService.hxx"

#include <memory>
#include <cstring>

void SALOME_Embedded_NamingService::Register(const Engines::IORType& ObjRef, const char *Path)
{
  SALOME_Fake_NamingService ns;
  ns.Register(IORToObject(ObjRef),Path);
}

void SALOME_Embedded_NamingService::Destroy_FullDirectory(const char *Path)
{
  SALOME_Fake_NamingService ns;
  ns.Destroy_FullDirectory(Path);
}

void SALOME_Embedded_NamingService::Destroy_Name(const char *Path)
{
  SALOME_Fake_NamingService ns;
  ns.Destroy_Name(Path);
}

Engines::IORType *SALOME_Embedded_NamingService::Resolve(const char *Path)
{
  SALOME_Fake_NamingService ns;
  CORBA::Object_var obj = ns.Resolve(Path);
  return ObjectToIOR(obj);
}

Engines::IORType *SALOME_Embedded_NamingService::ResolveFirst(const char *Path)
{
  SALOME_Fake_NamingService ns;
  CORBA::Object_var obj = ns.ResolveFirst(Path);
  return ObjectToIOR(obj);
}
