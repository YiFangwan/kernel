//  File   : SALOMEDS_SComponent.cxx
//  Author : Sergey RUIN
//  Module : SALOME

using namespace std; 

#include "SALOMEDS_SComponent.hxx"

SALOMEDS_SComponent::SALOMEDS_SComponent(SALOMEDS::SComponent_ptr theSComponent)
:SALOMEDS_SObject(theSComponent) 
{}

SALOMEDS_SComponent::SALOMEDS_SComponent(const Handle(SALOMEDSImpl_SComponent)& theSComponent)
:SALOMEDS_SObject(theSComponent) 
{}

SALOMEDS_SComponent::~SALOMEDS_SComponent()
{}

char* SALOMEDS_SComponent::ComponentDataType()
{
  TCollection_AsciiString aType;
  if(_isLocal) {
    aType = (Handle(SALOMEDSImpl_SComponent)::DownCast(GetLocalImpl()))->ComponentDataType();
  }
  else aType = (SALOMEDS::SComponent::_narrow(GetCORBAImpl()))->ComponentDataType();

  return aType.ToCString();
}

bool SALOMEDS_SComponent::ComponentIOR(char* theID)
{
  bool ret;
  if(_isLocal) { 
    TCollection_AsciiString anIOR;
    ret = (Handle(SALOMEDSImpl_SComponent)::DownCast(GetLocalImpl()))->ComponentIOR(anIOR);
    theID = anIOR.ToCString();
  }
  else {
    ret = (SALOMEDS::SComponent::_narrow(GetCORBAImpl()))->ComponentIOR(theID);
  }

  return ret;
}
