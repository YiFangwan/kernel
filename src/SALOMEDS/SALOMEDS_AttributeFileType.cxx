//  File   : SALOMEDS_AttributeFileType.cxx
//  Author : Sergey RUIN
//  Module : SALOME

#include "SALOMEDS_AttributeFileType.hxx"

#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>

SALOMEDS_AttributeFileType::SALOMEDS_AttributeFileType(const Handle(SALOMEDSImpl_AttributeFileType)& theAttr)
:SALOMEDS_GenericAttribute(theAttr)
{}

SALOMEDS_AttributeFileType::SALOMEDS_AttributeFileType(SALOMEDS::AttributeFileType_ptr theAttr)
:SALOMEDS_GenericAttribute(theAttr)
{}

SALOMEDS_AttributeFileType::~SALOMEDS_AttributeFileType()
{}

char* SALOMEDS_AttributeFileType::Value()
{
  TCollection_AsciiString aValue;
  if(_isLocal) aValue = Handle(SALOMEDSImpl_AttributeFileType)::DownCast(_local_impl)->Value();
  else aValue = SALOMEDS::AttributeFileType::_narrow(_corba_impl)->Value();
  return aValue.ToCString();
}
 
void SALOMEDS_AttributeFileType::SetValue(const char* value)
{
  CheckLocked();
  TCollection_AsciiString aValue((char*)value);
  if(_isLocal) Handle(SALOMEDSImpl_AttributeFileType)::DownCast(_local_impl)->SetValue(aValue);
  else SALOMEDS::AttributeFileType::_narrow(_corba_impl)->SetValue(aValue.ToCString());
}
