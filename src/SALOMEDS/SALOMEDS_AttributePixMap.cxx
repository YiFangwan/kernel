//  File   : SALOMEDS_AttributePixMap.cxx
//  Author : Sergey RUIN
//  Module : SALOME

#include "SALOMEDS_AttributePixMap.hxx"

#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>

SALOMEDS_AttributePixMap::SALOMEDS_AttributePixMap(const Handle(SALOMEDSImpl_AttributePixMap)& theAttr)
:SALOMEDS_GenericAttribute(theAttr)
{}

SALOMEDS_AttributePixMap::SALOMEDS_AttributePixMap(SALOMEDS::AttributePixMap_ptr theAttr)
:SALOMEDS_GenericAttribute(theAttr)
{}

SALOMEDS_AttributePixMap::~SALOMEDS_AttributePixMap()
{}

bool SALOMEDS_AttributePixMap::HasPixMap()
{
  bool ret;
  if(_isLocal) ret = Handle(SALOMEDSImpl_AttributePixMap)::DownCast(_local_impl)->HasPixMap();
  else ret = SALOMEDS::AttributePixMap::_narrow(_corba_impl)->HasPixMap();
}

char* SALOMEDS_AttributePixMap::GetPixMap()
{
  TCollection_AsciiString aValue;
  if(_isLocal) aValue = Handle(SALOMEDSImpl_AttributePixMap)::DownCast(_local_impl)->GetPixMap();
  else aValue = SALOMEDS::AttributePixMap::_narrow(_corba_impl)->GetPixMap();
  return aValue.ToCString();
}
 
void SALOMEDS_AttributePixMap::SetPixMap(const char* value)
{
  CheckLocked();
  TCollection_AsciiString aValue((char*)value);
  if(_isLocal) Handle(SALOMEDSImpl_AttributePixMap)::DownCast(_local_impl)->SetPixMap(aValue);
  else SALOMEDS::AttributePixMap::_narrow(_corba_impl)->SetPixMap(aValue.ToCString());
}
