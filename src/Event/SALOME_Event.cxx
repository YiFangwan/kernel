//  KERNEL SALOME_Event : Define event posting mechanism
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
//  File   : SALOME_Event.cxx
//  Author : Sergey ANIKIN
//  Module : KERNEL
//  $Header$

#include "SALOME_Event.hxx"

#include "utilities.h"

#include <qsemaphore.h>
#include <qapplication.h>
#include <qthread.h>

//===========================================================
/*!
 *  SALOME_Event::SALOME_Event
 *  Constructor
 */
//===========================================================
SALOME_Event::SALOME_Event( int salomeEventType, bool wait, bool autoRelease )
: myType( salomeEventType ), 
  myWait( wait ),
  myAutoRelease( autoRelease )
{
  MESSAGE( "SALOME_Event::SALOME_Event(): myType = "<<myType<<", myWait = "<<myWait );
  if ( wait ) {
    // Prepare the semaphore 
    mySemaphore = new QSemaphore( 2 );
    mySemaphore->operator+=( 2 );
  }
}

//===========================================================
/*!
 *  SALOME_Event::~SALOME_Event
 *  Destructor
 */
//===========================================================
SALOME_Event::~SALOME_Event()
{
  MESSAGE( "SALOME_Event::~SALOME_Event(): myType = "<<myType<<", myWait = "<<myWait );
  if ( myWait ) {
    if ( mySemaphore->available() < mySemaphore->total() )
      mySemaphore->operator-=( mySemaphore->total() - mySemaphore->available() );
    delete mySemaphore;
  }
}

//===========================================================
/*!
 *  SALOME_Event::process
 *  Posts the event and optionally waits for its completion
 */
//===========================================================
void SALOME_Event::process()
{
  MESSAGE( "SALOME_Event::process(): myType = "<<myType<<",myWait = "<<myWait );
  QThread::postEvent( qApp, new QCustomEvent( SALOME_EVENT, (void*)this ) );
  if ( myWait ) {
    MESSAGE( "SALOME_Event::process(): available = " << mySemaphore->available() );
    if ( !mySemaphore->available() )
      mySemaphore->operator+=( 1 );

    MESSAGE( "SALOME_Event::process() COMPLETED: myType = "<<myType<<",myWait = "<<myWait );
  }
  if ( myAutoRelease )
    release();
}

//===========================================================
/*!
 *  SALOME_Event::processed
 *  Signals that this event has been processed
 */
//===========================================================
void SALOME_Event::processed()
{
  MESSAGE( "SALOME_Event::processed(): myType = "<<myType<<",myWait = "<<myWait );
  if ( myWait ) {
    MESSAGE( "SALOME_Event::processed(): available = " << mySemaphore->available() );
    if ( !mySemaphore->available() ) {
      // process() takes control over mySemaphore after the next line is executed
      mySemaphore->operator-=( 1 );

      MESSAGE( "SALOME_Event::processed(): semaphore DECREMENTED" );

      // Current thread will block here until process() completes
      mySemaphore->operator+=( mySemaphore->total() );
    }
  }
  MESSAGE( "SALOME_Event::processed() COMPLETED: myType = "<<myType<<",myWait = "<<myWait );
}

//===========================================================
/*!
 *  SALOME_Event::release
 *  Wakes up the desktop
 */
//===========================================================
void SALOME_Event::release()
{
  MESSAGE( "SALOME_Event::release(): myType = "<<myType<<",myWait = "<<myWait );
  if ( myWait ) {
    MESSAGE( "SALOME_Event::release(): available = " << mySemaphore->available() );
    mySemaphore->operator-=( mySemaphore->total() - mySemaphore->available() );
  }
  MESSAGE( "SALOME_Event::release() COMPLETED: myType = "<<myType<<",myWait = "<<myWait );
}




