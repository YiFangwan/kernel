//  File   : SALOMEDS_ChildIterator_i.cxx
//  Author : Sergey RUIN
//  Module : SALOME

using namespace std;
#include "SALOMEDS_ChildIterator_i.hxx"
#include "SALOMEDS_SObject_i.hxx"
#include "SALOMEDS.hxx"
#include "SALOMEDSImpl_SObject.hxx"
#include "utilities.h"

//============================================================================
/*! Function : constructor
 *  Purpose  :
 */
//============================================================================
SALOMEDS_ChildIterator_i::SALOMEDS_ChildIterator_i(TDF_Label lab,
						   CORBA::ORB_ptr orb) 
  : _lab(lab)
{
  SALOMEDS::Locker lock;
  _orb = CORBA::ORB::_duplicate(orb);
  _it.Initialize (lab);
}

//============================================================================
/*! Function : destructor
 *  Purpose  :
 */
//============================================================================
SALOMEDS_ChildIterator_i::~SALOMEDS_ChildIterator_i()
{
}

//============================================================================
/*! Function :Init
 * 
 */
//============================================================================
void SALOMEDS_ChildIterator_i::Init()
{ 
  SALOMEDS::Locker lock;
  _it.Initialize (_lab);
}

//============================================================================
/*! Function :InitEx
 * 
 */
//============================================================================
void SALOMEDS_ChildIterator_i::InitEx(CORBA::Boolean allLevels)
{ 
  SALOMEDS::Locker lock;
  _it.Initialize (_lab, allLevels);
}

//============================================================================
/*! Function : More
 * 
 */
//============================================================================
CORBA::Boolean SALOMEDS_ChildIterator_i::More()
{
  SALOMEDS::Locker lock;
  return _it.More();
}

 //============================================================================
/*! Function : Next
 * 
 */
//============================================================================
void SALOMEDS_ChildIterator_i::Next()
{
  SALOMEDS::Locker lock;
  _it.Next();
}


//============================================================================
/*! Function :
 *  Purpose  :
 */
//============================================================================

SALOMEDS::SObject_ptr SALOMEDS_ChildIterator_i::Value()
{
  SALOMEDS::Locker lock;
  Handle(SALOMEDSImpl_SObject) aSO = new SALOMEDSImpl_SObject(_it.Value());
  SALOMEDS_SObject_i *  so_servant = new SALOMEDS_SObject_i (aSO, _orb);
  SALOMEDS::SObject_var so = SALOMEDS::SObject::_narrow(so_servant->_this());
  return so;
}

