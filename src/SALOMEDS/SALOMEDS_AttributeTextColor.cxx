//  File   : SALOMEDS_AttributeTextColor.cxx
//  Author : Sergey RUIN
//  Module : SALOME

#include "SALOMEDS_AttributeTextColor.hxx"

#include <TColStd_HArray1OfReal.hxx>

SALOMEDS_AttributeTextColor::SALOMEDS_AttributeTextColor(const Handle(SALOMEDSImpl_AttributeTextColor)& theAttr)
:SALOMEDS_GenericAttribute(theAttr)
{}

SALOMEDS_AttributeTextColor::SALOMEDS_AttributeTextColor(SALOMEDS::AttributeTextColor_ptr theAttr)
:SALOMEDS_GenericAttribute(theAttr)
{}

SALOMEDS_AttributeTextColor::~SALOMEDS_AttributeTextColor()
{}


std::vector<double> SALOMEDS_AttributeTextColor::TextColor()
{
  std::vector<double> aVector;
  int i, aLength;
  if(_isLocal) {
    Handle(TColStd_HArray1OfReal) aSeq = Handle(SALOMEDSImpl_AttributeTextColor)::DownCast(_local_impl)->TextColor();
    aLength = aSeq->Length();
    for(i=1; i<=aLength; i++) aVector.push_back(aSeq->Value(i));
  }
  else {
    SALOMEDS::Color aColor = SALOMEDS::AttributeTextColor::_narrow(_corba_impl)->TextColor();
    aVector.push_back(aColor.R);
    aVector.push_back(aColor.G);
    aVector.push_back(aColor.B);
  }
  return aVector;
}
 
void SALOMEDS_AttributeTextColor::SetTextColor(const std::vector<double>& value)
{
  CheckLocked();
  int i;
  if(_isLocal) {
    Handle(TColStd_HArray1OfReal) aSeq = new TColStd_HArray1OfReal(1, 3);
    for(i=0; i<3; i++) aSeq->SetValue(i+1, value[i]);
    Handle(SALOMEDSImpl_AttributeTextColor)::DownCast(_local_impl)->ChangeArray(aSeq);
  }
  else {
    SALOMEDS::Color aColor;
    aColor.R = value[0];
    aColor.G = value[1];
    aColor.B = value[2];
    SALOMEDS::AttributeTextColor::_narrow(_corba_impl)->SetTextColor(aColor);
  }
}
