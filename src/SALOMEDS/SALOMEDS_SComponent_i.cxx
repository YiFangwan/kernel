//  File   : SALOMEDS_SComponent_i.cxx
//  Author : Sergey RUIN
//  Module : SALOME

using namespace std;
#include "SALOMEDS_SComponent_i.hxx"
#include "SALOMEDS.hxx"
#include "utilities.h"


//============================================================================
/*! Function : constructor
 *  Purpose  : 
 */
//============================================================================
SALOMEDS_SComponent_i::SALOMEDS_SComponent_i(const Handle(SALOMEDSImpl_SComponent)& theImpl, CORBA::ORB_ptr orb)
  :SALOMEDS_SObject_i(theImpl, orb)
{}
  
//============================================================================
/*! Function : destructor
 *  Purpose  : 
 */
//============================================================================
SALOMEDS_SComponent_i::~SALOMEDS_SComponent_i()
{}
   
//============================================================================
/*! Function : ComponentDataType
 *  Purpose  : 
 */
//============================================================================
char* SALOMEDS_SComponent_i::ComponentDataType()
{
  SALOMEDS::Locker lock;
  TCollection_AsciiString aType = Handle(SALOMEDSImpl_SComponent)::DownCast(_impl)->ComponentDataType();
  return CORBA::string_dup(aType.ToCString());
}
  

//============================================================================
/*! Function : ComponentIOR
 *  Purpose  : 
 */
//============================================================================
CORBA::Boolean SALOMEDS_SComponent_i::ComponentIOR(CORBA::String_out IOR)
{
  SALOMEDS::Locker lock;
  TCollection_AsciiString ior;
  if(!Handle(SALOMEDSImpl_SComponent)::DownCast(_impl)->ComponentIOR(ior)) return false;
  IOR = CORBA::string_dup(ior.ToCString());
  return true;
}
