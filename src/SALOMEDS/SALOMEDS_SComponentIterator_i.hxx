//  File   : SALOMEDS_SComponentIterator_i.hxx
//  Author : Sergey RUIN
//  Module : SALOME

#ifndef __SALOMEDS_SCOMPONENTITERATOR_I_H__
#define __SALOMEDS_SCOMPONENTITERATOR_I_H__

// std C++ headers
#include <iostream.h>

// IDL headers
#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(SALOMEDS)

//SALOMEDS headers
#include "SALOMEDS_SComponent_i.hxx"
#include "SALOMEDSImpl_SComponentIterator.hxx"

class SALOMEDS_SComponentIterator_i:public POA_SALOMEDS::SComponentIterator,
				    public PortableServer::RefCountServantBase 
{

private:

  CORBA::ORB_ptr                  _orb;
  SALOMEDSImpl_SComponentIterator _impl;

public:
  
  SALOMEDS_SComponentIterator_i(const SALOMEDSImpl_SComponentIterator& theImpl, CORBA::ORB_ptr);
  
  ~SALOMEDS_SComponentIterator_i();
  
  virtual void Init();
  virtual CORBA::Boolean More();
  virtual void Next();
  virtual SALOMEDS::SComponent_ptr Value();  
};
#endif
