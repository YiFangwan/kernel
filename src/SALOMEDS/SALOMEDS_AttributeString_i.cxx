// Copyright (C) 2005  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
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
//  File   : SALOMEDS_AttributeString_i.cxx
//  Author : Sergey RUIN
//  Module : SALOME


#include "SALOMEDS_AttributeString_i.hxx"

#include <TCollection_ExtendedString.hxx>
#include "SALOMEDS_SObject_i.hxx"
#include "SALOMEDS.hxx"

using namespace std;

char* SALOMEDS_AttributeString_i::Value()
{
  SALOMEDS::Locker lock;
  
  CORBA::String_var c_s =
    CORBA::string_dup(TCollection_AsciiString(Handle(SALOMEDSImpl_AttributeString)::DownCast(_impl)->Value()).ToCString());
  return c_s._retn();
}

void SALOMEDS_AttributeString_i::SetValue(const char* value) 
{
  SALOMEDS::Locker lock; 

  CheckLocked();
  TCollection_AsciiString aStr((char*)value);
  Handle(SALOMEDSImpl_AttributeString)::DownCast(_impl)->SetValue(TCollection_ExtendedString(aStr));
}
