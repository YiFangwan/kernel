//  File   : SALOMEDS_StudyManager.hxx
//  Author : Sergey RUIN
//  Module : SALOME

#ifndef __SALOMEDS_STUDYMANAGER_H__
#define __SALOMEDS_STUDYMANAGER_H__

#include <vector>
#include <string>

#include "SALOMEDSClient_StudyManager.hxx" 
#include "SALOMEDSClient_Study.hxx"
#include "SALOMEDSImpl_StudyManager.hxx"

// IDL headers

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(SALOMEDS)

class SALOMEDS_StudyManager: public SALOMEDSClient_StudyManager
{
private:
  bool                              _isLocal;
  Handle(SALOMEDSImpl_StudyManager) _local_impl;
  SALOMEDS::StudyManager_var        _corba_impl;  
  CORBA::ORB_var                    _orb;

public:

  SALOMEDS_StudyManager(SALOMEDS::StudyManager_ptr theManager);
  ~SALOMEDS_StudyManager();

  virtual SALOMEDSClient_Study* NewStudy(const std::string& study_name);
  virtual SALOMEDSClient_Study* Open(const std::string& theStudyUrl);
  virtual void Close( SALOMEDSClient_Study* theStudy);
  virtual void Save( SALOMEDSClient_Study* theStudy, bool theMultiFile);
  virtual void SaveASCII( SALOMEDSClient_Study* theStudy, bool theMultiFile);
  virtual void SaveAs(const std::string& theUrl,  SALOMEDSClient_Study* theStudy, bool theMultiFile);
  virtual void SaveAsASCII(const std::string& theUrl,  SALOMEDSClient_Study* theStudy, bool theMultiFile);
  virtual std::vector<std::string> GetOpenStudies();
  virtual SALOMEDSClient_Study* GetStudyByName(const std::string& theStudyName) ;
  virtual SALOMEDSClient_Study* GetStudyByID(int theStudyID) ;
  virtual bool CanCopy(SALOMEDSClient_SObject* theSO);
  virtual bool Copy(SALOMEDSClient_SObject* theSO);
  virtual bool CanPaste(SALOMEDSClient_SObject* theSO);
  virtual SALOMEDSClient_SObject* Paste(SALOMEDSClient_SObject* theSO); 

private:
  void init_orb();
};

#endif 
