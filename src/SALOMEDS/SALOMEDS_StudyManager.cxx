//  File   : SALOMEDSClient_StudyManager.cxx
//  Author : Sergey RUIN
//  Module : SALOME

using namespace std; 
#include "SALOMEDS_StudyManager.hxx"

#include "SALOMEDSImpl_Study.hxx"
#include "SALOMEDS_Study.hxx"
#include "SALOMEDS_SObject.hxx"
#include "SALOMEDS_Driver_i.hxx"

#include "Utils_ORB_INIT.hxx" 
#include "Utils_SINGLETON.hxx" 

#ifdef WIN32
#include <process.h>
#else
#include <sys/types.h>
#include <unistd.h>
#endif

#include <string>
#include <TCollection_AsciiString.hxx> 
#include <TColStd_HSequenceOfTransient.hxx>

#include "OpUtil.hxx"

SALOMEDS_Driver_i* GetDriver(const Handle(SALOMEDSImpl_SObject)& theObject, CORBA::ORB_ptr orb);

SALOMEDS_StudyManager::SALOMEDS_StudyManager(SALOMEDS::StudyManager_ptr theManager)
{

#ifdef WIN32
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

  init_orb();
}

SALOMEDS_StudyManager::~SALOMEDS_StudyManager()
{
  //if(!_isLocal) CORBA::release(_corba_impl);
}

SALOMEDSClient_Study* SALOMEDS_StudyManager::NewStudy(const std::string& study_name)
{
  SALOMEDS_Study* aStudy;

  if(_isLocal) {
    Handle(SALOMEDSImpl_Study) aStudy_impl = _local_impl->NewStudy((char*)study_name.c_str());
    if(aStudy_impl.IsNull()) return NULL;
    aStudy = new SALOMEDS_Study(aStudy_impl);
  }
  else {
    SALOMEDS::Study_var aStudy_impl = _corba_impl->NewStudy((char*)study_name.c_str());
    if(CORBA::is_nil(aStudy_impl)) return NULL; 
    aStudy = new SALOMEDS_Study(aStudy_impl);
  }

  return aStudy;
}

SALOMEDSClient_Study* SALOMEDS_StudyManager::Open(const std::string& theStudyUrl)
{
  SALOMEDS_Study* aStudy;

  if(_isLocal) {
    Handle(SALOMEDSImpl_Study) aStudy_impl = _local_impl->Open((char*)theStudyUrl.c_str());
    if(aStudy_impl.IsNull()) return NULL;
    aStudy = new SALOMEDS_Study(aStudy_impl);
  }
  else {
    SALOMEDS::Study_var aStudy_impl = _corba_impl->Open((char*)theStudyUrl.c_str());
    if(CORBA::is_nil(aStudy_impl)) return NULL; 
    
    aStudy = new SALOMEDS_Study(aStudy_impl.in());
  }

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
 
void SALOMEDS_StudyManager::SaveAs(const std::string& theUrl,  SALOMEDSClient_Study* theStudy, bool theMultiFile)
{
  //SRN: Pure CORBA save as the save operation require CORBA in any case 
  SALOMEDS::Study_var aStudy = _corba_impl->GetStudyByID(theStudy->StudyId());
  _corba_impl->SaveAs((char*)theUrl.c_str(), aStudy, theMultiFile);
}
 
void SALOMEDS_StudyManager::SaveAsASCII(const std::string& theUrl,  SALOMEDSClient_Study* theStudy, bool theMultiFile)
{
  //SRN: Pure CORBA save as the save operation require CORBA in any case 
  SALOMEDS::Study_var aStudy = _corba_impl->GetStudyByID(theStudy->StudyId());
  _corba_impl->SaveAsASCII((char*)theUrl.c_str(), aStudy, theMultiFile);
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
 
SALOMEDSClient_Study* SALOMEDS_StudyManager::GetStudyByName(const std::string& theStudyName) 
{
  SALOMEDS_Study* aStudy = NULL;
  if(_isLocal) {
    Handle(SALOMEDSImpl_Study) aStudy_impl = _local_impl->GetStudyByName((char*)theStudyName.c_str());
    if(aStudy_impl.IsNull()) return NULL;
    aStudy = new SALOMEDS_Study(aStudy_impl);
  }
  else  {
    SALOMEDS::Study_var aStudy_impl = _corba_impl->GetStudyByName((char*)theStudyName.c_str());
    if(CORBA::is_nil(aStudy_impl)) return NULL; 
    aStudy = new SALOMEDS_Study(aStudy_impl);
  }
  return aStudy;
}

SALOMEDSClient_Study* SALOMEDS_StudyManager::GetStudyByID(int theStudyID) 
{
  SALOMEDS_Study* aStudy = NULL;
  if(_isLocal) {
    Handle(SALOMEDSImpl_Study) aStudy_impl = _local_impl->GetStudyByID(theStudyID);
    if(aStudy_impl.IsNull()) return NULL;
    aStudy = new SALOMEDS_Study(aStudy_impl);
  }
  else { 
    SALOMEDS::Study_var aStudy_impl = _corba_impl->GetStudyByID(theStudyID);
    if(CORBA::is_nil(aStudy_impl)) return NULL; 
    aStudy = new SALOMEDS_Study(aStudy_impl);
  }
  return aStudy; 
}
 
bool SALOMEDS_StudyManager::CanCopy(SALOMEDSClient_SObject* theSO)
{
  SALOMEDS_SObject* aSO = dynamic_cast<SALOMEDS_SObject*>(theSO);
  bool ret;

  if(_isLocal) {
    Handle(SALOMEDSImpl_SObject) aSO_impl = aSO->GetLocalImpl();
    SALOMEDS_Driver_i* aDriver = GetDriver(aSO_impl, _orb);
    ret = _local_impl->CanCopy(aSO_impl, aDriver);
    delete aDriver;
  }
  else {
    ret = _corba_impl->CanCopy(aSO->GetCORBAImpl());
  }

  return ret;
}
 
bool SALOMEDS_StudyManager::Copy(SALOMEDSClient_SObject* theSO)
{
  SALOMEDS_SObject* aSO = dynamic_cast<SALOMEDS_SObject*>(theSO);
  bool ret;
  if(_isLocal) {
    Handle(SALOMEDSImpl_SObject) aSO_impl = aSO->GetLocalImpl();
    SALOMEDS_Driver_i* aDriver = GetDriver(aSO_impl, _orb);
    ret = _local_impl->Copy(aSO_impl, aDriver);
    delete aDriver;
  }
  else {
    ret = _corba_impl->Copy(aSO->GetCORBAImpl());
  }
  return ret;
}
 
bool SALOMEDS_StudyManager::CanPaste(SALOMEDSClient_SObject* theSO)
{
  SALOMEDS_SObject* aSO = dynamic_cast<SALOMEDS_SObject*>(theSO);
  bool ret;

  if(_isLocal) {
    Handle(SALOMEDSImpl_SObject) aSO_impl = aSO->GetLocalImpl();
    SALOMEDS_Driver_i* aDriver = GetDriver(aSO_impl, _orb);
    ret = _local_impl->CanPaste(aSO_impl, aDriver);
    delete aDriver;
  }
  else {
    ret = _corba_impl->CanPaste(aSO->GetCORBAImpl());
  }

  return ret;
}
 
SALOMEDSClient_SObject* SALOMEDS_StudyManager::Paste(SALOMEDSClient_SObject* theSO)
{
  SALOMEDS_SObject* aSO = dynamic_cast<SALOMEDS_SObject*>(theSO);
  SALOMEDSClient_SObject* aResult = NULL;

  if(_isLocal) {
    Handle(SALOMEDSImpl_SObject) aSO_impl = aSO->GetLocalImpl();
    SALOMEDS_Driver_i* aDriver = GetDriver(aSO_impl, _orb);
    Handle(SALOMEDSImpl_SObject) aNewSO = _local_impl->Paste(aSO_impl, aDriver);
    delete aDriver;
    if(aNewSO.IsNull()) return NULL;
    aResult = new SALOMEDS_SObject(aNewSO);
  }
  else {
    SALOMEDS::SObject_ptr aNewSO = _corba_impl->Paste(aSO->GetCORBAImpl());
    if(CORBA::is_nil(aNewSO)) return NULL;
    aResult = new SALOMEDS_SObject(aNewSO);
  }

  return aResult;
}


void SALOMEDS_StudyManager::init_orb()
{
  ORB_INIT &init = *SINGLETON_<ORB_INIT>::Instance() ;
  ASSERT(SINGLETON_<ORB_INIT>::IsAlreadyExisting()) ;
  _orb = init(0 , 0 ) ;     
}

SALOMEDS_Driver_i* GetDriver(const Handle(SALOMEDSImpl_SObject)& theObject, CORBA::ORB_ptr orb)
{
  SALOMEDS_Driver_i* driver = NULL;
  
  Handle(SALOMEDSImpl_SComponent) aSCO = theObject->GetFatherComponent();
  if(!aSCO.IsNull()) {
    TCollection_AsciiString IOREngine = aSCO->GetIOR();
    if(!IOREngine.IsEmpty()) {
      CORBA::Object_var obj = orb->string_to_object(IOREngine.ToCString());
      SALOMEDS::Driver_var Engine = SALOMEDS::Driver::_narrow(obj) ;
      driver = new SALOMEDS_Driver_i(Engine, orb);
    }
  }  

  return driver;
}
