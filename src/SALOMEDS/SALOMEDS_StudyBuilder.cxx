//  File   : SALOMEDS_StudyBuilder.cxx
//  Author : Sergey RUIN
//  Module : SALOME

using namespace std; 

#include "utilities.h"

#include "SALOMEDS_StudyBuilder.hxx"
#include "SALOMEDS_SObject.hxx"
#include "SALOMEDSImpl_SObject.hxx"
#include "SALOMEDS_SComponent.hxx"
#include "SALOMEDSImpl_SComponent.hxx"
#include "SALOMEDS_Driver_i.hxx"
#include "SALOMEDS_GenericAttribute.hxx"
#include "SALOMEDSImpl_GenericAttribute.hxx"
#include <TCollection_AsciiString.hxx>
#include <TDF_Attribute.hxx>
#include "SALOMEDS_StudyManager.hxx"

#include "Utils_CorbaException.hxx"
#include "Utils_ORB_INIT.hxx" 
#include "Utils_SINGLETON.hxx" 

SALOMEDS_StudyBuilder::SALOMEDS_StudyBuilder(const Handle(SALOMEDSImpl_StudyBuilder)& theBuilder)
{
  _isLocal = true;
  _local_impl = theBuilder;
  _corba_impl = SALOMEDS::StudyBuilder::_nil();
  init_orb();
}

SALOMEDS_StudyBuilder::SALOMEDS_StudyBuilder(SALOMEDS::StudyBuilder_ptr theBuilder)
{
  _isLocal = false;
  _local_impl = NULL;
  _corba_impl = theBuilder;
  init_orb();
}

SALOMEDS_StudyBuilder::~SALOMEDS_StudyBuilder() 
{
}

SALOMEDSClient_SComponent* SALOMEDS_StudyBuilder::NewComponent(const char* ComponentDataType)
{
  CheckLocked();

  SALOMEDS_SComponent* aSCO = NULL;
  TCollection_AsciiString aType((char*)ComponentDataType);
  
  if(_isLocal) {
    Handle(SALOMEDSImpl_SComponent) aSCO_impl =_local_impl->NewComponent(aType);
    if(aSCO_impl.IsNull()) return NULL;
    aSCO = new SALOMEDS_SComponent(aSCO_impl);
  }
  else {
    SALOMEDS::SComponent_var aSCO_impl = _corba_impl->NewComponent(aType.ToCString());
    if(CORBA::is_nil(aSCO_impl)) return NULL;
    aSCO = new SALOMEDS_SComponent(aSCO_impl);
  }
 
  return aSCO;
}

void SALOMEDS_StudyBuilder::DefineComponentInstance (SALOMEDSClient_SComponent* theSCO, 
						     const char* ComponentIOR)
{
  CheckLocked();

  SALOMEDS_SComponent* aSCO = dynamic_cast<SALOMEDS_SComponent*>(theSCO);
  TCollection_AsciiString anIOR((char*)ComponentIOR);
  if(_isLocal) _local_impl->DefineComponentInstance(Handle(SALOMEDSImpl_SComponent)::DownCast(aSCO->GetLocalImpl()),anIOR);
  else {
    CORBA::Object_var obj = _orb->string_to_object(ComponentIOR);
    _corba_impl->DefineComponentInstance(SALOMEDS::SComponent::_narrow(aSCO->GetCORBAImpl()), obj);
  }
}

void SALOMEDS_StudyBuilder::RemoveComponent(SALOMEDSClient_SComponent* theSCO)
{
  CheckLocked();

  SALOMEDS_SComponent* aSCO = dynamic_cast<SALOMEDS_SComponent*>(theSCO);
  if(_isLocal) _local_impl->RemoveComponent(Handle(SALOMEDSImpl_SComponent)::DownCast(aSCO->GetLocalImpl()));
  else _corba_impl->RemoveComponent(SALOMEDS::SComponent::_narrow(aSCO->GetCORBAImpl()));
}

SALOMEDSClient_SObject* SALOMEDS_StudyBuilder::NewObject(SALOMEDSClient_SObject* theFatherObject)
{
  CheckLocked();

  SALOMEDS_SObject* aSO = NULL;
  SALOMEDS_SObject* father = dynamic_cast< SALOMEDS_SObject*>(theFatherObject);
  if(father == NULL) return aSO;
  if(_isLocal) {
    Handle(SALOMEDSImpl_SObject) aSO_impl = _local_impl->NewObject(father->GetLocalImpl());
    if(aSO_impl.IsNull()) return NULL;
    aSO = new SALOMEDS_SObject(aSO_impl);
  }
  else {
    SALOMEDS::SObject_var aSO_impl = _corba_impl->NewObject(father->GetCORBAImpl());
    if(CORBA::is_nil(aSO_impl)) return NULL;
    aSO = new SALOMEDS_SObject(aSO_impl);
  }

  return aSO;
}

SALOMEDSClient_SObject* SALOMEDS_StudyBuilder::NewObjectToTag(SALOMEDSClient_SObject* theFatherObject, int theTag)
{  
  CheckLocked();

  SALOMEDS_SObject* aSO = NULL;
  SALOMEDS_SObject* father = dynamic_cast< SALOMEDS_SObject*>(theFatherObject);
  if(father == NULL) return aSO;
  if(_isLocal) {
    Handle(SALOMEDSImpl_SObject) aSO_impl = _local_impl->NewObjectToTag(father->GetLocalImpl(), theTag);
    if(aSO_impl.IsNull()) return NULL;
    aSO = new SALOMEDS_SObject(aSO_impl);
  }
  else {
    SALOMEDS::SObject_var aSO_impl = _corba_impl->NewObjectToTag(father->GetCORBAImpl(), theTag);
    if(CORBA::is_nil(aSO_impl)) return NULL;
    aSO = new SALOMEDS_SObject(aSO_impl);
  }

  return aSO;
  
}

void SALOMEDS_StudyBuilder::AddDirectory(const char* thePath)
{
  CheckLocked();

  TCollection_AsciiString aPath((char*)thePath);
  if(_isLocal) {
    _local_impl->AddDirectory(aPath);
    if(!_local_impl->AddDirectory(TCollection_AsciiString((char*)thePath))) {
      TCollection_AsciiString anErrorCode = _local_impl->GetErrorCode();
      if(anErrorCode == "StudyNameAlreadyUsed") throw SALOMEDS::Study::StudyNameAlreadyUsed(); 
      if(anErrorCode == "StudyInvalidDirectory") throw SALOMEDS::Study::StudyInvalidDirectory(); 
      if(anErrorCode == "StudyInvalidComponent") throw SALOMEDS::Study::StudyInvalidComponent();  
    }
  }
  else _corba_impl->AddDirectory(aPath.ToCString());
}

void SALOMEDS_StudyBuilder::LoadWith(SALOMEDSClient_SComponent* theSCO, const char* theIOR)
{
  SALOMEDS_SComponent* aSCO = dynamic_cast<SALOMEDS_SComponent*>(theSCO);
  TCollection_AsciiString anIOR((char*)theIOR);
  CORBA::Object_var obj = _orb->string_to_object(theIOR);
  SALOMEDS::Driver_var aDriver = SALOMEDS::Driver::_narrow(obj);
  
  if(_isLocal) {
    SALOMEDS_Driver_i* drv = new SALOMEDS_Driver_i(aDriver, _orb);
    bool isDone = _local_impl->LoadWith(Handle(SALOMEDSImpl_SComponent)::DownCast(aSCO->GetLocalImpl()), drv);
    delete drv;
    if(!isDone && _local_impl->IsError()) 
      THROW_SALOME_CORBA_EXCEPTION(_local_impl->GetErrorCode().ToCString(),SALOME::BAD_PARAM);
  }
  else {
    _corba_impl->LoadWith(SALOMEDS::SComponent::_narrow(aSCO->GetCORBAImpl()), aDriver);
  }
}

void SALOMEDS_StudyBuilder::Load(SALOMEDSClient_SObject* theSCO)
{
  SALOMEDS_SComponent* aSCO = dynamic_cast<SALOMEDS_SComponent*>(theSCO);
  if(_isLocal) _local_impl->Load(Handle(SALOMEDSImpl_SComponent)::DownCast(aSCO->GetLocalImpl()));
  else _corba_impl->Load(SALOMEDS::SComponent::_narrow(aSCO->GetCORBAImpl()));
}

void SALOMEDS_StudyBuilder::RemoveObject(SALOMEDSClient_SObject* theSO)
{
  CheckLocked();

  SALOMEDS_SObject* aSO = dynamic_cast<SALOMEDS_SObject*>(theSO);
  if(_isLocal) _local_impl->RemoveObject(aSO->GetLocalImpl());
  else _corba_impl->RemoveObject(aSO->GetCORBAImpl());
}

void SALOMEDS_StudyBuilder::RemoveObjectWithChildren(SALOMEDSClient_SObject* theSO)
{
  CheckLocked();

  SALOMEDS_SObject* aSO = dynamic_cast<SALOMEDS_SObject*>(theSO);
  if(_isLocal) _local_impl->RemoveObjectWithChildren(aSO->GetLocalImpl());
  else _corba_impl->RemoveObjectWithChildren(aSO->GetCORBAImpl());
}
 
SALOMEDSClient_GenericAttribute* SALOMEDS_StudyBuilder::FindOrCreateAttribute(SALOMEDSClient_SObject* theSO, 
									      const char* aTypeOfAttribute)
{
  SALOMEDS_SObject* aSO = dynamic_cast<SALOMEDS_SObject*>(theSO);
  SALOMEDS_GenericAttribute* anAttr = NULL;
  TCollection_AsciiString aType((char*)aTypeOfAttribute);
  if(_isLocal) {
    Handle(SALOMEDSImpl_GenericAttribute) aGA;
    try {
      aGA=Handle(SALOMEDSImpl_GenericAttribute)::DownCast(_local_impl->FindOrCreateAttribute(aSO->GetLocalImpl(),aType));
     }
    catch (...) {
      throw SALOMEDS::StudyBuilder::LockProtection();
    }  
    anAttr = SALOMEDS_GenericAttribute::CreateAttribute(aGA);
  }
  else {
    SALOMEDS::GenericAttribute_var aGA = _corba_impl->FindOrCreateAttribute(aSO->GetCORBAImpl(), aType.ToCString());
    anAttr = SALOMEDS_GenericAttribute::CreateAttribute(aGA);
  }

  return anAttr;
}

bool SALOMEDS_StudyBuilder::FindAttribute(SALOMEDSClient_SObject* theSO, 
					  SALOMEDSClient_GenericAttribute* anAttribute, 
					  const char* aTypeOfAttribute)
{
  bool ret;
  SALOMEDS_SObject* aSO = dynamic_cast<SALOMEDS_SObject*>(theSO);
  TCollection_AsciiString aType((char*)aTypeOfAttribute);
  anAttribute = NULL;
  if(_isLocal) {
    Handle(SALOMEDSImpl_GenericAttribute) aGA;
    ret = _local_impl->FindAttribute(aSO->GetLocalImpl(), aGA, aType);
    if(ret) anAttribute = SALOMEDS_GenericAttribute::CreateAttribute(aGA);
  }
  else {
    SALOMEDS::GenericAttribute_var aGA;
    ret = _corba_impl->FindAttribute(aSO->GetCORBAImpl(), aGA.out(), aType.ToCString()); 
    if(ret) anAttribute = SALOMEDS_GenericAttribute::CreateAttribute(aGA);
  }

  return ret;
}
 
void SALOMEDS_StudyBuilder::RemoveAttribute(SALOMEDSClient_SObject* theSO, const char* aTypeOfAttribute)
{
  CheckLocked();

  SALOMEDS_SObject* aSO = dynamic_cast<SALOMEDS_SObject*>(theSO);
  TCollection_AsciiString aType((char*)aTypeOfAttribute);
  if(_isLocal) _local_impl->RemoveAttribute(aSO->GetLocalImpl(), aType);
  else _corba_impl->RemoveAttribute(aSO->GetCORBAImpl(), aType.ToCString()); 
}

void SALOMEDS_StudyBuilder::Addreference(SALOMEDSClient_SObject* me, SALOMEDSClient_SObject* thereferencedObject)
{
  CheckLocked();

  SALOMEDS_SObject* aSO = dynamic_cast<SALOMEDS_SObject*>(me);
  SALOMEDS_SObject* aRefSO = dynamic_cast<SALOMEDS_SObject*>(thereferencedObject);
  if(_isLocal) _local_impl->Addreference(aSO->GetLocalImpl(), aRefSO->GetLocalImpl());
  else _corba_impl->Addreference(aSO->GetCORBAImpl(), aRefSO->GetCORBAImpl());
}

void SALOMEDS_StudyBuilder::RemoveReference(SALOMEDSClient_SObject* me)
{
  CheckLocked();

  SALOMEDS_SObject* aSO = dynamic_cast<SALOMEDS_SObject*>(me);
  if(_isLocal) _local_impl->RemoveReference(aSO->GetLocalImpl());
  else _corba_impl->RemoveReference(aSO->GetCORBAImpl());
}

void SALOMEDS_StudyBuilder::SetGUID(SALOMEDSClient_SObject* theSO, const char* theGUID)
{
  CheckLocked();

  SALOMEDS_SObject* aSO = dynamic_cast<SALOMEDS_SObject*>(theSO);
  TCollection_AsciiString aGUID((char*)theGUID);
  if(_isLocal) _local_impl->SetGUID(aSO->GetLocalImpl(), aGUID);
  else _corba_impl->SetGUID(aSO->GetCORBAImpl(), aGUID.ToCString());
}
 
bool SALOMEDS_StudyBuilder::IsGUID(SALOMEDSClient_SObject* theSO, const char* theGUID)
{
  SALOMEDS_SObject* aSO = dynamic_cast<SALOMEDS_SObject*>(theSO);
  TCollection_AsciiString aGUID((char*)theGUID);
  bool ret;
  if(_isLocal) ret = _local_impl->IsGUID(aSO->GetLocalImpl(), aGUID);
  else ret = _corba_impl->IsGUID(aSO->GetCORBAImpl(), aGUID.ToCString());

  return ret;
}

void SALOMEDS_StudyBuilder::NewCommand()
{
  if(_isLocal) _local_impl->NewCommand();
  else _corba_impl->NewCommand();
}
 
void SALOMEDS_StudyBuilder::CommitCommand()
{
  if(_isLocal) {
    try {
      _local_impl->CommitCommand();
    }
    catch(...) {
      throw SALOMEDS::StudyBuilder::LockProtection();
    }
  }
  else _corba_impl->CommitCommand();
}

bool SALOMEDS_StudyBuilder::HasOpenCommand()
{
  bool ret;
  if(_isLocal) ret = _local_impl->HasOpenCommand();
  else ret = _corba_impl->HasOpenCommand();
  return ret;
}

void SALOMEDS_StudyBuilder::AbortCommand()
{
  if(_isLocal) _local_impl->AbortCommand();
  else _corba_impl->AbortCommand();
}

void SALOMEDS_StudyBuilder::Undo()
{
  if(_isLocal) {
    try {
      _local_impl->Undo();
    }
    catch(...) {
      throw SALOMEDS::StudyBuilder::LockProtection();
    }
  }
  else _corba_impl->Undo();
}
 
void SALOMEDS_StudyBuilder::Redo()
{
  if(_isLocal) {
    try {
      _local_impl->Redo();
    }
    catch(...) {
      throw SALOMEDS::StudyBuilder::LockProtection();
    }
  }
  else _corba_impl->Redo(); 
}
 
bool SALOMEDS_StudyBuilder::GetAvailableUndos()
{
  bool ret;
  if(_isLocal) ret = _local_impl->GetAvailableUndos();
  else ret = _corba_impl->GetAvailableUndos();
  return ret;
}

bool SALOMEDS_StudyBuilder::GetAvailableRedos()
{
  bool ret;
  if(_isLocal) ret = _local_impl->GetAvailableRedos();
  else ret = _corba_impl->GetAvailableRedos();
  return ret; 
}

int SALOMEDS_StudyBuilder::UndoLimit()
{
  int aLimit;
  if(_isLocal) aLimit = _local_impl->UndoLimit();
  else aLimit = _corba_impl->UndoLimit();
  return aLimit;
}
 
void SALOMEDS_StudyBuilder::UndoLimit(int theLimit)
{
  CheckLocked();

  if(_isLocal) _local_impl->UndoLimit(theLimit);
  else _corba_impl->UndoLimit(theLimit);
}
 
void SALOMEDS_StudyBuilder::CheckLocked()
{
  //There is only local part as CORBA part throws the correct exeception
  if(_isLocal) {
    try {
      _local_impl->CheckLocked();
    }
    catch(...) {
      throw SALOMEDS::StudyBuilder::LockProtection();
    }
  }
}

void SALOMEDS_StudyBuilder::SetName(SALOMEDSClient_SObject* theSO, const char* theValue)
{
  CheckLocked();

  SALOMEDS_SObject* aSO = dynamic_cast<SALOMEDS_SObject*>(theSO);
  TCollection_AsciiString aValue((char*)theValue);
  if(_isLocal) _local_impl->SetName(aSO->GetLocalImpl(), aValue);
  else _corba_impl->SetName(aSO->GetCORBAImpl(), aValue.ToCString());
}

void SALOMEDS_StudyBuilder::SetComment(SALOMEDSClient_SObject* theSO, const char* theValue)
{
  CheckLocked();

  SALOMEDS_SObject* aSO = dynamic_cast<SALOMEDS_SObject*>(theSO);
  TCollection_AsciiString aValue((char*)theValue);
  if(_isLocal) _local_impl->SetComment(aSO->GetLocalImpl(), aValue);
  else _corba_impl->SetComment(aSO->GetCORBAImpl(), aValue.ToCString());
}

void SALOMEDS_StudyBuilder::SetIOR(SALOMEDSClient_SObject* theSO, const char* theValue)
{
  CheckLocked();

  SALOMEDS_SObject* aSO = dynamic_cast<SALOMEDS_SObject*>(theSO);
  TCollection_AsciiString aValue((char*)theValue);
  if(_isLocal) _local_impl->SetIOR(aSO->GetLocalImpl(), aValue);
  else _corba_impl->SetIOR(aSO->GetCORBAImpl(), aValue.ToCString());
}

void SALOMEDS_StudyBuilder::init_orb()
{
  ORB_INIT &init = *SINGLETON_<ORB_INIT>::Instance() ;
  ASSERT(SINGLETON_<ORB_INIT>::IsAlreadyExisting()) ;
  _orb = init(0 , 0 ) ;     
}
