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
//  File   : SALOMEDS_AttributeInteger_i.cxx
//  Author : Yves FRICAUD
//  Module : SALOME
//  $Header$

using namespace std;
#include "SALOMEDS_AttributeInteger_i.hxx"
#include "SALOMEDS_SObject_i.hxx"


CORBA::Long SALOMEDS_AttributeInteger_i::Value() {
  return Handle(TDataStd_Integer)::DownCast(_myAttr)->Get();
}

void SALOMEDS_AttributeInteger_i::SetValue(CORBA::Long value) {
  CheckLocked();
  Handle(TDataStd_Integer)::DownCast(_myAttr)->Set(value);
}

char* SALOMEDS_AttributeInteger_i::Store() {
  char* IntVal = new char[25];
  sprintf(IntVal, "%d", Value());
  return IntVal;
}

void SALOMEDS_AttributeInteger_i::Restore(const char* value) {
  char *err = NULL;
  CORBA::Long l =  atol(value);
  SetValue(l);
}
