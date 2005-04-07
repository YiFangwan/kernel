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

  virtual std::string GetID();
  virtual SALOMEDSClient_SComponent* GetFatherComponent();
  virtual SALOMEDSClient_SObject*    GetFather();
  virtual bool FindAttribute(SALOMEDSClient_GenericAttribute*& anAttribute, const std::string& aTypeOfAttribute);
  virtual bool ReferencedObject(SALOMEDSClient_SObject*& theObject);
  virtual bool FindSubObject(int theTag, SALOMEDSClient_SObject*& theObject);
  virtual SALOMEDSClient_Study* GetStudy();
  virtual std::string Name();
  virtual void  Name(const std::string& theName);
  virtual vector<SALOMEDSClient_GenericAttribute*> GetAllAttributes();
  virtual std::string GetName();
  virtual std::string GetComment();
  virtual std::string GetIOR();
  virtual int   Tag();
  virtual int   Depth();

  CORBA::Object_ptr GetObject();
  SALOMEDS::SObject_ptr        GetCORBAImpl() { return SALOMEDS::SObject::_duplicate(_corba_impl); }
  Handle(SALOMEDSImpl_SObject) GetLocalImpl() { return _local_impl; }
};

#endif
