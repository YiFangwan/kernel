//  File   : SALOMEDSClient_AttributePixMap.hxx
//  Author : Sergey RUIN
//  Module : SALOME

#ifndef SALOMEDSClient_AttributePixMap_HeaderFile
#define SALOMEDSClient_AttributePixMap_HeaderFile

#include "SALOMEDSClient_GenericAttribute.hxx" 

class SALOMEDSClient_AttributePixMap: public virtual SALOMEDSClient_GenericAttribute
{
public:

  virtual bool HasPixMap() = 0;
  virtual char* GetPixMap() = 0;
  virtual void SetPixMap(const char* value) = 0;

};


#endif
