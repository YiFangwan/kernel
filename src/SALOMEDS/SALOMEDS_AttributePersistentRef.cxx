//  File   : SALOMEDS_AttributePersistentRef.cxx
//  Author : Sergey RUIN
//  Module : SALOME

#include "SALOMEDS_AttributePersistentRef.hxx"

#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>

SALOMEDS_AttributePersistentRef::SALOMEDS_AttributePersistentRef(const Handle(SALOMEDSImpl_AttributePersistentRef)& theAttr)
:SALOMEDS_GenericAttribute(theAttr)
{}

SALOMEDS_AttributePersistentRef::SALOMEDS_AttributePersistentRef(SALOMEDS::AttributePersistentRef_ptr theAttr)
:SALOMEDS_GenericAttribute(theAttr)
{}

SALOMEDS_AttributePersistentRef::~SALOMEDS_AttributePersistentRef()
{}

char* SALOMEDS_AttributePersistentRef::Value()
{
  TCollection_AsciiString aValue;
  if(_isLocal) aValue = Handle(SALOMEDSImpl_AttributePersistentRef)::DownCast(_local_impl)->Value();
  else aValue = SALOMEDS::AttributePersistentRef::_narrow(_corba_impl)->Value();
  return aValue.ToCString();
}
 
void SALOMEDS_AttributePersistentRef::SetValue(const char* value)
{
  CheckLocked();
  TCollection_AsciiString aValue((char*)value);
  if(_isLocal) Handle(SALOMEDSImpl_AttributePersistentRef)::DownCast(_local_impl)->SetValue(aValue);
  else SALOMEDS::AttributePersistentRef::_narrow(_corba_impl)->SetValue(aValue.ToCString());
}
