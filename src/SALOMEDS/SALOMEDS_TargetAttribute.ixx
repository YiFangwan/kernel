//  File      : SALOMEDS_TargetAttribute.ixx
//  Created   : Fri Aug 16 10:20:05 2002
//  Author    : Yves FRICAUD

//  Project   : SALOME
//  Module    : SALOMEDS
//  Copyright : Open CASCADE 2002
//  $Header: 

#include "SALOMEDS_TargetAttribute.jxx"

#ifndef _Standard_TypeMismatch_HeaderFile
#include <Standard_TypeMismatch.hxx>
#endif

SALOMEDS_TargetAttribute::~SALOMEDS_TargetAttribute() {}
 


Standard_EXPORT Handle_Standard_Type& SALOMEDS_TargetAttribute_Type_()
{

    static Handle_Standard_Type aType1 = STANDARD_TYPE(TDataStd_Relation);
  if ( aType1.IsNull()) aType1 = STANDARD_TYPE(TDataStd_Relation);
  static Handle_Standard_Type aType2 = STANDARD_TYPE(TDF_Attribute);
  if ( aType2.IsNull()) aType2 = STANDARD_TYPE(TDF_Attribute);
  static Handle_Standard_Type aType3 = STANDARD_TYPE(MMgt_TShared);
  if ( aType3.IsNull()) aType3 = STANDARD_TYPE(MMgt_TShared);
  static Handle_Standard_Type aType4 = STANDARD_TYPE(Standard_Transient);
  if ( aType4.IsNull()) aType4 = STANDARD_TYPE(Standard_Transient);
 

  static Handle_Standard_Transient _Ancestors[]= {aType1,aType2,aType3,aType4,NULL};
  static Handle_Standard_Type _aType = new Standard_Type("SALOMEDS_TargetAttribute",
			                                 sizeof(SALOMEDS_TargetAttribute),
			                                 1,
			                                 (Standard_Address)_Ancestors,
			                                 (Standard_Address)NULL);

  return _aType;
}


// DownCast method
//   allow safe downcasting
//
const Handle(SALOMEDS_TargetAttribute) Handle(SALOMEDS_TargetAttribute)::DownCast(const Handle(Standard_Transient)& AnObject) 
{
  Handle(SALOMEDS_TargetAttribute) _anOtherObject;

  if (!AnObject.IsNull()) {
     if (AnObject->IsKind(STANDARD_TYPE(SALOMEDS_TargetAttribute))) {
       _anOtherObject = Handle(SALOMEDS_TargetAttribute)((Handle(SALOMEDS_TargetAttribute)&)AnObject);
     }
  }

  return _anOtherObject ;
}
const Handle(Standard_Type)& SALOMEDS_TargetAttribute::DynamicType() const 
{ 
  return STANDARD_TYPE(SALOMEDS_TargetAttribute) ; 
}
Standard_Boolean SALOMEDS_TargetAttribute::IsKind(const Handle(Standard_Type)& AType) const 
{ 
  return (STANDARD_TYPE(SALOMEDS_TargetAttribute) == AType || TDataStd_Relation::IsKind(AType)); 
}
Handle_SALOMEDS_TargetAttribute::~Handle_SALOMEDS_TargetAttribute() {}

