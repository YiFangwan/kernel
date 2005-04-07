//  File   : SALOMEDS_StudyBuilder.hxx
//  Author : Sergey RUIN
//  Module : SALOME

#ifndef __SALOMEDS_STUDYBUILDER_H__
#define __SALOMEDS_STUDYBUILDER_H__

#include "SALOMEDSClient_StudyBuilder.hxx"
#include "SALOMEDSClient_SObject.hxx"
#include "SALOMEDSClient_SComponent.hxx"
#include "SALOMEDSClient_GenericAttribute.hxx"
#include "SALOMEDSImpl_StudyBuilder.hxx"

// IDL headers
#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(SALOMEDS)


class SALOMEDS_StudyBuilder: public SALOMEDSClient_StudyBuilder
{
private:
  bool _isLocal;
  Handle(SALOMEDSImpl_StudyBuilder) _local_impl;
  SALOMEDS::StudyBuilder_var        _corba_impl;
  CORBA::ORB_var                    _orb;

public:

  SALOMEDS_StudyBuilder(const Handle(SALOMEDSImpl_StudyBuilder)& theBuilder);
  SALOMEDS_StudyBuilder(SALOMEDS::StudyBuilder_ptr theBuilder);
  ~SALOMEDS_StudyBuilder();

  virtual SALOMEDSClient_SComponent* NewComponent(const std::string& ComponentDataType);
  virtual void DefineComponentInstance (SALOMEDSClient_SComponent*, const std::string& ComponentIOR);
  virtual void RemoveComponent(SALOMEDSClient_SComponent* theSCO);
  virtual SALOMEDSClient_SObject* NewObject(SALOMEDSClient_SObject* theFatherObject);
  virtual SALOMEDSClient_SObject* NewObjectToTag(SALOMEDSClient_SObject* theFatherObject, int theTag);
  virtual void AddDirectory(const std::string& thePath);
  virtual void LoadWith(SALOMEDSClient_SComponent* theSCO, const std::string& theIOR);
  virtual void Load(SALOMEDSClient_SObject* theSCO);
  virtual void RemoveObject(SALOMEDSClient_SObject* theSO);
  virtual void RemoveObjectWithChildren(SALOMEDSClient_SObject* theSO);
  virtual SALOMEDSClient_GenericAttribute* FindOrCreateAttribute(SALOMEDSClient_SObject* theSO, 
								 const std::string& aTypeOfAttribute);
  virtual bool FindAttribute(SALOMEDSClient_SObject* theSO, 
                             SALOMEDSClient_GenericAttribute* theAttribute, 
			     const std::string& aTypeOfAttribute);
  virtual void RemoveAttribute(SALOMEDSClient_SObject* theSO, const std::string& aTypeOfAttribute);
  virtual void Addreference(SALOMEDSClient_SObject* me, SALOMEDSClient_SObject* thereferencedObject);
  virtual void RemoveReference(SALOMEDSClient_SObject* me);
  virtual void SetGUID(SALOMEDSClient_SObject* theSO, const std::string& theGUID);
  virtual bool IsGUID(SALOMEDSClient_SObject* theSO, const std::string& theGUID);
  virtual void NewCommand();
  virtual void CommitCommand();
  virtual bool HasOpenCommand();
  virtual void AbortCommand();
  virtual void Undo();
  virtual void Redo();
  virtual bool GetAvailableUndos();
  virtual bool GetAvailableRedos();
  virtual int UndoLimit();
  virtual void UndoLimit(int theLimit);
  virtual void SetName(SALOMEDSClient_SObject* theSO, const std::string& theValue);
  virtual void SetComment(SALOMEDSClient_SObject* theSO, const std::string& theValue);
  virtual void SetIOR(SALOMEDSClient_SObject* theSO, const std::string& theValue);

private:
  void CheckLocked();
  void init_orb();
};
#endif
