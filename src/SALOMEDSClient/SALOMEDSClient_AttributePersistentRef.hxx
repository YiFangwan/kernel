//  File   : SALOMEDSClient_AttributePersistentRef.hxx
//  Author : Sergey RUIN
//  Module : SALOME

#ifndef SALOMEDSClient_AttributePersistentRef_HeaderFile
#define SALOMEDSClient_AttributePersistentRef_HeaderFile

#include "SALOMEDSClient_GenericAttribute.hxx" 

class SALOMEDSClient_AttributePersistentRef: public virtual SALOMEDSClient_GenericAttribute
{
public:
  
  virtual char* Value() = 0;
  virtual void SetValue(const char* value) = 0;

};



#endif
