//  File   : SALOMEDSClient_AttributeUserID.hxx
//  Author : Sergey RUIN
//  Module : SALOME

#ifndef SALOMEDSClient_AttributeUserID_HeaderFile
#define SALOMEDSClient_AttributeUserID_HeaderFile

#include "SALOMEDSClient_GenericAttribute.hxx" 

class SALOMEDSClient_AttributeUserID: public virtual SALOMEDSClient_GenericAttribute
{
public:
  
  virtual char* Value() = 0;
  virtual void SetValue(const char* value) = 0;

};




#endif
