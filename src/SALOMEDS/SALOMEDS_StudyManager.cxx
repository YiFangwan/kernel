//  File   : SALOMEDSClient_StudyManager.cxx
//  Author : Sergey RUIN
//  Module : SALOME

using namespace std; 
#include "SALOMEDS_StudyManager.hxx"

#include "SALOMEDSImpl_Study.hxx"
#include "SALOMEDS_Study.hxx"
#include "SALOMEDS_SObject.hxx"

#ifdef WNT
#include <process.h>
#else
#include <sys/types.h>
#include <unistd.h>
#endif

#include <TCollection_AsciiString.hxx>
#include <TColStd_HSequenceOfTransient.hxx>

#include "OpUtil.hxx"

SALOMEDS_StudyManager::SALOMEDS_StudyManager(SALOMEDS::StudyManager_ptr theManager)
{

#ifdef WNT
  long pid =  (long)_getpid();
#else
  long pid =  (long)getpid();
#endif  

  long addr = theManager->GetLocalImpl(GetHostname().c_str(), pid, _isLocal);
  if(_isLocal) {
    _local_impl = ((SALOMEDSImpl_StudyManager*)(addr));
    _corba_impl = SALOMEDS::StudyManager::_duplicate(theManager);
  }
  else {
    _local_impl = NULL;
    _corba_impl = SALOMEDS::StudyManager::_duplicate(theManager);
  }
}

SALOMEDS_StudyManager::~SALOMEDS_StudyManager()
{
}

SALOMEDSClient_Study* SALOMEDS_StudyManager::NewStudy(const char* study_name)
{
  SALOMEDS_Study* aStudy;
  TCollection_AsciiString aName((char*)study_name);

  if(_isLocal) aStudy = new SALOMEDS_Study(_local_impl->NewStudy(aName));
  else aStudy = new SALOMEDS_Study(_corba_impl->NewStudy(aName.ToCString()));

  return aStudy;
}

SALOMEDSClient_Study* SALOMEDS_StudyManager::Open(const char* theStudyUrl)
{
  SALOMEDS_Study* aStudy;
  TCollection_AsciiString aName((char*)theStudyUrl);

  if(_isLocal) aStudy = new SALOMEDS_Study(_local_impl->Open(aName));
  else aStudy = new SALOMEDS_Study(_corba_impl->Open(aName.ToCString()));

  return aStudy;
}
 
void SALOMEDS_StudyManager::Close( SALOMEDSClient_Study* theStudy)
{
  //SRN: Pure CORBA close as it does more cleaning then the local one
  SALOMEDS::Study_var aStudy = _corba_impl->GetStudyByID(theStudy->StudyId());
  _corba_impl->Close(aStudy);
}
 
void SALOMEDS_StudyManager::Save( SALOMEDSClient_Study* theStudy, bool theMultiFile)
{
  //SRN: Pure CORBA save as the save operation require CORBA in any case 
  SALOMEDS::Study_var aStudy = _corba_impl->GetStudyByID(theStudy->StudyId());
  _corba_impl->Save(aStudy, theMultiFile);
}
 
void SALOMEDS_StudyManager::SaveASCII( SALOMEDSClient_Study* theStudy, bool theMultiFile)
{
  //SRN: Pure CORBA save as the save operation require CORBA in any case 
  SALOMEDS::Study_var aStudy = _corba_impl->GetStudyByID(theStudy->StudyId());
  _corba_impl->SaveASCII(aStudy, theMultiFile);
}
 
void SALOMEDS_StudyManager::SaveAs(const char* theUrl,  SALOMEDSClient_Study* theStudy, bool theMultiFile)
{
  //SRN: Pure CORBA save as the save operation require CORBA in any case 
  TCollection_AsciiString anURL((char*)theUrl);
  SALOMEDS::Study_var aStudy = _corba_impl->GetStudyByID(theStudy->StudyId());
  _corba_impl->SaveAs(anURL.ToCString(), aStudy, theMultiFile);
}
 
void SALOMEDS_StudyManager::SaveAsASCII(const char* theUrl,  SALOMEDSClient_Study* theStudy, bool theMultiFile)
{
  //SRN: Pure CORBA save as the save operation require CORBA in any case 
  TCollection_AsciiString anURL((char*)theUrl);
  SALOMEDS::Study_var aStudy = _corba_impl->GetStudyByID(theStudy->StudyId());
  _corba_impl->SaveAsASCII(anURL.ToCString(), aStudy, theMultiFile);
}

std::vector<std::string> SALOMEDS_StudyManager::GetOpenStudies()
{
  std::vector<std::string> aVector;
  int aLength, i;

  if(_isLocal) {
    Handle(TColStd_HSequenceOfTransient) aSeq = _local_impl->GetOpenStudies();
    aLength = aSeq->Length();
    for(i = 1; i <= aLength; i++) 
      aVector.push_back(Handle(SALOMEDSImpl_Study)::DownCast(aSeq->Value(i))->Name().ToCString());
  }
  else {
    SALOMEDS::ListOfOpenStudies_var aSeq = _corba_impl->GetOpenStudies();
    aLength = aSeq->length();
    for(i = 0; i < aLength; i++) 
      aVector.push_back(aSeq[i].in());
  }
  return aVector;
}
 
SALOMEDSClient_Study* SALOMEDS_StudyManager::GetStudyByName(const char* theStudyName) 
{
  SALOMEDS_Study* aStudy = NULL;
  TCollection_AsciiString aName((char*)theStudyName);
  if(_isLocal) aStudy = new SALOMEDS_Study(_local_impl->GetStudyByName(aName));
  else aStudy = new SALOMEDS_Study(_corba_impl->GetStudyByName(aName.ToCString()));
  return aStudy;
}

SALOMEDSClient_Study* SALOMEDS_StudyManager::GetStudyByID(int theStudyID) 
{
  SALOMEDS_Study* aStudy = NULL;
  if(_isLocal) aStudy = new SALOMEDS_Study(_local_impl->GetStudyByID(theStudyID));
  else aStudy = new SALOMEDS_Study(_corba_impl->GetStudyByID(theStudyID));
  return aStudy; 
}
 
bool SALOMEDS_StudyManager::CanCopy(SALOMEDSClient_SObject* theSO)
{
  SALOMEDS_SObject* aSO = dynamic_cast<SALOMEDS_SObject*>(theSO);
  //SRN: Pure CORBA call as SALOMEDS::Driver required for this method
  return _corba_impl->CanCopy(aSO->GetCORBAImpl());
}
 
bool SALOMEDS_StudyManager::Copy(SALOMEDSClient_SObject* theSO)
{
  SALOMEDS_SObject* aSO = dynamic_cast<SALOMEDS_SObject*>(theSO);
  //SRN: Pure CORBA call as SALOMEDS::Driver required for this method
  return _corba_impl->Copy(aSO->GetCORBAImpl());
}
 
bool SALOMEDS_StudyManager::CanPaste(SALOMEDSClient_SObject* theSO)
{
  SALOMEDS_SObject* aSO = dynamic_cast<SALOMEDS_SObject*>(theSO);
  //SRN: Pure CORBA call as SALOMEDS::Driver required for this method
  return _corba_impl->CanPaste(aSO->GetCORBAImpl());
}
 
SALOMEDSClient_SObject* SALOMEDS_StudyManager::Paste(SALOMEDSClient_SObject* theSO)
{
  SALOMEDS_SObject* aSO = dynamic_cast<SALOMEDS_SObject*>(theSO);
  //SRN: Pure CORBA call as SALOMEDS::Driver required for this method
  SALOMEDS_SObject* aResultSO = new SALOMEDS_SObject(_corba_impl->Paste(aSO->GetCORBAImpl()));
  return aResultSO;
}

