//  File   : SALOMEDSClient_AttributeTextColor.hxx
//  Author : Sergey RUIN
//  Module : SALOME

#ifndef SALOMEDSClient_AttributeTextColor_HeaderFile
#define SALOMEDSClient_AttributeTextColor_HeaderFile

#include <vector>
#include "SALOMEDSClient_GenericAttribute.hxx" 

class SALOMEDSClient_AttributeTextColor: public virtual SALOMEDSClient_GenericAttribute
{
public:

  virtual std::vector<double> TextColor() = 0;
  virtual void SetTextColor(const std::vector<double>& value) = 0;

};


#endif
