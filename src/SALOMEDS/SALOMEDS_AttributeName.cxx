//  File   : SALOMEDS_AttributeName.cxx
//  Author : Sergey RUIN
//  Module : SALOME

#include "SALOMEDS_AttributeName.hxx"

#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>

SALOMEDS_AttributeName::SALOMEDS_AttributeName(const Handle(SALOMEDSImpl_AttributeName)& theAttr)
:SALOMEDS_GenericAttribute(theAttr)
{
    cout <<"Creation of NAME attr : local" << endl;
}

SALOMEDS_AttributeName::SALOMEDS_AttributeName(SALOMEDS::AttributeName_ptr theAttr)
:SALOMEDS_GenericAttribute(theAttr)
{}

SALOMEDS_AttributeName::~SALOMEDS_AttributeName()
{}

char* SALOMEDS_AttributeName::Value()
{
  TCollection_AsciiString aValue;
  if(_isLocal) aValue = Handle(SALOMEDSImpl_AttributeName)::DownCast(_local_impl)->Value();
  else aValue = SALOMEDS::AttributeName::_narrow(_corba_impl)->Value();
  return aValue.ToCString();
}
 
void SALOMEDS_AttributeName::SetValue(const char* value)
{
  CheckLocked();
  TCollection_AsciiString aValue((char*)value);
  if(_isLocal) Handle(SALOMEDSImpl_AttributeName)::DownCast(_local_impl)->SetValue(aValue);
  else SALOMEDS::AttributeName::_narrow(_corba_impl)->SetValue(aValue.ToCString());
}
