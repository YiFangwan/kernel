//  File   : SALOMEDSClient_AttributeName.hxx
//  Author : Sergey RUIN
//  Module : SALOME
//  $Header:

#ifndef SALOMEDSClient_AttributeName_HeaderFile
#define SALOMEDSClient_AttributeName_HeaderFile

#include "SALOMEDSClient_GenericAttribute.hxx" 

class SALOMEDSClient_AttributeName: public virtual SALOMEDSClient_GenericAttribute
{
public:

  virtual char* Value() = 0;
  virtual void SetValue(const char* value) = 0;

};




#endif
