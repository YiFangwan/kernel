// File:	SALOME_Event.cxx
// Created:	Tue Mar 30 15:06:32 2004
// Author:	Sergey ANIKIN
//		<san@startrex.nnov.opencascade.com>


#include "SALOME_Event.hxx"

#include "utilities.h"

//===========================================================================
// ~SALOME_Semaphore
//===========================================================================
SALOME_Semaphore::~SALOME_Semaphore()
{
}

//===========================================================================
// SALOME_Event
//===========================================================================
SALOME_Event::SALOME_Event( int salomeEventType, SALOME_Semaphore* s )
: QCustomEvent( (QEvent::Type)(QEvent::User + 1), (void*)s ), 
  myType( salomeEventType )
{
  MESSAGE( "SALOME_Event::SALOME_Event(): type = " << myType << ", semaphore = " << s );
  if ( s ) {
    MESSAGE( "SALOME_Event::SALOME_Event(): available = " << s->available() );
    ASSERT( s->available() == 1 );
    s->operator++( 1 );
  }
}

//===========================================================================
// processed
//===========================================================================
void SALOME_Event::processed()
{
  MESSAGE( "SALOME_Event::processed()" );
  if ( getSemaphore() && !getSemaphore()->available() ) {
    MESSAGE( "SALOME_Event::SALOME_Event(): available = " << getSemaphore()->available() );
    getSemaphore()->operator--( 1 );
  }
}



