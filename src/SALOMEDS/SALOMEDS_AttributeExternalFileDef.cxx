//  File   : SALOMEDS_AttributeExternalFileDef.cxx
//  Author : Sergey RUIN
//  Module : SALOME

#include "SALOMEDS_AttributeExternalFileDef.hxx"

#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>

SALOMEDS_AttributeExternalFileDef::SALOMEDS_AttributeExternalFileDef(const Handle(SALOMEDSImpl_AttributeExternalFileDef)& theAttr)
:SALOMEDS_GenericAttribute(theAttr)
{}

SALOMEDS_AttributeExternalFileDef::SALOMEDS_AttributeExternalFileDef(SALOMEDS::AttributeExternalFileDef_ptr theAttr)
:SALOMEDS_GenericAttribute(theAttr)
{}

SALOMEDS_AttributeExternalFileDef::~SALOMEDS_AttributeExternalFileDef()
{}

char* SALOMEDS_AttributeExternalFileDef::Value()
{
  TCollection_AsciiString aValue;
  if(_isLocal) aValue = Handle(SALOMEDSImpl_AttributeExternalFileDef)::DownCast(_local_impl)->Value();
  else aValue = SALOMEDS::AttributeExternalFileDef::_narrow(_corba_impl)->Value();
  return aValue.ToCString();
}
 
void SALOMEDS_AttributeExternalFileDef::SetValue(const char* value)
{
  CheckLocked();
  TCollection_AsciiString aValue((char*)value);
  if(_isLocal) Handle(SALOMEDSImpl_AttributeExternalFileDef)::DownCast(_local_impl)->SetValue(aValue);
  else SALOMEDS::AttributeExternalFileDef::_narrow(_corba_impl)->SetValue(aValue.ToCString());
}
