//  File   : SALOMEDS_UseCaseBuilder.hxx
//  Author : Sergey RUIN
//  Module : SALOME

#ifndef __SALOMEDS_USECaseBuilder_H__
#define __SALOMEDS_USECaseBuilder_H__

#include "SALOMEDSClient_UseCaseIterator.hxx"
#include "SALOMEDSClient_UseCaseBuilder.hxx"
#include "SALOMEDSClient_SObject.hxx"
#include "SALOMEDSImpl_UseCaseBuilder.hxx"

// IDL headers
#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(SALOMEDS)

class SALOMEDS_UseCaseBuilder: public SALOMEDSClient_UseCaseBuilder
{

private: 
  bool _isLocal;
  Handle(SALOMEDSImpl_UseCaseBuilder) _local_impl;
  SALOMEDS::UseCaseBuilder_var        _corba_impl;

public:
  
  SALOMEDS_UseCaseBuilder(const Handle(SALOMEDSImpl_UseCaseBuilder)& theBuilder);
  SALOMEDS_UseCaseBuilder(SALOMEDS::UseCaseBuilder_ptr theBuilder);
  ~SALOMEDS_UseCaseBuilder();

  virtual bool Append(SALOMEDSClient_SObject* theObject);
  virtual bool Remove(SALOMEDSClient_SObject* theObject);
  virtual bool AppendTo(SALOMEDSClient_SObject* theFather, SALOMEDSClient_SObject* theObject);
  virtual bool InsertBefore(SALOMEDSClient_SObject* theFirst, SALOMEDSClient_SObject* theNext);
  virtual bool  SetCurrentObject(SALOMEDSClient_SObject* theObject);
  virtual bool SetRootCurrent();
  virtual bool  HasChildren(SALOMEDSClient_SObject* theObject);
  virtual bool  IsUseCase(SALOMEDSClient_SObject* theObject);
  virtual bool SetName(const char* theName);
  virtual SALOMEDSClient_SObject* GetCurrentObject();
  virtual char* GetName();
  virtual SALOMEDSClient_SObject* AddUseCase(const char* theName);
  virtual SALOMEDSClient_UseCaseIterator* GetUseCaseIterator(SALOMEDSClient_SObject* anObject);

};
#endif
