//  SALOME SALOMEDS : data structure of SALOME and sources of Salome data server 
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS 
// 
//  This library is free software; you can redistribute it and/or 
//  modify it under the terms of the GNU Lesser General Public 
//  License as published by the Free Software Foundation; either 
//  version 2.1 of the License. 
// 
//  This library is distributed in the hope that it will be useful, 
//  but WITHOUT ANY WARRANTY; without even the implied warranty of 
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
//  Lesser General Public License for more details. 
// 
//  You should have received a copy of the GNU Lesser General Public 
//  License along with this library; if not, write to the Free Software 
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA 
// 
//  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 
//
//
//
//  File   : SALOMEDS_DrawableAttribute.cxx
//  Author : Yves FRICAUD
//  Module : SALOME
//  $Header$

using namespace std;
#include "SALOMEDS_DrawableAttribute.ixx"
#include <TDataStd_Integer.hxx>

//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================

const Standard_GUID& SALOMEDS_DrawableAttribute::GetID () 
{
  static Standard_GUID SALOMEDS_DrawableAttributeID ("12837184-8F52-11d6-A8A3-0001021E8C7F");
  return SALOMEDS_DrawableAttributeID;
}



//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

Handle(SALOMEDS_DrawableAttribute) SALOMEDS_DrawableAttribute::Set (const TDF_Label& L,
                                                                  const Standard_Integer value) 
{
  Handle(SALOMEDS_DrawableAttribute) A;
  if (!L.FindAttribute(SALOMEDS_DrawableAttribute::GetID(),A)) {
    A = new  SALOMEDS_DrawableAttribute(); 
    L.AddAttribute(A);
  }
  
  (Handle(TDataStd_Integer)::DownCast(A))->Set (value); 
  return A;
}


//=======================================================================
//function : constructor
//purpose  : 
//=======================================================================
SALOMEDS_DrawableAttribute::SALOMEDS_DrawableAttribute()
{
  TDataStd_Integer::Set(0);
}

//=======================================================================
//function : ID
//purpose  : 
//=======================================================================

const Standard_GUID& SALOMEDS_DrawableAttribute::ID () const { return GetID(); }


//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) SALOMEDS_DrawableAttribute::NewEmpty () const
{  
  return new SALOMEDS_DrawableAttribute(); 
}

//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================

void SALOMEDS_DrawableAttribute::Restore(const Handle(TDF_Attribute)& with) 
{
  Standard_Integer s = Handle(TDataStd_Integer)::DownCast (with)->Get ();
  TDataStd_Integer::Set(s);
  return;
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void SALOMEDS_DrawableAttribute::Paste (const Handle(TDF_Attribute)& into,
                                    const Handle(TDF_RelocationTable)& RT) const
{
  Handle(TDataStd_Integer)::DownCast (into)->Set (Get());
}

