//  File   : SALOMEDS_AttributeTextHighlightColor.cxx
//  Author : Sergey RUIN
//  Module : SALOME

#include "SALOMEDS_AttributeTextHighlightColor.hxx"

#include <TColStd_HArray1OfReal.hxx>

SALOMEDS_AttributeTextHighlightColor::SALOMEDS_AttributeTextHighlightColor(const Handle(SALOMEDSImpl_AttributeTextHighlightColor)& theAttr)
:SALOMEDS_GenericAttribute(theAttr)
{}

SALOMEDS_AttributeTextHighlightColor::SALOMEDS_AttributeTextHighlightColor(SALOMEDS::AttributeTextHighlightColor_ptr theAttr)
:SALOMEDS_GenericAttribute(theAttr)
{}

SALOMEDS_AttributeTextHighlightColor::~SALOMEDS_AttributeTextHighlightColor()
{}


std::vector<double> SALOMEDS_AttributeTextHighlightColor::TextHighlightColor()
{
  std::vector<double> aVector;
  int i, aLength;
  if(_isLocal) {
    Handle(TColStd_HArray1OfReal) aSeq = Handle(SALOMEDSImpl_AttributeTextHighlightColor)::DownCast(_local_impl)->TextHighlightColor();
    aLength = aSeq->Length();
    for(i=1; i<=aLength; i++) aVector.push_back(aSeq->Value(i));
  }
  else {
    SALOMEDS::Color aColor = SALOMEDS::AttributeTextHighlightColor::_narrow(_corba_impl)->TextHighlightColor();
    aVector.push_back(aColor.R);
    aVector.push_back(aColor.G);
    aVector.push_back(aColor.B);
  }
  return aVector;
}
 
void SALOMEDS_AttributeTextHighlightColor::SetTextHighlightColor(const std::vector<double>& value)
{
  CheckLocked();
  int i;
  if(_isLocal) {
    Handle(TColStd_HArray1OfReal) aSeq = new TColStd_HArray1OfReal(1, 3);
    for(i=0; i<3; i++) aSeq->SetValue(i+1, value[i]);
    Handle(SALOMEDSImpl_AttributeTextHighlightColor)::DownCast(_local_impl)->ChangeArray(aSeq);
  }
  else {
    SALOMEDS::Color aColor;
    aColor.R = value[0];
    aColor.G = value[1];
    aColor.B = value[2];  
    SALOMEDS::AttributeTextHighlightColor::_narrow(_corba_impl)->SetTextHighlightColor(aColor);
  }
}
