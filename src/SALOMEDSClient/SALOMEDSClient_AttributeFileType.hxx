//  File   : SALOMEDSClient_AttributeFileType.hxx
//  Author : Sergey RUIN
//  Module : SALOME

#ifndef SALOMEDSClient_AttributeFileType_HeaderFile
#define SALOMEDSClient_AttributeFileType_HeaderFile

#include "SALOMEDSClient_GenericAttribute.hxx" 

class SALOMEDSClient_AttributeFileType: public virtual SALOMEDSClient_GenericAttribute
{
public:
  virtual char* Value() = 0;
  virtual void SetValue(const char* value) = 0;
  
};



#endif
