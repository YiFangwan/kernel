//  File   : SALOMEDSClient_Study.hxx
//  Author : Sergey RUIN
//  Module : SALOME

#ifndef __SALOMEDSClient_STUDY_H__
#define __SALOMEDSClient_STUDY_H__

#include <vector>
#include <string>

#include "SALOMEDSClient_SComponentIterator.hxx"
#include "SALOMEDSClient_StudyBuilder.hxx"
#include "SALOMEDSClient_SObject.hxx"
#include "SALOMEDSClient_SComponent.hxx"
#include "SALOMEDSClient_UseCaseBuilder.hxx"
#include "SALOMEDSClient_AttributeStudyProperties.hxx"
#include "SALOMEDSClient_ChildIterator.hxx"

class SALOMEDSClient_Study
{

public:

  virtual char* GetPersistentReference() = 0;
  virtual char* GetTransientReference() = 0;
  virtual bool IsEmpty() = 0;
  virtual SALOMEDSClient_SComponent* FindComponent (const char* aComponentName) = 0;
  virtual SALOMEDSClient_SComponent* FindComponentID(const char* aComponentID) = 0;
  virtual SALOMEDSClient_SObject* FindObject(const char* anObjectName) = 0;
  virtual std::vector<SALOMEDSClient_SObject*> FindObjectByName( const char* anObjectName, const char* aComponentName ) = 0;  
  virtual SALOMEDSClient_SObject* FindObjectID(const char* anObjectID) = 0;
  virtual SALOMEDSClient_SObject* CreateObjectID(const char* anObjectID) = 0;
  virtual SALOMEDSClient_SObject* FindObjectIOR(const char* anObjectIOR) = 0;
  virtual SALOMEDSClient_SObject* FindObjectByPath(const char* thePath) = 0;
  virtual char* GetObjectPath(SALOMEDSClient_SObject* theSO) = 0;
  virtual void SetContext(const char* thePath) = 0;
  virtual char* GetContext() = 0;  
  virtual std::vector<std::string> GetObjectNames(const char* theContext) = 0;
  virtual std::vector<std::string> GetDirectoryNames(const char* theContext) = 0;
  virtual std::vector<std::string> GetFileNames(const char* theContext) = 0;
  virtual std::vector<std::string> GetComponentNames(const char* theContext) = 0;
  virtual SALOMEDSClient_ChildIterator* NewChildIterator(SALOMEDSClient_SObject* theSO) = 0;
  virtual SALOMEDSClient_SComponentIterator* NewComponentIterator() = 0;
  virtual SALOMEDSClient_StudyBuilder* NewBuilder() = 0;
  virtual char* Name() = 0;
  virtual void  Name(const char* name) = 0;
  virtual bool IsSaved() = 0;
  virtual void  IsSaved(bool save) = 0;
  virtual bool IsModified() = 0;
  virtual char* URL() = 0;
  virtual void  URL(const char* url) = 0;
  virtual int StudyId() = 0;
  virtual void  StudyId(int id) = 0;
  virtual std::vector<SALOMEDSClient_SObject*> FindDependances(SALOMEDSClient_SObject* theSO) = 0;
  virtual SALOMEDSClient_AttributeStudyProperties* GetProperties() = 0;
  virtual char* GetLastModificationDate() = 0;
  virtual std::vector<std::string> GetModificationsDate() = 0;
  virtual SALOMEDSClient_UseCaseBuilder* GetUseCaseBuilder() = 0;
  virtual void Close() = 0;
  virtual void EnableUseCaseAutoFilling(bool isEnabled) = 0;
  virtual bool DumpStudy(const char* thePath, const char* theBaseName, bool isPublished) = 0; 
};
#endif
