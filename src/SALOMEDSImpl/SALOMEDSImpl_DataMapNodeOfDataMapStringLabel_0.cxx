//  SALOME SALOMEDSImpl : data structure of SALOME and sources of Salome data server 
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
//  File   : Handle_SALOMEDSImpl_DataMapNodeOfDataMapStringLabel_0.cxx
//  Author : Sergey RUIN
//  Module : SALOME

using namespace std;
#include "SALOMEDSImpl_DataMapNodeOfDataMapStringLabel.hxx"

#ifndef _Standard_TypeMismatch_HeaderFile
#include <Standard_TypeMismatch.hxx>
#endif

#ifndef _TCollection_ExtendedString_HeaderFile
#include <TCollection_ExtendedString.hxx>
#endif
#ifndef _TDF_Label_HeaderFile
#include <TDF_Label.hxx>
#endif
#ifndef _SALOMEDSImpl_DataMapStringLabel_HeaderFile
#include "SALOMEDSImpl_DataMapStringLabel.hxx"
#endif
#ifndef _SALOMEDSImpl_DataMapIteratorOfDataMapStringLabel_HeaderFile
#include "SALOMEDSImpl_DataMapIteratorOfDataMapStringLabel.hxx"
#endif
SALOMEDSImpl_DataMapNodeOfDataMapStringLabel::~SALOMEDSImpl_DataMapNodeOfDataMapStringLabel() {}
 


Standard_EXPORT Handle_Standard_Type& SALOMEDSImpl_DataMapNodeOfDataMapStringLabel_Type_()
{

    static Handle_Standard_Type aType1 = STANDARD_TYPE(TCollection_MapNode);
  if ( aType1.IsNull()) aType1 = STANDARD_TYPE(TCollection_MapNode);
  static Handle_Standard_Type aType2 = STANDARD_TYPE(MMgt_TShared);
  if ( aType2.IsNull()) aType2 = STANDARD_TYPE(MMgt_TShared);
  static Handle_Standard_Type aType3 = STANDARD_TYPE(Standard_Transient);
  if ( aType3.IsNull()) aType3 = STANDARD_TYPE(Standard_Transient);
 

  static Handle_Standard_Transient _Ancestors[]= {aType1,aType2,aType3,NULL};
  static Handle_Standard_Type _aType = new Standard_Type("SALOMEDSImpl_DataMapNodeOfDataMapStringLabel",
			                                 sizeof(SALOMEDSImpl_DataMapNodeOfDataMapStringLabel),
			                                 1,
			                                 (Standard_Address)_Ancestors,
			                                 (Standard_Address)NULL);

  return _aType;
}


// DownCast method
//   allow safe downcasting
//
const Handle(SALOMEDSImpl_DataMapNodeOfDataMapStringLabel) Handle(SALOMEDSImpl_DataMapNodeOfDataMapStringLabel)::DownCast(const Handle(Standard_Transient)& AnObject) 
{
  Handle(SALOMEDSImpl_DataMapNodeOfDataMapStringLabel) _anOtherObject;

  if (!AnObject.IsNull()) {
     if (AnObject->IsKind(STANDARD_TYPE(SALOMEDSImpl_DataMapNodeOfDataMapStringLabel))) {
       _anOtherObject = Handle(SALOMEDSImpl_DataMapNodeOfDataMapStringLabel)((Handle(SALOMEDSImpl_DataMapNodeOfDataMapStringLabel)&)AnObject);
     }
  }

  return _anOtherObject ;
}
const Handle(Standard_Type)& SALOMEDSImpl_DataMapNodeOfDataMapStringLabel::DynamicType() const 
{ 
  return STANDARD_TYPE(SALOMEDSImpl_DataMapNodeOfDataMapStringLabel) ; 
}
Standard_Boolean SALOMEDSImpl_DataMapNodeOfDataMapStringLabel::IsKind(const Handle(Standard_Type)& AType) const 
{ 
  return (STANDARD_TYPE(SALOMEDSImpl_DataMapNodeOfDataMapStringLabel) == AType || TCollection_MapNode::IsKind(AType)); 
}
Handle_SALOMEDSImpl_DataMapNodeOfDataMapStringLabel::~Handle_SALOMEDSImpl_DataMapNodeOfDataMapStringLabel() {}
#define TheKey TCollection_ExtendedString
#define TheKey_hxx <TCollection_ExtendedString.hxx>
#define TheItem TDF_Label
#define TheItem_hxx <TDF_Label.hxx>
#define Hasher TCollection_ExtendedString
#define Hasher_hxx <TCollection_ExtendedString.hxx>
#define TCollection_DataMapNode SALOMEDSImpl_DataMapNodeOfDataMapStringLabel
#define TCollection_DataMapNode_hxx "SALOMEDSImpl_DataMapNodeOfDataMapStringLabel.hxx"
#define TCollection_DataMapIterator SALOMEDSImpl_DataMapIteratorOfDataMapStringLabel
#define TCollection_DataMapIterator_hxx "SALOMEDSImpl_DataMapIteratorOfDataMapStringLabel.hxx"
#define Handle_TCollection_DataMapNode Handle_SALOMEDSImpl_DataMapNodeOfDataMapStringLabel
#define TCollection_DataMapNode_Type_() SALOMEDSImpl_DataMapNodeOfDataMapStringLabel_Type_()
#define TCollection_DataMap SALOMEDSImpl_DataMapStringLabel
#define TCollection_DataMap_hxx "SALOMEDSImpl_DataMapStringLabel.hxx"
#include <TCollection_DataMapNode.gxx>

