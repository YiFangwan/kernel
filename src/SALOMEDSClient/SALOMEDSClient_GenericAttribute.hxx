//  File   : SALOMEDSClient__GenericAttribute.hxx
//  Author : Sergey RUIN
//  Module : SALOME

#ifndef _GENERICCLIENT_ATTRIBUTE_HXX_
#define _GENERICCLIENT_ATTRIBUTE_HXX_

#include <string> 

#include "SALOMEDSClient_SObject.hxx"

class SALOMEDSClient_GenericAttribute
{
public:
  
  virtual void CheckLocked() = 0;
  virtual std::string Type() = 0;
  virtual std::string GetClassType() = 0;
  virtual SALOMEDSClient_SObject* GetSObject() = 0;
};

#endif
