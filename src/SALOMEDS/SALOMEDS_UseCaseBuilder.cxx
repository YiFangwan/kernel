//  File   : SALOMEDS_UseCaseBuilder.cxx
//  Author : Sergey RUIN
//  Module : SALOME

using namespace std; 

#include "SALOMEDS_UseCaseBuilder.hxx"

#include "SALOMEDSImpl_SObject.hxx"
#include "SALOMEDS_SObject.hxx"
#include "SALOMEDSImpl_UseCaseIterator.hxx"
#include "SALOMEDS_UseCaseIterator.hxx"
#include <TCollection_AsciiString.hxx>

SALOMEDS_UseCaseBuilder::SALOMEDS_UseCaseBuilder(const Handle(SALOMEDSImpl_UseCaseBuilder)& theBuilder)
{
  _isLocal = true;
  _local_impl = theBuilder;
  _corba_impl = SALOMEDS::UseCaseBuilder::_nil();
}

SALOMEDS_UseCaseBuilder::SALOMEDS_UseCaseBuilder(SALOMEDS::UseCaseBuilder_ptr theBuilder)
{
  _isLocal = false;
  _local_impl = NULL;
  _corba_impl = SALOMEDS::UseCaseBuilder::_duplicate(theBuilder);
}

SALOMEDS_UseCaseBuilder::~SALOMEDS_UseCaseBuilder()
{
}  

bool SALOMEDS_UseCaseBuilder::Append(SALOMEDSClient_SObject* theObject)
{
  bool ret;
  SALOMEDS_SObject* obj = dynamic_cast<SALOMEDS_SObject*>(theObject);
  if(_isLocal) ret = _local_impl->Append(obj->GetLocalImpl());
  else ret = _corba_impl->Append(obj->GetCORBAImpl());
  return ret;
}

bool SALOMEDS_UseCaseBuilder::Remove(SALOMEDSClient_SObject* theObject)
{
  bool ret;
  SALOMEDS_SObject* obj = dynamic_cast<SALOMEDS_SObject*>(theObject);
  if(_isLocal) ret = _local_impl->Remove(obj->GetLocalImpl());
  else ret = _corba_impl->Remove(obj->GetCORBAImpl());
  return ret;
}

bool SALOMEDS_UseCaseBuilder::AppendTo(SALOMEDSClient_SObject* theFather, SALOMEDSClient_SObject* theObject)
{
  bool ret;
  SALOMEDS_SObject* father = dynamic_cast<SALOMEDS_SObject*>(theFather);
  SALOMEDS_SObject* obj = dynamic_cast<SALOMEDS_SObject*>(theObject);
  if(_isLocal) ret = _local_impl->AppendTo(father->GetLocalImpl(), obj->GetLocalImpl());
  else ret = _corba_impl->AppendTo(father->GetCORBAImpl(), obj->GetCORBAImpl());
  return ret;
}

bool SALOMEDS_UseCaseBuilder::InsertBefore(SALOMEDSClient_SObject* theFirst, SALOMEDSClient_SObject* theNext)
{
  bool ret;
  SALOMEDS_SObject* first = dynamic_cast<SALOMEDS_SObject*>(theFirst);
  SALOMEDS_SObject* next = dynamic_cast<SALOMEDS_SObject*>(theNext);
  if(_isLocal) ret = _local_impl->InsertBefore(first->GetLocalImpl(), next->GetLocalImpl());
  else ret = _corba_impl->InsertBefore(first->GetCORBAImpl(), next->GetCORBAImpl());
  return ret;
}

bool SALOMEDS_UseCaseBuilder::SetCurrentObject(SALOMEDSClient_SObject* theObject)
{
  bool ret;
  SALOMEDS_SObject* obj = dynamic_cast<SALOMEDS_SObject*>(theObject);
  if(_isLocal) ret = _local_impl->SetCurrentObject(obj->GetLocalImpl());
  else ret = _corba_impl->SetCurrentObject(obj->GetCORBAImpl());
  return ret;
}

bool SALOMEDS_UseCaseBuilder::SetRootCurrent()
{
  bool ret;
  if(_isLocal) ret = _local_impl->SetRootCurrent();
  else ret = _corba_impl->SetRootCurrent();
  return ret;
}

bool SALOMEDS_UseCaseBuilder::HasChildren(SALOMEDSClient_SObject* theObject)
{
  bool ret;
  SALOMEDS_SObject* obj = dynamic_cast<SALOMEDS_SObject*>(theObject);
  if(_isLocal) ret = _local_impl->HasChildren(obj->GetLocalImpl());
  else ret = _corba_impl->HasChildren(obj->GetCORBAImpl());
  return ret;
}

bool SALOMEDS_UseCaseBuilder::IsUseCase(SALOMEDSClient_SObject* theObject)
{
  bool ret;
  SALOMEDS_SObject* obj = dynamic_cast<SALOMEDS_SObject*>(theObject);
  if(_isLocal) ret = _local_impl->IsUseCase(obj->GetLocalImpl());
  else ret = _corba_impl->IsUseCase(obj->GetCORBAImpl());
  return ret;
}

bool SALOMEDS_UseCaseBuilder::SetName(const char* theName)
{
  bool ret;
  TCollection_AsciiString aName((char*)theName);
  if(_isLocal) ret = _local_impl->SetName(aName);
  else ret = _corba_impl->SetName(aName.ToCString());
  return ret;
}

SALOMEDSClient_SObject* SALOMEDS_UseCaseBuilder::GetCurrentObject()
{
  SALOMEDS_SObject* obj = NULL;
  if(_isLocal) obj = new SALOMEDS_SObject(_local_impl->GetCurrentObject());
  else obj = new SALOMEDS_SObject(_corba_impl->GetCurrentObject());
  return obj;
}

char* SALOMEDS_UseCaseBuilder::GetName()
{
  TCollection_AsciiString aName;
  if(_isLocal) aName = _local_impl->GetName();
  else aName = _corba_impl->GetName();
  return aName.ToCString();
}

SALOMEDSClient_SObject* SALOMEDS_UseCaseBuilder::AddUseCase(const char* theName)
{
  SALOMEDS_SObject* obj = NULL;
  TCollection_AsciiString aName((char*)theName);
  if(_isLocal) obj = new SALOMEDS_SObject(_local_impl->AddUseCase(aName));
  else obj = new SALOMEDS_SObject(_corba_impl->AddUseCase(aName.ToCString()));
  return obj;
}

SALOMEDSClient_UseCaseIterator* SALOMEDS_UseCaseBuilder::GetUseCaseIterator(SALOMEDSClient_SObject* theObject) 
{
  SALOMEDS_UseCaseIterator* it = NULL;
  SALOMEDS_SObject* obj = dynamic_cast<SALOMEDS_SObject*>(theObject);
  if(_isLocal) it = new SALOMEDS_UseCaseIterator(_local_impl->GetUseCaseIterator(obj->GetLocalImpl()));
  else it = new SALOMEDS_UseCaseIterator(_corba_impl->GetUseCaseIterator(obj->GetCORBAImpl()));
  return it;
}
