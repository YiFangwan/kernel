//  File   : SALOMEDS_AttributeIOR.cxx
//  Author : Sergey RUIN
//  Module : SALOME

#include "SALOMEDS_AttributeIOR.hxx"

#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>

SALOMEDS_AttributeIOR::SALOMEDS_AttributeIOR(const Handle(SALOMEDSImpl_AttributeIOR)& theAttr)
:SALOMEDS_GenericAttribute(theAttr)
{}

SALOMEDS_AttributeIOR::SALOMEDS_AttributeIOR(SALOMEDS::AttributeIOR_ptr theAttr)
:SALOMEDS_GenericAttribute(theAttr)
{}

SALOMEDS_AttributeIOR::~SALOMEDS_AttributeIOR()
{}

char* SALOMEDS_AttributeIOR::Value()
{
  TCollection_AsciiString aValue;
  if(_isLocal) aValue = Handle(SALOMEDSImpl_AttributeIOR)::DownCast(_local_impl)->Value();
  else aValue = SALOMEDS::AttributeIOR::_narrow(_corba_impl)->Value();
  return aValue.ToCString();
}
 
void SALOMEDS_AttributeIOR::SetValue(const char* value)
{
  CheckLocked();
  TCollection_AsciiString aValue((char*)value);
  if(_isLocal) Handle(SALOMEDSImpl_AttributeIOR)::DownCast(_local_impl)->SetValue(aValue);
  else SALOMEDS::AttributeIOR::_narrow(_corba_impl)->SetValue(aValue.ToCString());
}
