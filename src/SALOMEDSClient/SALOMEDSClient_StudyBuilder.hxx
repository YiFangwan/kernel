//  File   : SALOMEDSClient_StudyBuilder.hxx
//  Author : Sergey RUIN
//  Module : SALOME

#ifndef __SALOMEDSClient_STUDYBUILDER_H__
#define __SALOMEDSClient_STUDYBUILDER_H__

#include "SALOMEDSClient_SObject.hxx"
#include "SALOMEDSClient_SComponent.hxx"
#include "SALOMEDSClient_GenericAttribute.hxx"

class SALOMEDSClient_StudyBuilder
{
public:
  virtual SALOMEDSClient_SComponent* NewComponent(const char* ComponentDataType) = 0;
  virtual void DefineComponentInstance (SALOMEDSClient_SComponent*, const char* ComponentIOR) = 0;
  virtual void RemoveComponent(SALOMEDSClient_SComponent* theSCO) = 0;
  virtual SALOMEDSClient_SObject* NewObject(SALOMEDSClient_SObject* theFatherObject) = 0;
  virtual SALOMEDSClient_SObject* NewObjectToTag(SALOMEDSClient_SObject* theFatherObject, int theTag) = 0;
  virtual void AddDirectory(const char* thePath) = 0;
  virtual void LoadWith(SALOMEDSClient_SComponent* theSCO, const char* theIOR) = 0;
  virtual void Load(SALOMEDSClient_SObject* theSCO) = 0;
  virtual void RemoveObject(SALOMEDSClient_SObject* theSO) = 0;
  virtual void RemoveObjectWithChildren(SALOMEDSClient_SObject* theSO) = 0;
  virtual SALOMEDSClient_GenericAttribute* FindOrCreateAttribute(SALOMEDSClient_SObject* theSO, const char* aTypeOfAttribute) = 0;
  virtual bool FindAttribute(SALOMEDSClient_SObject* theSO, 
                             SALOMEDSClient_GenericAttribute* theAttribute, 
			     const char* aTypeOfAttribute) = 0;
  virtual void RemoveAttribute(SALOMEDSClient_SObject* theSO, const char* aTypeOfAttribute) = 0;
  virtual void Addreference(SALOMEDSClient_SObject* me, SALOMEDSClient_SObject* thereferencedObject) = 0;
  virtual void RemoveReference(SALOMEDSClient_SObject* me) = 0;
  virtual void SetGUID(SALOMEDSClient_SObject* theSO, const char* theGUID) = 0;
  virtual bool IsGUID(SALOMEDSClient_SObject* theSO, const char* theGUID) = 0;
  virtual void NewCommand() = 0;
  virtual void CommitCommand()= 0;
  virtual bool HasOpenCommand() = 0;
  virtual void AbortCommand() = 0;
  virtual void Undo() = 0;
  virtual void Redo() = 0;
  virtual bool GetAvailableUndos() = 0;
  virtual bool GetAvailableRedos() = 0;
  virtual int UndoLimit() = 0;
  virtual void UndoLimit(int theLimit) = 0;
  virtual void SetName(SALOMEDSClient_SObject* theSO, const char* theValue) = 0;
  virtual void SetComment(SALOMEDSClient_SObject* theSO, const char* theValue) = 0;
  virtual void SetIOR(SALOMEDSClient_SObject* theSO, const char* theValue) = 0;
};
#endif
