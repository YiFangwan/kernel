//  File   : SALOMEDSClient_AttributeExternalFileDef.hxx
//  Author : Sergey RUIN
//  Module : SALOME

#ifndef SALOMEDSClient_AttributeExternalFileDef_HeaderFile
#define SALOMEDSClient_AttributeExternalFileDef_HeaderFile

#include "SALOMEDSClient_GenericAttribute.hxx" 

class SALOMEDSClient_AttributeExternalFileDef: public virtual SALOMEDSClient_GenericAttribute
{
public:
  virtual char* Value() = 0;
  virtual void SetValue(const char* value) = 0;
};



#endif
