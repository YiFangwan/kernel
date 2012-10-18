// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

//  File   : SALOMEDS_SimanStudy_i.cxx
//  Author : Mikhail PONIKAROV
//  Module : SALOME
//
#include "utilities.h"
#include "SALOMEDS_SimanStudy_i.hxx"
#include "SALOMEDSImpl_SimanStudy.hxx"
#include "SALOMEDSImpl_StudyManager.hxx"
#include "SALOMEDS_Study_i.hxx"
#include "SALOMEDS_Study.hxx"

#include "Basics_Utils.hxx"

//============================================================================
/*! Function : SALOMEDS_SimanStudy_i
 *  Purpose  : standard constructor
 */
//============================================================================
SALOMEDS_SimanStudy_i::SALOMEDS_SimanStudy_i(SALOMEDSImpl_SimanStudy* theImpl, CORBA::ORB_ptr orb)
{
  _orb = CORBA::ORB::_duplicate(orb);
  _impl = theImpl;
}

//============================================================================
/*! Function : ~SALOMEDS_SimanStudy_i
 *  Purpose  : standard destructor
 */
//============================================================================
SALOMEDS_SimanStudy_i::~SALOMEDS_SimanStudy_i()
{
  //delete implementation
  delete _impl;
}

//============================================================================
/*! Function : CheckOut
 *  Purpose  : Get data from SIMAN and put it to the given study
 */
//============================================================================
void SALOMEDS_SimanStudy_i::CheckOut(SALOMEDS::Study_ptr theTarget)
{
  // SimanStudy and Study must be located at the same place anyway
  SALOMEDS_Study aStudy(theTarget);
  _study = aStudy.GetLocalImpl();
  if (_study) {
    _impl->CheckOut(_study);
  }
}

//============================================================================
/*! Function : CheckIn
 *  Purpose  : Get data from SIMAN study and stores to SIMAN
 */
//============================================================================
void SALOMEDS_SimanStudy_i::CheckIn(const char* theModuleName)
{
  if (_study) {
    _impl->CheckIn(theModuleName);
  }
}

//============================================================================
/*! Function : getReferencedStudy
 *  Purpose  : Returns the %Study with checked out data
 */
//============================================================================
SALOMEDS::Study_ptr SALOMEDS_SimanStudy_i::getReferencedStudy()
{
  SALOMEDS::Study_var aStudy = (new SALOMEDS_Study_i(_study, _orb))->_this();
  return aStudy._retn();
}

//============================================================================
/*! Function : StudyId
 *  Purpose  : The ID of the study in SIMAN server
 */
//============================================================================
char* SALOMEDS_SimanStudy_i::StudyId()
{
  return CORBA::string_dup(_impl->StudyId().c_str());
}

//============================================================================
/*! Function : StudyId
 *  Purpose  : The ID of the study in SIMAN server
 */
//============================================================================
void SALOMEDS_SimanStudy_i::StudyId(const char* theId)
{
  _impl->StudyId(theId);
}

//============================================================================
/*! Function : ScenarioId
 *  Purpose  : The ID of the scenario in SIMAN server
 */
//============================================================================
char* SALOMEDS_SimanStudy_i::ScenarioId()
{
  return CORBA::string_dup(_impl->ScenarioId().c_str());
}

//============================================================================
/*! Function : ScenarioId
 *  Purpose  : The ID of the scenario in SIMAN server
 */
//============================================================================
void SALOMEDS_SimanStudy_i::ScenarioId(const char* theId)
{
  _impl->StudyId(theId);
}

//============================================================================
/*! Function : UserId
 *  Purpose  : The ID of the user in SIMAN server
 */
//============================================================================
char* SALOMEDS_SimanStudy_i::UserId()
{
  return CORBA::string_dup(_impl->UserId().c_str());
}

//============================================================================
/*! Function : UserId
 *  Purpose  : The ID of the user in SIMAN server
 */
//============================================================================
void SALOMEDS_SimanStudy_i::UserId(const char* theId)
{
  _impl->UserId(theId);
}


SALOMEDS_SimanStudy_i* SALOMEDS_SimanStudy_i::GetSimanServant(SALOMEDSImpl_SimanStudy* aSimanImpl, CORBA::ORB_ptr orb)
{
  static SALOMEDS_SimanStudy_i* aServant = 0;
  if (aServant == 0) {
    aServant = new SALOMEDS_SimanStudy_i(aSimanImpl, orb);
  }
  return aServant;
}
                                  
/// PRIVATE FUNCTIONS
CORBA::LongLong SALOMEDS_SimanStudy_i::GetLocalImpl(const char* theHostname, CORBA::Long thePID, CORBA::Boolean& isLocal)
{
#ifdef WIN32
  long pid = (long)_getpid();
#else
  long pid = (long)getpid();
#endif
  isLocal = (strcmp(theHostname, Kernel_Utils::GetHostname().c_str()) == 0 && pid == thePID)?1:0;
  return reinterpret_cast<CORBA::LongLong>(_impl);
}
