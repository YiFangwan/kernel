//  File   : SALOMEDS_AttributeComment.cxx
//  Author : Sergey RUIN
//  Module : SALOME

#include "SALOMEDS_AttributeComment.hxx"

#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>

SALOMEDS_AttributeComment::SALOMEDS_AttributeComment(const Handle(SALOMEDSImpl_AttributeComment)& theAttr)
:SALOMEDS_GenericAttribute(theAttr)
{}

SALOMEDS_AttributeComment::SALOMEDS_AttributeComment(SALOMEDS::AttributeComment_ptr theAttr)
:SALOMEDS_GenericAttribute(theAttr)
{}

SALOMEDS_AttributeComment::~SALOMEDS_AttributeComment()
{}

char* SALOMEDS_AttributeComment::Value()
{
  TCollection_AsciiString aValue;
  if(_isLocal) aValue = Handle(SALOMEDSImpl_AttributeComment)::DownCast(_local_impl)->Value();
  else aValue = SALOMEDS::AttributeComment::_narrow(_corba_impl)->Value();
  return aValue.ToCString();
}
 
void SALOMEDS_AttributeComment::SetValue(const char* value)
{
  CheckLocked();
  TCollection_AsciiString aValue((char*)value);
  if(_isLocal) Handle(SALOMEDSImpl_AttributeComment)::DownCast(_local_impl)->SetValue(aValue);
  else SALOMEDS::AttributeComment::_narrow(_corba_impl)->SetValue(aValue.ToCString());
}
