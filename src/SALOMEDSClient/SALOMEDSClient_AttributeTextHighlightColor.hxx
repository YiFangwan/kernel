//  File   : SALOMEDSClient_AttributeTextHighlightColor.hxx
//  Author : Sergey RUIN
//  Module : SALOME

#ifndef SALOMEDSClient_AttributeTextHighlightColor_HeaderFile
#define SALOMEDSClient_AttributeTextHighlightColor_HeaderFile

#include <vector>
#include "SALOMEDSClient_GenericAttribute.hxx" 

class SALOMEDSClient_AttributeTextHighlightColor: public virtual SALOMEDSClient_GenericAttribute
{
public:
  virtual std::vector<double> TextHighlightColor() = 0;
  virtual void SetTextHighlightColor(const std::vector<double>& value) = 0;
};


#endif
