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

  virtual char* GetPersistentReference();
  virtual char* GetTransientReference();
  virtual bool IsEmpty();
  virtual SALOMEDSClient_SComponent* FindComponent (const char* aComponentName);
  virtual SALOMEDSClient_SComponent* FindComponentID(const char* aComponentID);
  virtual SALOMEDSClient_SObject* FindObject(const char* anObjectName);
  virtual std::vector<SALOMEDSClient_SObject*> FindObjectByName( const char* anObjectName, const char* aComponentName ) ;  
  virtual SALOMEDSClient_SObject* FindObjectID(const char* anObjectID);
  virtual SALOMEDSClient_SObject* CreateObjectID(const char* anObjectID);
  virtual SALOMEDSClient_SObject* FindObjectIOR(const char* anObjectIOR);
  virtual SALOMEDSClient_SObject* FindObjectByPath(const char* thePath);
  virtual char* GetObjectPath(SALOMEDSClient_SObject* theSO);
  virtual void SetContext(const char* thePath);
  virtual char* GetContext();  
  virtual std::vector<std::string> GetObjectNames(const char* theContext);
  virtual std::vector<std::string> GetDirectoryNames(const char* theContext);
  virtual std::vector<std::string> GetFileNames(const char* theContext);
  virtual std::vector<std::string> GetComponentNames(const char* theContext);
  virtual SALOMEDSClient_ChildIterator* NewChildIterator(SALOMEDSClient_SObject* theSO);
  virtual SALOMEDSClient_SComponentIterator* NewComponentIterator();
  virtual SALOMEDSClient_StudyBuilder* NewBuilder();
  virtual char* Name();
  virtual void  Name(const char* name);
  virtual bool IsSaved();
  virtual void  IsSaved(bool save);
  virtual bool IsModified();
  virtual char* URL();
  virtual void  URL(const char* url);
  virtual int StudyId();
  virtual void  StudyId(int id);
  virtual std::vector<SALOMEDSClient_SObject*> FindDependances(SALOMEDSClient_SObject* theSO);
  virtual SALOMEDSClient_AttributeStudyProperties* GetProperties();
  virtual char* GetLastModificationDate();
  virtual std::vector<std::string> GetModificationsDate();
  virtual SALOMEDSClient_UseCaseBuilder* GetUseCaseBuilder();
  virtual void Close();
  virtual void EnableUseCaseAutoFilling(bool isEnabled);
  virtual bool DumpStudy(const char* thePath, const char* theBaseName, bool isPublished); 

  char* ConvertObjectToIOR(CORBA::Object_ptr theObject);
  CORBA::Object_ptr ConvertIORToObject(const char* theIOR);     

private:
  void init_orb();

};
#endif
