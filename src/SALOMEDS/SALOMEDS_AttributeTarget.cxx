//  File   : SALOMEDS_AttributeTarget.cxx
//  Author : Sergey RUIN
//  Module : SALOME

#include "SALOMEDS_AttributeTarget.hxx"

#include <TColStd_HSequenceOfTransient.hxx>

#include "SALOMEDSImpl_SObject.hxx"
#include "SALOMEDS_SObject.hxx"


SALOMEDS_AttributeTarget::SALOMEDS_AttributeTarget(const Handle(SALOMEDSImpl_AttributeTarget)& theAttr)
:SALOMEDS_GenericAttribute(theAttr)
{}

SALOMEDS_AttributeTarget::SALOMEDS_AttributeTarget(SALOMEDS::AttributeTarget_ptr theAttr)
:SALOMEDS_GenericAttribute(theAttr)
{}

SALOMEDS_AttributeTarget::~SALOMEDS_AttributeTarget()
{}

void SALOMEDS_AttributeTarget::Add(SALOMEDSClient_SObject* theObject)
{
  SALOMEDS_SObject* aSO = dynamic_cast<SALOMEDS_SObject*>(theObject);

  if(_isLocal) Handle(SALOMEDSImpl_AttributeTarget)::DownCast(_local_impl)->Add(aSO->GetLocalImpl());
  else SALOMEDS::AttributeTarget::_narrow(_corba_impl)->Add(aSO->GetCORBAImpl());
}

std::vector<SALOMEDSClient_SObject*> SALOMEDS_AttributeTarget::Get()
{
  std::vector<SALOMEDSClient_SObject*> aVector;
  int aLength, i;
  SALOMEDS_SObject* aSO = NULL;
  
  if(_isLocal) {
    Handle(TColStd_HSequenceOfTransient) aSeq = Handle(SALOMEDSImpl_AttributeTarget)::DownCast(_local_impl)->Get();
    aLength = aSeq->Length();
    for(i=1; i<=aLength; i++) {
      aSO = new SALOMEDS_SObject(Handle(SALOMEDSImpl_SObject)::DownCast(aSeq->Value(i)));
      aVector.push_back(aSO);
    }
  }
  else {
    SALOMEDS::Study::ListOfSObject_var aSeq = SALOMEDS::AttributeTarget::_narrow(_corba_impl)->Get();
    aLength = aSeq->length();
    aSO = new SALOMEDS_SObject(aSeq[i].in());
    for(i = 0; i<aLength; i++) aVector.push_back(aSO);
  }

  return aVector;
}

void SALOMEDS_AttributeTarget::Remove(SALOMEDSClient_SObject* theObject)
{
  SALOMEDS_SObject* aSO = dynamic_cast<SALOMEDS_SObject*>(theObject);

  if(_isLocal) Handle(SALOMEDSImpl_AttributeTarget)::DownCast(_local_impl)->Remove(aSO->GetLocalImpl());
  else SALOMEDS::AttributeTarget::_narrow(_corba_impl)->Remove(aSO->GetCORBAImpl());
}

