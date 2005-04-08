//  File   : SALOMEDS_Study.hxx
//  Author : Sergey RUIN
//  Module : SALOME

#ifndef __SALOMEDS_STUDY_H__
#define __SALOMEDS_STUDY_H__

#include <vector>
#include <string>

#include "SALOMEDSClient_Study.hxx"
#include "SALOMEDSClient_SComponentIterator.hxx"
#include "SALOMEDSClient_StudyBuilder.hxx"
#include "SALOMEDSClient_SObject.hxx"
#include "SALOMEDSClient_SComponent.hxx"
#include "SALOMEDSClient_UseCaseBuilder.hxx"
#include "SALOMEDSClient_AttributeStudyProperties.hxx"
#include "SALOMEDSClient_ChildIterator.hxx"

#include "SALOMEDSImpl_Study.hxx"

// IDL headers
#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(SALOMEDS)

class SALOMEDS_Study: public SALOMEDSClient_Study
{

private:
  bool                       _isLocal;
  Handle(SALOMEDSImpl_Study) _local_impl;
  SALOMEDS::Study_var        _corba_impl;
  CORBA::ORB_var             _orb;

public:

  SALOMEDS_Study(const Handle(SALOMEDSImpl_Study)& theStudy);
  SALOMEDS_Study(SALOMEDS::Study_ptr theStudy);
  ~SALOMEDS_Study();

  virtual std::string GetPersistentReference();
  virtual std::string GetTransientReference();
  virtual bool IsEmpty();
  virtual SALOMEDSClient_SComponent* FindComponent (const std::string& aComponentName);
  virtual SALOMEDSClient_SComponent* FindComponentID(const std::string& aComponentID);
  virtual SALOMEDSClient_SObject* FindObject(const std::string& anObjectName);
  virtual std::vector<SALOMEDSClient_SObject*> FindObjectByName( const std::string& anObjectName, const std::string& aComponentName ) ;  
  virtual SALOMEDSClient_SObject* FindObjectID(const std::string& anObjectID);
  virtual SALOMEDSClient_SObject* CreateObjectID(const std::string& anObjectID);
  virtual SALOMEDSClient_SObject* FindObjectIOR(const std::string& anObjectIOR);
  virtual SALOMEDSClient_SObject* FindObjectByPath(const std::string& thePath);
  virtual std::string GetObjectPath(SALOMEDSClient_SObject* theSO);
  virtual void SetContext(const std::string& thePath);
  virtual std::string GetContext();  
  virtual std::vector<std::string> GetObjectNames(const std::string& theContext);
  virtual std::vector<std::string> GetDirectoryNames(const std::string& theContext);
  virtual std::vector<std::string> GetFileNames(const std::string& theContext);
  virtual std::vector<std::string> GetComponentNames(const std::string& theContext);
  virtual SALOMEDSClient_ChildIterator* NewChildIterator(SALOMEDSClient_SObject* theSO);
  virtual SALOMEDSClient_SComponentIterator* NewComponentIterator();
  virtual SALOMEDSClient_StudyBuilder* NewBuilder();
  virtual std::string Name();
  virtual void  Name(const std::string& name);
  virtual bool IsSaved();
  virtual void  IsSaved(bool save);
  virtual bool IsModified();
  virtual std::string URL();
  virtual void  URL(const std::string& url);
  virtual int StudyId();
  virtual void  StudyId(int id);
  virtual std::vector<SALOMEDSClient_SObject*> FindDependances(SALOMEDSClient_SObject* theSO);
  virtual SALOMEDSClient_AttributeStudyProperties* GetProperties();
  virtual std::string GetLastModificationDate();
  virtual std::vector<std::string> GetModificationsDate();
  virtual SALOMEDSClient_UseCaseBuilder* GetUseCaseBuilder();
  virtual void Close();
  virtual void EnableUseCaseAutoFilling(bool isEnabled);
  virtual bool DumpStudy(const std::string& thePath, const std::string& theBaseName, bool isPublished); 

  std::string ConvertObjectToIOR(CORBA::Object_ptr theObject);
  CORBA::Object_ptr ConvertIORToObject(const std::string& theIOR);     

  SALOMEDS::Study_ptr GetStudy();

private:
  void init_orb();

};
#endif
