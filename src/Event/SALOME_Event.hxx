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
//  File   : SALOME_Event.hxx
//  Author : Sergey ANIKIN
//  Module : KERNEL
//  $Header$


#ifndef SALOME_Event_HeaderFile
#define SALOME_Event_HeaderFile

#include <qevent.h>

#define SALOME_EVENT QEvent::Type( QEvent::User + 10000 )

class QSemaphore;

//===========================================================
/*!
 *  Class: SALOME_Event
 *  Description: 
 *  This class encapsulates data and functionality required for 
 *  posting component-specific events. These events are then dispatched
 *  by QAD_Desktop to corresponding component GUI object, on the basis of 
 *  <type> value passed to the constructor. SALOME_Event objects can be used 
 *  by any thread belonging to the GUI process.
 *
 *  It is possible to make the thread that creates SALOME_Event
 *  wait until the event is processed by the component GUI, SALOME_Event
 *  should be constructed with <wait> == TRUE in such a case.
 *
 *  SALOME_Event objects should be created on the heap. QAD_Desktop deletes
 *  these objects as soon as they have been processed.
 *
 *  Usage:
 *  - create SALOME_Event object on the heap with proper <type> and <wait> parameters. 
 *    Components can derive their own event class from SALOME_Event
 *    in order to pass custom data to the event handler.
 *  - call process() method to post the event.
 * 
 *  processed() method is used by the desktop to signal that event processing 
 *  has been completed.
 *  
 *  NOTE: 
 *  1. Never create SALOME_Event with <wait> == TRUE in code that is 
 *     supposed to be called from built-in GUI Python console, for this will result
 *     in GUI process deadlock.
 *  2. Never use pointers to the event after it has been processed to avoid
 *     application crashes!
 */
//===========================================================

class SALOME_Event
{
public:
  SALOME_Event( int salomeEventType, bool wait );
  virtual ~SALOME_Event();

  int getType() const { return myType; }

  void process();
  void processed();

private:
  int         myType;
  bool        myWait;
  QSemaphore* mySemaphore;
};

#endif
