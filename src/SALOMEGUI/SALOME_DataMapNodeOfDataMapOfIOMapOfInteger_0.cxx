using namespace std;
// File generated by CPPExt (Transient)
//                     Copyright (C) 1991,1995 by
//  
//                      MATRA DATAVISION, FRANCE
//  
// This software is furnished in accordance with the terms and conditions
// of the contract and with the inclusion of the above copyright notice.
// This software or any other copy thereof may not be provided or otherwise
// be made available to any other person. No title to an ownership of the
// software is hereby transferred.
//  
// At the termination of the contract, the software and all copies of this
// software must be deleted.
//
#include "SALOME_DataMapNodeOfDataMapOfIOMapOfInteger.hxx"

#ifndef _Standard_TypeMismatch_HeaderFile
#include <Standard_TypeMismatch.hxx>
#endif

#ifndef _SALOME_InteractiveObject_HeaderFile
#include "SALOME_InteractiveObject.hxx"
#endif
#ifndef _TColStd_MapOfInteger_HeaderFile
#include <TColStd_MapOfInteger.hxx>
#endif
#ifndef _TColStd_MapTransientHasher_HeaderFile
#include <TColStd_MapTransientHasher.hxx>
#endif
#ifndef _SALOME_DataMapOfIOMapOfInteger_HeaderFile
#include "SALOME_DataMapOfIOMapOfInteger.hxx"
#endif
#ifndef _SALOME_DataMapIteratorOfDataMapOfIOMapOfInteger_HeaderFile
#include "SALOME_DataMapIteratorOfDataMapOfIOMapOfInteger.hxx"
#endif
SALOME_DataMapNodeOfDataMapOfIOMapOfInteger::~SALOME_DataMapNodeOfDataMapOfIOMapOfInteger() {}
 


Standard_EXPORT Handle_Standard_Type& SALOME_DataMapNodeOfDataMapOfIOMapOfInteger_Type_()
{

    static Handle_Standard_Type aType1 = STANDARD_TYPE(TCollection_MapNode);
  if ( aType1.IsNull()) aType1 = STANDARD_TYPE(TCollection_MapNode);
  static Handle_Standard_Type aType2 = STANDARD_TYPE(MMgt_TShared);
  if ( aType2.IsNull()) aType2 = STANDARD_TYPE(MMgt_TShared);
  static Handle_Standard_Type aType3 = STANDARD_TYPE(Standard_Transient);
  if ( aType3.IsNull()) aType3 = STANDARD_TYPE(Standard_Transient);
 

  static Handle_Standard_Transient _Ancestors[]= {aType1,aType2,aType3,NULL};
  static Handle_Standard_Type _aType = new Standard_Type("SALOME_DataMapNodeOfDataMapOfIOMapOfInteger",
			                                 sizeof(SALOME_DataMapNodeOfDataMapOfIOMapOfInteger),
			                                 1,
			                                 (Standard_Address)_Ancestors,
			                                 (Standard_Address)NULL);

  return _aType;
}


// DownCast method
//   allow safe downcasting
//
const Handle(SALOME_DataMapNodeOfDataMapOfIOMapOfInteger) Handle(SALOME_DataMapNodeOfDataMapOfIOMapOfInteger)::DownCast(const Handle(Standard_Transient)& AnObject) 
{
  Handle(SALOME_DataMapNodeOfDataMapOfIOMapOfInteger) _anOtherObject;

  if (!AnObject.IsNull()) {
     if (AnObject->IsKind(STANDARD_TYPE(SALOME_DataMapNodeOfDataMapOfIOMapOfInteger))) {
       _anOtherObject = Handle(SALOME_DataMapNodeOfDataMapOfIOMapOfInteger)((Handle(SALOME_DataMapNodeOfDataMapOfIOMapOfInteger)&)AnObject);
     }
  }

  return _anOtherObject ;
}
const Handle(Standard_Type)& SALOME_DataMapNodeOfDataMapOfIOMapOfInteger::DynamicType() const 
{ 
  return STANDARD_TYPE(SALOME_DataMapNodeOfDataMapOfIOMapOfInteger) ; 
}
Standard_Boolean SALOME_DataMapNodeOfDataMapOfIOMapOfInteger::IsKind(const Handle(Standard_Type)& AType) const 
{ 
  return (STANDARD_TYPE(SALOME_DataMapNodeOfDataMapOfIOMapOfInteger) == AType || TCollection_MapNode::IsKind(AType)); 
}
Handle_SALOME_DataMapNodeOfDataMapOfIOMapOfInteger::~Handle_SALOME_DataMapNodeOfDataMapOfIOMapOfInteger() {}
#define TheKey Handle_SALOME_InteractiveObject
#define TheKey_hxx "SALOME_InteractiveObject.hxx"
#define TheItem TColStd_MapOfInteger
#define TheItem_hxx <TColStd_MapOfInteger.hxx>
#define Hasher TColStd_MapTransientHasher
#define Hasher_hxx <TColStd_MapTransientHasher.hxx>
#define TCollection_DataMapNode SALOME_DataMapNodeOfDataMapOfIOMapOfInteger
#define TCollection_DataMapNode_hxx "SALOME_DataMapNodeOfDataMapOfIOMapOfInteger.hxx"
#define TCollection_DataMapIterator SALOME_DataMapIteratorOfDataMapOfIOMapOfInteger
#define TCollection_DataMapIterator_hxx "SALOME_DataMapIteratorOfDataMapOfIOMapOfInteger.hxx"
#define Handle_TCollection_DataMapNode Handle_SALOME_DataMapNodeOfDataMapOfIOMapOfInteger
#define TCollection_DataMapNode_Type_() SALOME_DataMapNodeOfDataMapOfIOMapOfInteger_Type_()
#define TCollection_DataMap SALOME_DataMapOfIOMapOfInteger
#define TCollection_DataMap_hxx "SALOME_DataMapOfIOMapOfInteger.hxx"
#include <TCollection_DataMapNode.gxx>

