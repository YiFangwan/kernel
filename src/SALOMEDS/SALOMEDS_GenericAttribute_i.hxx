//  File   : SALOMEDS_GenericAttribute_i.hxx
//  Author : Sergey RUIN
//  Module : SALOME

#ifndef _GENERIC_ATTRIBUTE_I_HXX_
#define _GENERIC_ATTRIBUTE_I_HXX_

// IDL headers
#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(SALOMEDS)
#include <TDF_Attribute.hxx>
#include "SALOMEDS_SObject_i.hxx"
#include "SALOMEDSImpl_GenericAttribute.hxx"

class SALOMEDS_GenericAttribute_i: public POA_SALOMEDS::GenericAttribute,
				   public PortableServer::RefCountServantBase 
{

protected:
  Handle(TDF_Attribute) _impl;
  CORBA::ORB_ptr        _orb;

public:
  SALOMEDS_GenericAttribute_i(const Handle(TDF_Attribute)& theImpl, CORBA::ORB_ptr theOrb);
  virtual ~SALOMEDS_GenericAttribute_i() {};

  void CheckLocked() throw (SALOMEDS::GenericAttribute::LockProtection);
  
  virtual char* Type();

  char* GetClassType();

  SALOMEDS::SObject_ptr GetSObject();
  
  virtual Handle(TDF_Attribute) GetImpl() { return _impl; }

  static SALOMEDS::GenericAttribute_ptr CreateAttribute(const Handle(TDF_Attribute)& theAttr, CORBA::ORB_ptr theOrb);  

  virtual long GetLocalImpl(const char* theHostname, CORBA::Long thePID, CORBA::Boolean& isLocal);
};

#endif
