//  File   : SALOMEDS_AttributeUserID.cxx
//  Author : Sergey RUIN
//  Module : SALOME

#include "SALOMEDS_AttributeUserID.hxx"

#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <Standard_GUID.hxx>

SALOMEDS_AttributeUserID::SALOMEDS_AttributeUserID(const Handle(SALOMEDSImpl_AttributeUserID)& theAttr)
:SALOMEDS_GenericAttribute(theAttr)
{}

SALOMEDS_AttributeUserID::SALOMEDS_AttributeUserID(SALOMEDS::AttributeUserID_ptr theAttr)
:SALOMEDS_GenericAttribute(theAttr)
{}

SALOMEDS_AttributeUserID::~SALOMEDS_AttributeUserID()
{}

char* SALOMEDS_AttributeUserID::Value()
{
  TCollection_AsciiString aValue;
  if(_isLocal) {
    char guid[40];
    Handle(SALOMEDSImpl_AttributeUserID)::DownCast(_local_impl)->Value().ToCString(guid);
    aValue = TCollection_AsciiString(guid);
  }
  else aValue = SALOMEDS::AttributeUserID::_narrow(_corba_impl)->Value();
  return aValue.ToCString();
}
 
void SALOMEDS_AttributeUserID::SetValue(const char* value)
{
  CheckLocked();
  TCollection_AsciiString aValue((char*)value);
  if(_isLocal) Handle(SALOMEDSImpl_AttributeUserID)::DownCast(_local_impl)->SetValue(Standard_GUID(aValue.ToCString()));
  else SALOMEDS::AttributeUserID::_narrow(_corba_impl)->SetValue(aValue.ToCString());
}
