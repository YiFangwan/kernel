//  File   : SALOMEDS_Study.cxx
//  Author : Sergey RUIN
//  Module : SALOME

using namespace std; 

#include "utilities.h" 

#include "SALOMEDS_Study.hxx"
#include "SALOMEDS_SComponent.hxx"
#include "SALOMEDSImpl_SComponent.hxx"
#include "SALOMEDS_SObject.hxx"
#include "SALOMEDSImpl_SObject.hxx"
#include "SALOMEDS_StudyBuilder.hxx"
#include "SALOMEDSImpl_StudyBuilder.hxx"
#include "SALOMEDS_ChildIterator.hxx"
#include "SALOMEDSImpl_ChildIterator.hxx"
#include "SALOMEDS_SComponentIterator.hxx"
#include "SALOMEDSImpl_SComponentIterator.hxx"
#include "SALOMEDS_AttributeStudyProperties.hxx"
#include "SALOMEDSImpl_AttributeStudyProperties.hxx"
#include "SALOMEDS_UseCaseBuilder.hxx"
#include "SALOMEDSImpl_UseCaseBuilder.hxx"

#include "SALOMEDS_Driver_i.hxx"

#include <TColStd_HSequenceOfAsciiString.hxx>
#include <TColStd_HSequenceOfTransient.hxx>

#include "Utils_ORB_INIT.hxx" 
#include "Utils_SINGLETON.hxx" 

#ifdef WNT
#include <process.h>
#else
#include <sys/types.h>
#include <unistd.h>
#endif

#include "OpUtil.hxx"

SALOMEDS_Study::SALOMEDS_Study(const Handle(SALOMEDSImpl_Study)& theStudy)
{
  _isLocal = true;
  _local_impl = theStudy;
  _corba_impl = SALOMEDS::Study::_nil();
  init_orb();
}

SALOMEDS_Study::SALOMEDS_Study(SALOMEDS::Study_ptr theStudy)
{
#ifdef WNT
  long pid =  (long)_getpid();
#else
  long pid =  (long)getpid();
#endif  

  long addr = theStudy->GetLocalImpl(GetHostname().c_str(), pid, _isLocal);
  if(_isLocal) {
    _local_impl = ((SALOMEDSImpl_Study*)(addr));
    _corba_impl = SALOMEDS::Study::_nil();
  }
  else {
    _local_impl = NULL;
    _corba_impl = SALOMEDS::Study::_duplicate(theStudy);
  }

  init_orb();
}

SALOMEDS_Study::~SALOMEDS_Study()
{
}

char* SALOMEDS_Study::GetPersistentReference()
{
  TCollection_AsciiString aRef;
  if(_isLocal) aRef = _local_impl->GetPersistentReference();
  else aRef = _corba_impl->GetPersistentReference();
  return aRef.ToCString();
}

char* SALOMEDS_Study::GetTransientReference()
{
  TCollection_AsciiString aRef;
  if(_isLocal) aRef = _local_impl->GetTransientReference();
  else aRef = _corba_impl->GetTransientReference();
  return aRef.ToCString();
}
 
bool SALOMEDS_Study::IsEmpty()
{
  bool ret;
  if(_isLocal) ret = _local_impl->IsEmpty();
  else ret = _corba_impl->IsEmpty();
  return ret;
}

SALOMEDSClient_SComponent* SALOMEDS_Study::FindComponent (const char* aComponentName)
{
  SALOMEDS_SComponent* aSCO = NULL;
  TCollection_AsciiString aName((char*)aComponentName);
  if(_isLocal) {
    Handle(SALOMEDSImpl_SComponent) aSCO_impl =_local_impl->FindComponent(aName);
    if(aSCO_impl.IsNull()) return NULL;
    aSCO = new SALOMEDS_SComponent(aSCO_impl);
  }
  else {
    SALOMEDS::SComponent_var aSCO_impl = _corba_impl->FindComponent(aName.ToCString());
    if(CORBA::is_nil(aSCO_impl)) return NULL;
    aSCO = new SALOMEDS_SComponent(aSCO_impl);
  }
  return aSCO;
}
 
SALOMEDSClient_SComponent* SALOMEDS_Study::FindComponentID(const char* aComponentID)
{  
  SALOMEDS_SComponent* aSCO = NULL;
  TCollection_AsciiString anID((char*)aComponentID);
  if(_isLocal) {
    Handle(SALOMEDSImpl_SComponent) aSCO_impl =_local_impl->FindComponentID(anID);
    if(aSCO_impl.IsNull()) return NULL;
    aSCO = new SALOMEDS_SComponent(aSCO_impl);
  }
  else {
    SALOMEDS::SComponent_var aSCO_impl = _corba_impl->FindComponentID(anID.ToCString());
    if(CORBA::is_nil(aSCO_impl)) return NULL;
    aSCO = new SALOMEDS_SComponent(aSCO_impl);
  }
  return aSCO;
  
}
 
SALOMEDSClient_SObject* SALOMEDS_Study::FindObject(const char* anObjectName)
{
  SALOMEDS_SObject* aSO = NULL;
  TCollection_AsciiString aName((char*)anObjectName);
  if(_isLocal) {
    Handle(SALOMEDSImpl_SObject) aSO_impl = _local_impl->FindObject(aName);
    if(aSO_impl.IsNull()) return NULL;
    aSO = new SALOMEDS_SObject(aSO_impl);
  }
  else { 
    SALOMEDS::SObject_var aSO_impl = _corba_impl->FindObject(aName.ToCString());
    if(CORBA::is_nil(aSO_impl)) return NULL;
    aSO = new SALOMEDS_SObject(aSO_impl);
  }
  return aSO;
}
 
std::vector<SALOMEDSClient_SObject*> SALOMEDS_Study::FindObjectByName(const char* anObjectName, 
								      const char* aComponentName)   
{
  std::vector<SALOMEDSClient_SObject*> aVector;
  int i, aLength = 0;
  TCollection_AsciiString anObjName((char*)anObjectName);
  TCollection_AsciiString aCompName((char*)aComponentName);
  
  if(_isLocal) {
    Handle(TColStd_HSequenceOfTransient) aSeq = _local_impl->FindObjectByName(anObjName, aCompName);
    aLength = aSeq->Length();
    for(i = 1; i<= aLength; i++) 
      aVector.push_back(new SALOMEDS_SObject(Handle(SALOMEDSImpl_SObject)::DownCast(aSeq->Value(i))));
  }
  else {
    SALOMEDS::Study::ListOfSObject_var aSeq = _corba_impl->FindObjectByName(anObjName.ToCString(), aCompName.ToCString());
    aLength = aSeq->length();
    for(i = 0; i< aLength; i++) aVector.push_back(new SALOMEDS_SObject(aSeq[i]));
  }

  return aVector;
}
 
SALOMEDSClient_SObject* SALOMEDS_Study::FindObjectID(const char* anObjectID)
{
  SALOMEDS_SObject* aSO = NULL;
  TCollection_AsciiString anID((char*)anObjectID);
  if(_isLocal) {
    Handle(SALOMEDSImpl_SObject) aSO_impl = _local_impl->FindObjectID(anID);
    if(aSO_impl.IsNull()) return NULL;
    aSO = new SALOMEDS_SObject(aSO_impl);
  }
  else { 
    SALOMEDS::SObject_var aSO_impl = _corba_impl->FindObjectID(anID.ToCString());
    if(CORBA::is_nil(aSO_impl)) return NULL;
    aSO = new SALOMEDS_SObject(aSO_impl);
  }
  return aSO;
}
 
SALOMEDSClient_SObject* SALOMEDS_Study::CreateObjectID(const char* anObjectID)
{
  SALOMEDS_SObject* aSO = NULL;
  TCollection_AsciiString anID((char*)anObjectID);
  if(_isLocal) aSO = new SALOMEDS_SObject(_local_impl->CreateObjectID(anID));
  else aSO = new SALOMEDS_SObject(_corba_impl->CreateObjectID(anID.ToCString())); 
  return aSO;
}
 
SALOMEDSClient_SObject* SALOMEDS_Study::FindObjectIOR(const char* anObjectIOR)
{
  SALOMEDS_SObject* aSO = NULL;
  TCollection_AsciiString anIOR((char*)anObjectIOR);
  if(_isLocal) {
    Handle(SALOMEDSImpl_SObject) aSO_impl = _local_impl->FindObjectIOR(anIOR);
    if(aSO_impl.IsNull()) return NULL;
    aSO = new SALOMEDS_SObject(aSO_impl);
  }
  else { 
    SALOMEDS::SObject_var aSO_impl = _corba_impl->FindObjectIOR(anIOR.ToCString());
    if(CORBA::is_nil(aSO_impl)) return NULL;
    aSO = new SALOMEDS_SObject(aSO_impl);
  }
  return aSO;
}

SALOMEDSClient_SObject* SALOMEDS_Study::FindObjectByPath(const char* thePath)
{
  SALOMEDS_SObject* aSO = NULL;
  TCollection_AsciiString aPath((char*)thePath);
  if(_isLocal) {
    Handle(SALOMEDSImpl_SObject) aSO_impl = _local_impl->FindObjectByPath(aPath);
    if(aSO_impl.IsNull()) return NULL;
    aSO = new SALOMEDS_SObject(aSO_impl);
  }
  else {
    SALOMEDS::SObject_var aSO_impl = _corba_impl->FindObjectByPath(aPath.ToCString());
    if(CORBA::is_nil(aSO_impl)) return NULL;
    aSO = new SALOMEDS_SObject(aSO_impl);
  }
  return aSO;
}

char* SALOMEDS_Study::GetObjectPath(SALOMEDSClient_SObject* theSO)
{
  SALOMEDS_SObject* aSO = dynamic_cast<SALOMEDS_SObject*>(theSO);
  TCollection_AsciiString aPath;
  if(_isLocal) aPath = _local_impl->GetObjectPath(aSO->GetLocalImpl());
  else aPath = _corba_impl->GetObjectPath(aSO->GetCORBAImpl());
  return aPath.ToCString();
}

void SALOMEDS_Study::SetContext(const char* thePath)
{
  TCollection_AsciiString aPath((char*)thePath);
  if(_isLocal) _local_impl->SetContext(aPath);
  else _corba_impl->SetContext(aPath.ToCString());
}

char* SALOMEDS_Study::GetContext()  
{
  TCollection_AsciiString aPath;
  if(_isLocal) aPath = _local_impl->GetContext();
  else aPath = _corba_impl->GetContext();
  return aPath.ToCString();
}

std::vector<std::string> SALOMEDS_Study::GetObjectNames(const char* theContext)
{
  std::vector<std::string> aVector;
  TCollection_AsciiString aContext((char*)theContext);
  int aLength, i;
  if(_isLocal) {
    Handle(TColStd_HSequenceOfAsciiString) aSeq = _local_impl->GetObjectNames(aContext);
    aLength = aSeq->Length();
    for(i = 1; i<=aLength; i++) aVector.push_back(aSeq->Value(i).ToCString());
  }
  else {
    SALOMEDS::ListOfStrings_var aSeq = _corba_impl->GetObjectNames(aContext.ToCString());
    aLength = aSeq->length();
    for(i = 0; i<aLength; i++) aVector.push_back(TCollection_AsciiString((char*)aSeq[i].in()).ToCString());
  }
  return aVector;
}
 
std::vector<std::string> SALOMEDS_Study::GetDirectoryNames(const char* theContext)
{
  std::vector<std::string> aVector;
  TCollection_AsciiString aContext((char*)theContext);
  int aLength, i;
  if(_isLocal) {
    Handle(TColStd_HSequenceOfAsciiString) aSeq = _local_impl->GetDirectoryNames(aContext);
    aLength = aSeq->Length();
    for(i = 1; i<=aLength; i++) aVector.push_back(aSeq->Value(i).ToCString());
  }
  else {
    SALOMEDS::ListOfStrings_var aSeq = _corba_impl->GetDirectoryNames(aContext.ToCString());
    aLength = aSeq->length();
    for(i = 0; i<aLength; i++) aVector.push_back(TCollection_AsciiString((char*)aSeq[i].in()).ToCString());
  }
  return aVector;
}
 
std::vector<std::string> SALOMEDS_Study::GetFileNames(const char* theContext)
{
  std::vector<std::string> aVector;
  TCollection_AsciiString aContext((char*)theContext);
  int aLength, i;
  if(_isLocal) {
    Handle(TColStd_HSequenceOfAsciiString) aSeq = _local_impl->GetFileNames(aContext);
    aLength = aSeq->Length();
    for(i = 1; i<=aLength; i++) aVector.push_back(aSeq->Value(i).ToCString());
  }
  else {
    SALOMEDS::ListOfStrings_var aSeq = _corba_impl->GetFileNames(aContext.ToCString());
    aLength = aSeq->length();

    for(i = 0; i<aLength; i++) aVector.push_back(TCollection_AsciiString((char*)aSeq[i].in()).ToCString());
  }
  return aVector;
}
 
std::vector<std::string> SALOMEDS_Study::GetComponentNames(const char* theContext)
{
  std::vector<std::string> aVector;
  TCollection_AsciiString aContext((char*)theContext);
  int aLength, i;
  if(_isLocal) {
    Handle(TColStd_HSequenceOfAsciiString) aSeq = _local_impl->GetComponentNames(aContext);
    aLength = aSeq->Length();
    for(i = 1; i<=aLength; i++) aVector.push_back(aSeq->Value(i).ToCString());
  }
  else {
    SALOMEDS::ListOfStrings_var aSeq = _corba_impl->GetComponentNames(aContext.ToCString());
    aLength = aSeq->length();
    for(i = 0; i<aLength; i++) aVector.push_back(TCollection_AsciiString((char*)aSeq[i].in()).ToCString());
  }
  return aVector;
}

SALOMEDSClient_ChildIterator* SALOMEDS_Study::NewChildIterator(SALOMEDSClient_SObject* theSO)
{
  SALOMEDS_SObject* aSO = dynamic_cast<SALOMEDS_SObject*>(theSO);
  SALOMEDS_ChildIterator* aCI = NULL; 
  if(_isLocal) {
    Handle(SALOMEDSImpl_ChildIterator) aCIimpl = _local_impl->NewChildIterator(aSO->GetLocalImpl());
    aCI = new SALOMEDS_ChildIterator(aCIimpl);
  }
  else {
    SALOMEDS::ChildIterator_var aCIimpl = _corba_impl->NewChildIterator(aSO->GetCORBAImpl());
    aCI = new SALOMEDS_ChildIterator(aCIimpl);
  }

  return aCI;
}
 
SALOMEDSClient_SComponentIterator* SALOMEDS_Study::NewComponentIterator()
{
  SALOMEDS_SComponentIterator* aCI = NULL; 
  if(_isLocal) {
    SALOMEDSImpl_SComponentIterator aCIimpl = _local_impl->NewComponentIterator();
    aCI = new SALOMEDS_SComponentIterator(aCIimpl);
  }
  else {
    SALOMEDS::SComponentIterator_var aCIimpl = _corba_impl->NewComponentIterator();
    aCI = new SALOMEDS_SComponentIterator(aCIimpl);
  }

  return aCI;
}
 
SALOMEDSClient_StudyBuilder* SALOMEDS_Study::NewBuilder()
{
  SALOMEDS_StudyBuilder* aSB = NULL; 
  if(_isLocal) {
    Handle(SALOMEDSImpl_StudyBuilder) aSBimpl = _local_impl->NewBuilder();
    aSB = new SALOMEDS_StudyBuilder(aSBimpl);
  }
  else {
    SALOMEDS::StudyBuilder_var aSBimpl = _corba_impl->NewBuilder();
    aSB = new SALOMEDS_StudyBuilder(aSBimpl);
  }

  return aSB;
}

char* SALOMEDS_Study::Name()
{
  TCollection_AsciiString aName;
  if(_isLocal) aName = _local_impl->Name();
  else aName = _corba_impl->Name();
  return aName.ToCString();
}
 
void SALOMEDS_Study::Name(const char* name)
{
  TCollection_AsciiString aName((char*)name);
  if(_isLocal) _local_impl->Name(aName);
  else _corba_impl->Name(aName.ToCString());
}

bool SALOMEDS_Study::IsSaved()
{
  bool isSaved;
  if(_isLocal) isSaved = _local_impl->IsSaved();
  else isSaved = _corba_impl->IsSaved();
  return isSaved;
}

void SALOMEDS_Study::IsSaved(bool save)
{
  if(_isLocal) _local_impl->IsSaved(save);
  else _corba_impl->IsSaved(save);
}

bool SALOMEDS_Study::IsModified()
{
  bool isModified;
  if(_isLocal) isModified = _local_impl->IsModified();
  else isModified = _corba_impl->IsModified();
  return isModified;
}
 
char* SALOMEDS_Study::URL()
{
  TCollection_AsciiString aURL;
  if(_isLocal) aURL = _local_impl->URL();
  else aURL = _corba_impl->URL();
  return aURL.ToCString();
}

void SALOMEDS_Study::URL(const char* url)
{
  TCollection_AsciiString aURL((char*)url);
  if(_isLocal) _local_impl->URL(aURL);
  else _corba_impl->URL(aURL.ToCString());
}

int SALOMEDS_Study::StudyId()
{
  int anID;
  if(_isLocal) anID = _local_impl->StudyId();
  else anID = _corba_impl->StudyId();
  return anID;
}
 
void SALOMEDS_Study::StudyId(int id) 
{
  if(_isLocal) _local_impl->StudyId(id);
  else _corba_impl->StudyId(id);  
}

std::vector<SALOMEDSClient_SObject*> SALOMEDS_Study::FindDependances(SALOMEDSClient_SObject* theSO)
{
  std::vector<SALOMEDSClient_SObject*> aVector;
  SALOMEDS_SObject* aSO = dynamic_cast<SALOMEDS_SObject*>(theSO);
  int aLength, i;
  if(_isLocal) {
    Handle(TColStd_HSequenceOfTransient) aSeq = _local_impl->FindDependances(aSO->GetLocalImpl());
    aLength = aSeq->Length();
    for(i=1; i<=aLength; i++) 
      aVector.push_back(new SALOMEDS_SObject(Handle(SALOMEDSImpl_SObject)::DownCast(aSeq->Value(i))));
  }
  else {
    SALOMEDS::Study::ListOfSObject_var aSeq = _corba_impl->FindDependances(aSO->GetCORBAImpl());
    aLength = aSeq->length();
    for(i=0; i<aLength; i++) aVector.push_back(new SALOMEDS_SObject(aSeq[i]));
  }
  return aVector;
}
 
SALOMEDSClient_AttributeStudyProperties* SALOMEDS_Study::GetProperties()
{
  SALOMEDS_AttributeStudyProperties* aProp;
  if(_isLocal) aProp = new SALOMEDS_AttributeStudyProperties(_local_impl->GetProperties());
  else aProp = new SALOMEDS_AttributeStudyProperties(_corba_impl->GetProperties());
  return aProp;
}
 
char* SALOMEDS_Study::GetLastModificationDate() 
{
  TCollection_AsciiString aDate;
  if(_isLocal) aDate = _local_impl->GetLastModificationDate();
  else aDate = _corba_impl->GetLastModificationDate();
  return aDate.ToCString();
}

std::vector<std::string> SALOMEDS_Study::GetModificationsDate()
{
  std::vector<std::string> aVector;
  int aLength, i;
  if(_isLocal) {
    Handle(TColStd_HSequenceOfAsciiString) aSeq = _local_impl->GetModificationsDate();
    aLength = aSeq->Length();
    for(i=1; i<=aLength; i++) aVector.push_back(aSeq->Value(i).ToCString());
  }
  else {
    SALOMEDS::ListOfDates_var aSeq = _corba_impl->GetModificationsDate();
    aLength = aSeq->length();
    for(i=0; i<aLength; i++) aVector.push_back(TCollection_AsciiString((char*)aSeq[i].in()).ToCString());
  }
  return aVector;
}

SALOMEDSClient_UseCaseBuilder* SALOMEDS_Study::GetUseCaseBuilder()
{
  SALOMEDS_UseCaseBuilder* aUB = NULL;
  if(_isLocal) {
    Handle(SALOMEDSImpl_UseCaseBuilder) aUBimpl = _local_impl->GetUseCaseBuilder();
    aUB = new SALOMEDS_UseCaseBuilder(aUBimpl);
  }
  else {
    SALOMEDS::UseCaseBuilder_var aUBimpl = _corba_impl->GetUseCaseBuilder();
    aUB = new SALOMEDS_UseCaseBuilder(aUBimpl);
  }

  return aUB;
}

void SALOMEDS_Study::Close()
{
  if(_isLocal) _local_impl->Close();
  else _corba_impl->Close();
}

void SALOMEDS_Study::EnableUseCaseAutoFilling(bool isEnabled)
{
  if(_isLocal) _local_impl->EnableUseCaseAutoFilling(isEnabled);
  else _corba_impl->EnableUseCaseAutoFilling(isEnabled);
}

bool SALOMEDS_Study::DumpStudy(const char* thePath, const char* theBaseName, bool isPublished)
{
  TCollection_AsciiString aPath((char*)thePath);
  TCollection_AsciiString aBaseName((char*)theBaseName);
  bool ret;
  if(_isLocal) {
    SALOMEDS_DriverFactory_i* aFactory = new SALOMEDS_DriverFactory_i(_orb);
    ret = _local_impl->DumpStudy(aPath, aBaseName, isPublished, aFactory);
    delete aFactory;
  }
  else ret = _corba_impl->DumpStudy(aPath.ToCString(), aBaseName.ToCString(), isPublished);
  return ret;
}     

char* SALOMEDS_Study::ConvertObjectToIOR(CORBA::Object_ptr theObject) 
{
  return _orb->object_to_string(theObject); 
}

CORBA::Object_ptr SALOMEDS_Study::ConvertIORToObject(const char* theIOR) 
{ 
  return _orb->string_to_object(theIOR); 
} 

void SALOMEDS_Study::init_orb()
{
  ORB_INIT &init = *SINGLETON_<ORB_INIT>::Instance() ;
  ASSERT(SINGLETON_<ORB_INIT>::IsAlreadyExisting()) ;
  _orb = init(0 , 0 ) ;     
}
