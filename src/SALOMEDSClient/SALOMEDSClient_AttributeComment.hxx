//  File   : SALOMEDSClient_AttributeComment.hxx
//  Author : Sergey RUIN
//  Module : SALOME

#ifndef SALOMEDSClient_AttributeComment_HeaderFile
#define SALOMEDSClient_AttributeComment_HeaderFile

#include "SALOMEDSClient_GenericAttribute.hxx" 

class SALOMEDSClient_AttributeComment: public virtual SALOMEDSClient_GenericAttribute
{
public:  
  virtual char* Value() = 0;
  virtual void SetValue(const char* value) = 0;
};

#endif
