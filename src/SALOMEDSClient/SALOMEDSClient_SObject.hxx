//  File   : SALOMEDSClient_SObject.hxx
//  Author : Sergey RUIN
//  Module : SALOME

#ifndef __SALOMEDSCLIENT_SOBJECT_H__
#define __SALOMEDSCLIENT_SOBJECT_H__

// std C++ headers
#include <vector>

class SALOMEDSClient_Study;
class SALOMEDSClient_SComponent;
class SALOMEDSClient_GenericAttribute;

class SALOMEDSClient_SObject
{
public:
  virtual char* GetID()  = 0;
  virtual SALOMEDSClient_SComponent* GetFatherComponent() = 0;
  virtual SALOMEDSClient_SObject*    GetFather() = 0;
  virtual bool FindAttribute(SALOMEDSClient_GenericAttribute* anAttribute, const char* aTypeOfAttribute) = 0;
  virtual bool ReferencedObject(SALOMEDSClient_SObject* theObject) = 0;
  virtual bool FindSubObject(int theTag, SALOMEDSClient_SObject* theObject) = 0;
  virtual SALOMEDSClient_Study* GetStudy() = 0;
  virtual char* Name() = 0;
  virtual void  Name(const char* theName)  = 0;
  virtual std::vector<SALOMEDSClient_GenericAttribute*> GetAllAttributes() = 0;
  virtual char* GetName() = 0;
  virtual char* GetComment() = 0;
  virtual char* GetIOR() = 0;
  virtual int   Tag() = 0;
  virtual int   Depth() = 0;
};
#endif
