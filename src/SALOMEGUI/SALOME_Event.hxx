// File:	SALOME_Event.hxx
// Created:	Mon Mar 29 16:36:52 2004
// Author:	Sergey ANIKIN
//		<san@startrex.nnov.opencascade.com>


#ifndef SALOME_Event_HeaderFile
#define SALOME_Event_HeaderFile

#include <qevent.h>

class QWaitCondition;

class SALOME_Event : public QCustomEvent
{
public:
  SALOME_Event( QWaitCondition* wc = 0 ) : QCustomEvent( (QEvent::Type)(QEvent::User + 1), (void*)wc ) {};

  QWaitCondition* getWaitCondition() { return (QWaitCondition*)data(); }
  
};

#endif
