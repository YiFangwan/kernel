// File:	SALOME_Event.hxx
// Created:	Mon Mar 29 16:36:52 2004
// Author:	Sergey ANIKIN
//		<san@startrex.nnov.opencascade.com>


#ifndef SALOME_Event_HeaderFile
#define SALOME_Event_HeaderFile

#include <qevent.h>
#include <qsemaphore.h>

class SALOME_Semaphore : public QSemaphore
{
public:
  SALOME_Semaphore() : QSemaphore( 1 ) {}
  virtual ~SALOME_Semaphore();
};

class SALOME_Event : public QCustomEvent
{
public:
  SALOME_Event( int salomeEventType, SALOME_Semaphore* s = 0 );

  int getType() const { return myType; }
  void processed();
  
private:
  SALOME_Semaphore* getSemaphore() { return (SALOME_Semaphore*)data(); }

private:
  int myType;
};

#endif
