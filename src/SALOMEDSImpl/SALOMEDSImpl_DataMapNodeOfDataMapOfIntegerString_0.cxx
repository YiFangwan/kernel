//  SALOME SALOMEDSImpl : data structure of SALOME and sources of Salome data server 
//
//  Copyright (C) 2003  CEA/DEN, EDF R&D
//
//
//
//  File   : SALOMEDSImpl_DataMapNodeOfDataMapOfIntegerString_0.cxx
//  Author : Sergey Ruin
//  Module : SALOME

#include <SALOMEDSImpl_DataMapNodeOfDataMapOfIntegerString.hxx>

#ifndef _Standard_TypeMismatch_HeaderFile
#include <Standard_TypeMismatch.hxx>
#endif

#ifndef _TCollection_ExtendedString_HeaderFile
#include <TCollection_ExtendedString.hxx>
#endif
#ifndef _TColStd_MapIntegerHasher_HeaderFile
#include <TColStd_MapIntegerHasher.hxx>
#endif
#ifndef _SALOMEDSImpl_DataMapOfIntegerString_HeaderFile
#include <SALOMEDSImpl_DataMapOfIntegerString.hxx>
#endif
#ifndef _SALOMEDSImpl_DataMapIteratorOfDataMapOfIntegerString_HeaderFile
#include <SALOMEDSImpl_DataMapIteratorOfDataMapOfIntegerString.hxx>
#endif
SALOMEDSImpl_DataMapNodeOfDataMapOfIntegerString::~SALOMEDSImpl_DataMapNodeOfDataMapOfIntegerString() {}
 


Standard_EXPORT Handle_Standard_Type& SALOMEDSImpl_DataMapNodeOfDataMapOfIntegerString_Type_()
{

    static Handle_Standard_Type aType1 = STANDARD_TYPE(TCollection_MapNode);
  if ( aType1.IsNull()) aType1 = STANDARD_TYPE(TCollection_MapNode);
  static Handle_Standard_Type aType2 = STANDARD_TYPE(MMgt_TShared);
  if ( aType2.IsNull()) aType2 = STANDARD_TYPE(MMgt_TShared);
  static Handle_Standard_Type aType3 = STANDARD_TYPE(Standard_Transient);
  if ( aType3.IsNull()) aType3 = STANDARD_TYPE(Standard_Transient);
 

  static Handle_Standard_Transient _Ancestors[]= {aType1,aType2,aType3,NULL};
  static Handle_Standard_Type _aType = new Standard_Type("SALOMEDSImpl_DataMapNodeOfDataMapOfIntegerString",
			                                 sizeof(SALOMEDSImpl_DataMapNodeOfDataMapOfIntegerString),
			                                 1,
			                                 (Standard_Address)_Ancestors,
			                                 (Standard_Address)NULL);

  return _aType;
}


// DownCast method
//   allow safe downcasting
//
const Handle(SALOMEDSImpl_DataMapNodeOfDataMapOfIntegerString) Handle(SALOMEDSImpl_DataMapNodeOfDataMapOfIntegerString)::DownCast(const Handle(Standard_Transient)& AnObject) 
{
  Handle(SALOMEDSImpl_DataMapNodeOfDataMapOfIntegerString) _anOtherObject;

  if (!AnObject.IsNull()) {
     if (AnObject->IsKind(STANDARD_TYPE(SALOMEDSImpl_DataMapNodeOfDataMapOfIntegerString))) {
       _anOtherObject = Handle(SALOMEDSImpl_DataMapNodeOfDataMapOfIntegerString)((Handle(SALOMEDSImpl_DataMapNodeOfDataMapOfIntegerString)&)AnObject);
     }
  }

  return _anOtherObject ;
}
const Handle(Standard_Type)& SALOMEDSImpl_DataMapNodeOfDataMapOfIntegerString::DynamicType() const 
{ 
  return STANDARD_TYPE(SALOMEDSImpl_DataMapNodeOfDataMapOfIntegerString) ; 
}
Standard_Boolean SALOMEDSImpl_DataMapNodeOfDataMapOfIntegerString::IsKind(const Handle(Standard_Type)& AType) const 
{ 
  return (STANDARD_TYPE(SALOMEDSImpl_DataMapNodeOfDataMapOfIntegerString) == AType || TCollection_MapNode::IsKind(AType)); 
}
Handle_SALOMEDSImpl_DataMapNodeOfDataMapOfIntegerString::~Handle_SALOMEDSImpl_DataMapNodeOfDataMapOfIntegerString() {}
#define TheKey Standard_Real
#define TheKey_hxx <Standard_Real.hxx>
#define TheItem TCollection_ExtendedString
#define TheItem_hxx <TCollection_ExtendedString.hxx>
#define Hasher TColStd_MapIntegerHasher
#define Hasher_hxx <TColStd_MapIntegerHasher.hxx>
#define TCollection_DataMapNode SALOMEDSImpl_DataMapNodeOfDataMapOfIntegerString
#define TCollection_DataMapNode_hxx <SALOMEDSImpl_DataMapNodeOfDataMapOfIntegerString.hxx>
#define TCollection_DataMapIterator SALOMEDSImpl_DataMapIteratorOfDataMapOfIntegerString
#define TCollection_DataMapIterator_hxx <SALOMEDSImpl_DataMapIteratorOfDataMapOfIntegerString.hxx>
#define Handle_TCollection_DataMapNode Handle_SALOMEDSImpl_DataMapNodeOfDataMapOfIntegerString
#define TCollection_DataMapNode_Type_() SALOMEDSImpl_DataMapNodeOfDataMapOfIntegerString_Type_()
#define TCollection_DataMap SALOMEDSImpl_DataMapOfIntegerString
#define TCollection_DataMap_hxx <SALOMEDSImpl_DataMapOfIntegerString.hxx>
#include <TCollection_DataMapNode.gxx>

