//  File   : SALOMEDS_SObject.hxx
//  Author : Sergey RUIN
//  Module : SALOME

#ifndef __SALOMEDS_SOBJECT_H__
#define __SALOMEDS_SOBJECT_H__

// std C++ headers
#include <vector>

#include <SALOMEDSClient_GenericAttribute.hxx>
#include <SALOMEDSClient_SComponent.hxx>
#include <SALOMEDSClient_Study.hxx>

// IDL headers
#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(SALOMEDS)

#include "SALOMEDS_SObject_i.hxx"
#include "SALOMEDSImpl_SObject.hxx"

class SALOMEDS_SObject: public virtual SALOMEDSClient_SObject
{
protected:

bool                         _isLocal;
Handle(SALOMEDSImpl_SObject) _local_impl;
SALOMEDS::SObject_var        _corba_impl;

public:
  
  SALOMEDS_SObject(SALOMEDS::SObject_ptr theSObject);
  SALOMEDS_SObject(const Handle(SALOMEDSImpl_SObject)& theSObject);       
  virtual ~SALOMEDS_SObject();

  virtual char* GetID();
  virtual SALOMEDSClient_SComponent* GetFatherComponent();
  virtual SALOMEDSClient_SObject*    GetFather();
  virtual bool FindAttribute(SALOMEDSClient_GenericAttribute* anAttribute, const char* aTypeOfAttribute);
  virtual bool ReferencedObject(SALOMEDSClient_SObject* theObject);
  virtual bool FindSubObject(int theTag, SALOMEDSClient_SObject* theObject);
  virtual SALOMEDSClient_Study* GetStudy();
  virtual char* Name();
  virtual void  Name(const char* theName);
  virtual vector<SALOMEDSClient_GenericAttribute*> GetAllAttributes();
  virtual char* GetName();
  virtual char* GetComment();
  virtual char* GetIOR();
  virtual int   Tag();
  virtual int   Depth();

  SALOMEDS::SObject_ptr        GetCORBAImpl() { return SALOMEDS::SObject::_duplicate(_corba_impl); }
  Handle(SALOMEDSImpl_SObject) GetLocalImpl() { return _local_impl; }
};

#endif
