//  File   : SALOMEDS_SObject.hxx
//  Author : Sergey RUIN
//  Module : SALOME

using namespace std;  

#include <string>
#include <TCollection_AsciiString.hxx> 
#include <TColStd_HSequenceOfTransient.hxx>

#include "SALOMEDS_SObject.hxx"
#include "SALOMEDS_SComponent.hxx"
#include "SALOMEDSImpl_SComponent.hxx"
#include "SALOMEDS_GenericAttribute.hxx"
#include "SALOMEDSImpl_GenericAttribute.hxx"
#include "SALOMEDS_Study.hxx"
#include "SALOMEDSImpl_Study.hxx"

#ifdef WIN32
#include <process.h>
#else
#include <sys/types.h>
#include <unistd.h>
#endif

#include "OpUtil.hxx"

SALOMEDS_SObject::SALOMEDS_SObject(SALOMEDS::SObject_ptr theSObject)
{
#ifdef WIN32
  long pid =  (long)_getpid();
#else
  long pid =  (long)getpid();
#endif  

  long addr = theSObject->GetLocalImpl(GetHostname().c_str(), pid, _isLocal);
  if(_isLocal) {
    _local_impl = ((SALOMEDSImpl_SObject*)(addr));
    _corba_impl = SALOMEDS::SObject::_nil();
  }
  else {
    _local_impl = NULL;
    _corba_impl = SALOMEDS::SObject::_duplicate(theSObject);
  }
}

SALOMEDS_SObject::SALOMEDS_SObject(const Handle(SALOMEDSImpl_SObject)& theSObject)
:_isLocal(true)
{
  _corba_impl = SALOMEDS::SObject::_nil();
  _local_impl = theSObject;
}

SALOMEDS_SObject::~SALOMEDS_SObject()
{
  if(!_isLocal) CORBA::release(_corba_impl);
}

std::string SALOMEDS_SObject::GetID()
{
  std::string aValue;
  if(_isLocal) aValue = _local_impl->GetID().ToCString();
  else aValue = _corba_impl->GetID();  
  return aValue;
}

SALOMEDSClient_SComponent* SALOMEDS_SObject::GetFatherComponent()
{
  if(_isLocal) 
    return new SALOMEDS_SComponent(Handle(SALOMEDSImpl_SComponent)::DownCast(_local_impl->GetFatherComponent()));
  return new SALOMEDS_SComponent(_corba_impl->GetFatherComponent());
}

SALOMEDSClient_SObject* SALOMEDS_SObject::GetFather()
{
  if(_isLocal) return new SALOMEDS_SObject(_local_impl->GetFather());
  return new SALOMEDS_SObject(_corba_impl->GetFather());
}

bool SALOMEDS_SObject::FindAttribute(SALOMEDSClient_GenericAttribute*& anAttribute, const std::string& aTypeOfAttribute)
{
  bool ret = false;
  if(_isLocal) {
    Handle(SALOMEDSImpl_GenericAttribute) anAttr;
    ret = _local_impl->FindAttribute(anAttr, (char*)aTypeOfAttribute.c_str());
    if(ret) anAttribute = SALOMEDS_GenericAttribute::CreateAttribute(anAttr);
  }
  else {
    SALOMEDS::GenericAttribute_var anAttr;
    ret = _corba_impl->FindAttribute(anAttr.out(), aTypeOfAttribute.c_str());
    if(ret) anAttribute = SALOMEDS_GenericAttribute::CreateAttribute(anAttr);
  }

  return ret;
}

bool SALOMEDS_SObject::ReferencedObject(SALOMEDSClient_SObject*& theObject)
{
  bool ret = false;
  if(_isLocal) {
    Handle(SALOMEDSImpl_SObject) aSO;
    ret = _local_impl->ReferencedObject(aSO);
    if(ret) theObject = new SALOMEDS_SObject(aSO);
  }
  else {
    SALOMEDS::SObject_var aSO;
    ret = _corba_impl->ReferencedObject(aSO.out());
    if(ret) theObject = new SALOMEDS_SObject(aSO);
  }

  return ret; 
}


bool SALOMEDS_SObject::FindSubObject(int theTag, SALOMEDSClient_SObject*& theObject)
{
  bool ret = false;
  if(_isLocal) {
    Handle(SALOMEDSImpl_SObject) aSO;
    ret = _local_impl->FindSubObject(theTag, aSO);
    if(ret) theObject = new SALOMEDS_SObject(aSO);
  }
  else {
    SALOMEDS::SObject_var aSO;
    ret = _corba_impl->FindSubObject(theTag, aSO.out());
    if(ret) theObject = new SALOMEDS_SObject(aSO);
  }

  return ret;   
}

SALOMEDSClient_Study* SALOMEDS_SObject::GetStudy()
{
  if(_isLocal) return new SALOMEDS_Study(_local_impl->GetStudy());
  return new SALOMEDS_Study(_corba_impl->GetStudy());
}

std::string SALOMEDS_SObject::Name()
{
  std::string aName;
  if(_isLocal) aName = _local_impl->Name().ToCString();
  else aName = _corba_impl->Name();

  return aName;
}

void  SALOMEDS_SObject::Name(const std::string& theName)
{
  if(_isLocal) _local_impl->Name((char*)theName.c_str());
  else _corba_impl->Name(theName.c_str());
}

vector<SALOMEDSClient_GenericAttribute*> SALOMEDS_SObject::GetAllAttributes()
{
  vector<SALOMEDSClient_GenericAttribute*> aVector;
  int aLength = 0;
  SALOMEDS_GenericAttribute* anAttr;

  if(_isLocal) {
    Handle(TColStd_HSequenceOfTransient) aSeq = _local_impl->GetAllAttributes();
    aLength = aSeq->Length();
    for(int i = 1; i <= aLength; i++) {
      anAttr = SALOMEDS_GenericAttribute::CreateAttribute(Handle(SALOMEDSImpl_GenericAttribute)::DownCast(aSeq->Value(i)));
      aVector.push_back(anAttr);
    }
  }
  else {
    SALOMEDS::ListOfAttributes_var aSeq = _corba_impl->GetAllAttributes();
    aLength = aSeq->length();
    for(int i = 0; i < aLength; i++) {
      anAttr = SALOMEDS_GenericAttribute::CreateAttribute(aSeq[i]);
      aVector.push_back(anAttr);
    }
  }

  return aVector;
}

std::string SALOMEDS_SObject::GetName()
{
  std::string aName;
  if(_isLocal) aName = _local_impl->GetName().ToCString();
  else aName = _corba_impl->GetName();

  return aName;
}

std::string SALOMEDS_SObject::GetComment()
{
  std::string aComment;
  if(_isLocal) aComment = _local_impl->GetComment().ToCString();
  else aComment = _corba_impl->GetComment();

  return aComment;
}

std::string SALOMEDS_SObject::GetIOR()
{
  std::string anIOR;
  if(_isLocal) anIOR = _local_impl->GetIOR().ToCString();
  else anIOR = _corba_impl->GetIOR();

  return anIOR;
}

int SALOMEDS_SObject::Tag()
{
  if(_isLocal) return _local_impl->Tag();
  return _corba_impl->Tag(); 
}

int SALOMEDS_SObject::Depth()
{
  if(_isLocal) return _local_impl->Depth();
  return _corba_impl->Depth();  
}

CORBA::Object_ptr SALOMEDS_SObject::GetObject()
{
  CORBA::Object_var obj;
  if(_isLocal) {
    std::string anIOR = GetIOR();
    SALOMEDS_Study* aStudy = dynamic_cast<SALOMEDS_Study*>(GetStudy());
    obj = CORBA::Object::_duplicate(aStudy->ConvertIORToObject(anIOR));
    delete aStudy;
  }
  else obj = CORBA::Object::_duplicate(_corba_impl->GetObject());
  return obj._retn();
}
