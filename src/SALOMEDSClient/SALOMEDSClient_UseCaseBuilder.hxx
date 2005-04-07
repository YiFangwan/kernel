//  File   : SALOMEDSClient_UseCaseBuilder.hxx
//  Author : Sergey RUIN
//  Module : SALOME

#ifndef __SALOMEDSClient_USECaseBuilder_H__
#define __SALOMEDSClient_USECaseBuilder_H__

#include "SALOMEDSClient_UseCaseIterator.hxx"
#include "SALOMEDSClient_SObject.hxx"
#include <string> 

class SALOMEDSClient_UseCaseBuilder
{
public:
  virtual ~SALOMEDSClient_UseCaseBuilder() {}
  
  virtual bool Append(SALOMEDSClient_SObject* theObject) = 0;
  virtual bool Remove(SALOMEDSClient_SObject* theObject) = 0;
  virtual bool AppendTo(SALOMEDSClient_SObject* theFather, SALOMEDSClient_SObject* theObject) = 0;
  virtual bool InsertBefore(SALOMEDSClient_SObject* theFirst, SALOMEDSClient_SObject* theNext) = 0;
  virtual bool  SetCurrentObject(SALOMEDSClient_SObject* theObject) = 0;
  virtual bool SetRootCurrent() = 0;
  virtual bool  HasChildren(SALOMEDSClient_SObject* theObject) = 0;
  virtual bool  IsUseCase(SALOMEDSClient_SObject* theObject) = 0;
  virtual bool SetName(const std::string& theName) = 0;
  virtual SALOMEDSClient_SObject* GetCurrentObject() = 0;
  virtual std::string GetName() = 0;
  virtual SALOMEDSClient_SObject* AddUseCase(const std::string& theName) = 0;
  virtual SALOMEDSClient_UseCaseIterator* GetUseCaseIterator(SALOMEDSClient_SObject* anObject) = 0;

};
#endif
