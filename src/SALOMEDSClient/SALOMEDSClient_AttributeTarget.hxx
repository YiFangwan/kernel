//  File   : SALOMEDSClient_AttributeTarget.hxx
//  Author : Sergey RUIN
//  Module : SALOME

#ifndef SALOMEDSClient_AttributeTarget_HeaderFile
#define SALOMEDSClient_AttributeTarget_HeaderFile

#include <vector>
#include "SALOMEDSClient_SObject.hxx"
#include "SALOMEDSClient_GenericAttribute.hxx" 

class SALOMEDSClient_AttributeTarget: public virtual SALOMEDSClient_GenericAttribute
{
public:
  virtual void Add(SALOMEDSClient_SObject* theObject) = 0;
  virtual std::vector<SALOMEDSClient_SObject*> Get() = 0;
  virtual void Remove(SALOMEDSClient_SObject* theObject) = 0;
};

#endif
