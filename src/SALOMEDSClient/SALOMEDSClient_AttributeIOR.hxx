//  File   : SALOMEDSClient_AttributeIOR.hxx
//  Author : Sergey RUIN
//  Module : SALOME

#ifndef SALOMEDSClient_AttributeIOR_HeaderFile
#define SALOMEDSClient_AttributeIOR_HeaderFile

#include "SALOMEDSClient_GenericAttribute.hxx" 

class SALOMEDSClient_AttributeIOR: public virtual SALOMEDSClient_GenericAttribute
{
public:
  
  virtual char* Value() = 0;
  virtual void SetValue(const char* value) = 0;
  
};


#endif
