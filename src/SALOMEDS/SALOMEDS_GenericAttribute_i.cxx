//  File   : SALOMEDS_GenericAttribute_i.cxx
//  Author : Sergey RUIN
//  Module : SALOME

using namespace std;
#include "utilities.h"
#include "SALOMEDS_GenericAttribute_i.hxx"
#include "SALOMEDS_Attributes.hxx"
#include "SALOMEDS.hxx"
#include "SALOMEDSImpl_SObject.hxx"
#include "Utils_ExceptHandlers.hxx"
#include <TCollection_AsciiString.hxx>

UNEXPECT_CATCH(GALockProtection, SALOMEDS::GenericAttribute::LockProtection);

SALOMEDS_GenericAttribute_i::SALOMEDS_GenericAttribute_i(const Handle(TDF_Attribute)& theImpl, CORBA::ORB_ptr theOrb)
{
  _orb = CORBA::ORB::_duplicate(theOrb);
  _impl = theImpl;
}

void SALOMEDS_GenericAttribute_i::CheckLocked() throw (SALOMEDS::GenericAttribute::LockProtection) 
{
  SALOMEDS::Locker lock;
  Unexpect aCatch(GALockProtection);

  if (!_impl.IsNull() && _impl->IsValid() && !CORBA::is_nil(_orb)) {
    try {
      SALOMEDSImpl_GenericAttribute::Impl_CheckLocked(_impl);
    }
    catch (...) {
      throw SALOMEDS::GenericAttribute::LockProtection();
    }
  }
}

SALOMEDS::SObject_ptr SALOMEDS_GenericAttribute_i::GetSObject() 
{
  SALOMEDS::Locker lock;
  if (_impl.IsNull() || _impl->Label().IsNull()) return SALOMEDS::SObject::_nil();
  Handle(SALOMEDSImpl_SObject) so_impl = new SALOMEDSImpl_SObject(_impl->Label());
  SALOMEDS_SObject_i *  so_servant = new SALOMEDS_SObject_i (so_impl, _orb);
  SALOMEDS::SObject_var so = SALOMEDS::SObject::_narrow(so_servant->_this()); 
  return so._retn();
}


char* SALOMEDS_GenericAttribute_i::Type() 
{
  SALOMEDS::Locker lock;
  if (!_impl.IsNull()) {
    return CORBA::string_dup(SALOMEDSImpl_GenericAttribute::Impl_GetType(_impl));
  }    

  return "";
}


SALOMEDS::GenericAttribute_ptr SALOMEDS_GenericAttribute_i::CreateAttribute(const Handle(TDF_Attribute)& theAttr,
									    CORBA::ORB_ptr theOrb) 
{
  SALOMEDS::Locker lock;
  char* aTypeOfAttribute = Handle(SALOMEDSImpl_GenericAttribute)::DownCast(theAttr)->GetClassType().ToCString();
  SALOMEDS::GenericAttribute_var anAttribute;
  __CreateGenericCORBAAttribute
  return anAttribute._retn(); 
}                                                                                                                    
