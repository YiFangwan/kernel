//  File   : SALOMEDSClient_StudyManager.hxx
//  Author : Sergey RUIN
//  Module : SALOME

#ifndef __SALOMEDSClient_STUDYMANAGER_H__
#define __SALOMEDSClient_STUDYMANAGER_H__

#include <vector>
#include <string>

#include "SALOMEDSClient_Study.hxx"

class SALOMEDSClient_StudyManager
{
public:

  virtual SALOMEDSClient_Study* NewStudy(const std::string& study_name) = 0;
  virtual SALOMEDSClient_Study* Open(const std::string& theStudyUrl) = 0;
  virtual void Close( SALOMEDSClient_Study* theStudy) = 0;
  virtual void Save( SALOMEDSClient_Study* theStudy, bool theMultiFile) = 0;
  virtual void SaveASCII( SALOMEDSClient_Study* theStudy, bool theMultiFile) = 0;
  virtual void SaveAs(const std::string& theUrl,  SALOMEDSClient_Study* theStudy, bool theMultiFile) = 0;
  virtual void SaveAsASCII(const std::string& theUrl,  SALOMEDSClient_Study* theStudy, bool theMultiFile) = 0;
  virtual std::vector<std::string> GetOpenStudies() = 0;
  virtual SALOMEDSClient_Study* GetStudyByName(const std::string& theStudyName) = 0;
  virtual SALOMEDSClient_Study* GetStudyByID(int theStudyID) = 0;
  virtual bool CanCopy(SALOMEDSClient_SObject* theSO) = 0;
  virtual bool Copy(SALOMEDSClient_SObject* theSO) = 0;
  virtual bool CanPaste(SALOMEDSClient_SObject* theSO) = 0;
  virtual SALOMEDSClient_SObject* Paste(SALOMEDSClient_SObject* theSO) = 0;
  
};

#endif 
