//  File   : SALOMEDS_AttributePythonObject.cxx
//  Author : Sergey RUIN
//  Module : SALOME

#include "SALOMEDS_AttributePythonObject.hxx"

#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>

SALOMEDS_AttributePythonObject::SALOMEDS_AttributePythonObject(const Handle(SALOMEDSImpl_AttributePythonObject)& theAttr)
:SALOMEDS_GenericAttribute(theAttr)
{}

SALOMEDS_AttributePythonObject::SALOMEDS_AttributePythonObject(SALOMEDS::AttributePythonObject_ptr theAttr)
:SALOMEDS_GenericAttribute(theAttr)
{}

SALOMEDS_AttributePythonObject::~SALOMEDS_AttributePythonObject()
{}

bool SALOMEDS_AttributePythonObject::IsScript()
{
  bool ret;
  if(_isLocal) ret = Handle(SALOMEDSImpl_AttributePythonObject)::DownCast(_local_impl)->IsScript();
  else ret = SALOMEDS::AttributePythonObject::_narrow(_corba_impl)->IsScript();
  return ret;
}

char* SALOMEDS_AttributePythonObject::GetObject()
{
  TCollection_AsciiString aValue;
  if(_isLocal) aValue = Handle(SALOMEDSImpl_AttributePythonObject)::DownCast(_local_impl)->GetObject();
  else aValue = SALOMEDS::AttributePythonObject::_narrow(_corba_impl)->GetObject();
  return aValue.ToCString();
}
 
void SALOMEDS_AttributePythonObject::SetObject(const char* theSequence, bool IsScript)
{
  CheckLocked();
  TCollection_AsciiString aValue((char*)theSequence);
  if(_isLocal) Handle(SALOMEDSImpl_AttributePythonObject)::DownCast(_local_impl)->SetObject(aValue, IsScript);
  else SALOMEDS::AttributePythonObject::_narrow(_corba_impl)->SetObject(aValue.ToCString(), IsScript);
}
