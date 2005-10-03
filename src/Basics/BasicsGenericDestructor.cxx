//  SALOME Basics : general SALOME definitions and tools (C++ part - no CORBA)
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS 
// 
//  This library is free software; you can redistribute it and/or 
//  modify it under the terms of the GNU Lesser General Public 
//  License as published by the Free Software Foundation; either 
//  version 2.1 of the License. 
// 
//  This library is distributed in the hope that it will be useful, 
//  but WITHOUT ANY WARRANTY; without even the implied warranty of 
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
//  Lesser General Public License for more details. 
// 
//  You should have received a copy of the GNU Lesser General Public 
//  License along with this library; if not, write to the Free Software 
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA 
// 
//  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 
//
//
//
//  File   : BasicGenericDestructor.cxx
//  Author : Antoine YESSAYAN, Paul RASCLE, EDF
//  Module : SALOME
//  $Header$

#include <iostream>
#include <list>
#include <cstdlib>

#include "BasicsGenericDestructor.hxx"

using namespace std;

void HouseKeeping();

std::list<PROTECTED_DELETE*> PROTECTED_DELETE::_objList;
pthread_mutex_t PROTECTED_DELETE::_listMutex;

std::list<GENERIC_DESTRUCTOR*> *GENERIC_DESTRUCTOR::Destructors = 0;
static bool atExitSingletonDone = false ;

void PROTECTED_DELETE::deleteInstance(PROTECTED_DELETE *anObject)
  {
    if (std::find(_objList.begin(), _objList.end(),anObject) == _objList.end())
      return;
    else
      {
	int ret;
	ret = pthread_mutex_lock(&_listMutex); // acquire lock, an check again
	if (std::find(_objList.begin(), _objList.end(), anObject)
	    != _objList.end())
	  {
	    cerr << "PROTECTED_DELETE::deleteInstance1 " << anObject << endl;
	    delete anObject;
	    cerr << "PROTECTED_DELETE::deleteInstance2 " << &_objList << endl;
	    _objList.remove(anObject);
	    cerr << "PROTECTED_DELETE::deleteInstance3" << endl;
	  }
	ret = pthread_mutex_unlock(&_listMutex); // release lock
      }
  }

void PROTECTED_DELETE::addObj(PROTECTED_DELETE *anObject)
{
  cerr << "PROTECTED_DELETE::addObj " << anObject << endl;
  _objList.push_back(anObject);
}

PROTECTED_DELETE::~PROTECTED_DELETE()
{
  cerr << "PROTECTED_DELETE::~PROTECTED_DELETE()" << endl;
}

// ============================================================================
/*! 
 * To execute only once GENERIC_DESTRUCTOR::HouseKeeping et the end of process,
 * a dedicated object is created, as a singleton: atExitSingleton.
 * When the singleton is created, the HouseKeeping() function is registered in
 * atExit().
 * Destructors is a list created on heap, and deleted by HouseKeeping(), with
 * the list content.
 */
// ============================================================================

class atExitSingleton
{
public:
  atExitSingleton(bool Make_ATEXIT)
  {
    if (Make_ATEXIT && !atExitSingletonDone)
      {
	cerr << "atExitSingleton(" << Make_ATEXIT << ")" << endl;
	assert(GENERIC_DESTRUCTOR::Destructors == 0);
	GENERIC_DESTRUCTOR::Destructors = new std::list<GENERIC_DESTRUCTOR*>;
	int cr = atexit(HouseKeeping);
	assert(cr == 0);
	atExitSingletonDone = true;
      }
  }

  ~atExitSingleton()
  {
    cerr << "atExitSingleton::~atExitSingleton()" << endl << flush;
  }
};

//! static singleton for atExitSingleton class

static atExitSingleton HouseKeeper = atExitSingleton(false);

// ============================================================================
/*! 
 *  Executes all objects of type DESTRUCTOR_OF in the Destructors list. 
 *  Deletes  all objects of type DESTRUCTOR_OF in the Destructors list.
 *  Deletes the list.
 */
// ============================================================================

void HouseKeeping( void )
{
  cerr << "HouseKeeping()" << endl;
  assert(GENERIC_DESTRUCTOR::Destructors);
  if(GENERIC_DESTRUCTOR::Destructors->size())
    {
      std::list<GENERIC_DESTRUCTOR*>::iterator it =
	GENERIC_DESTRUCTOR::Destructors->end();

      do
	{
	  it-- ;
	  GENERIC_DESTRUCTOR* ptr = *it ;
	  cerr << "HouseKeeping() " << typeid(ptr).name() << endl;
	  (*ptr)();
	  delete ptr ;
	}
      while (it !=  GENERIC_DESTRUCTOR::Destructors->begin()) ;

      cerr << "HouseKeeping() end list "<< endl;
      GENERIC_DESTRUCTOR::Destructors->clear() ;
      assert(GENERIC_DESTRUCTOR::Destructors->size() == 0);
      assert(GENERIC_DESTRUCTOR::Destructors->empty());
      cerr << "HouseKeeping()after clear "<< endl;
    }

  delete GENERIC_DESTRUCTOR::Destructors;
  GENERIC_DESTRUCTOR::Destructors = 0;
  cerr << "HouseKeeping() very end "<< endl;
  return ;
}

// ============================================================================
/*!
 * Adds a destruction object to the list of actions to be performed at the end
 * of the process
 */
// ============================================================================

const int GENERIC_DESTRUCTOR::Add(GENERIC_DESTRUCTOR &anObject)
{
  cerr << "GENERIC_DESTRUCTOR::Add("<<typeid(anObject).name()<<") "
       << &anObject << endl;
  if (!atExitSingletonDone)
    {
      HouseKeeper = atExitSingleton(true);
    }
  assert(Destructors);
//   if (std::find(Destructors->begin(), Destructors->end(), &anObject)
//       == Destructors->end())
//     {
//       Destructors->push_back(&anObject);
//     }
  Destructors->push_back(&anObject);
  return Destructors->size();
}
