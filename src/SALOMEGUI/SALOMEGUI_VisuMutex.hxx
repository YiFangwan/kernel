
#ifndef _SALOMEGUI_VISUMUTEX_HXX_
#define _SALOMEGUI_VISUMUTEX_HXX_

#include <qapplication.h>
#include <qthread.h> 

namespace VISU
{
  
  class Mutex
  {
    QMutex* myMutex;
    QApplication* myQApp;
    int isQAppLocked, isSessionLocked, myDelay;
  public:
    Mutex(QMutex* theMutex, QApplication* theQApp, int theDelay = 0);
    ~Mutex();
  };

  class SMutex
  {
    QMutex* myMutex;
    QApplication* myQApp;
    int isQAppLocked, isSessionLocked, myDelay;
  public:
    SMutex(QMutex* theMutex, QApplication* theQApp, int theDelay = 0);
    ~SMutex();
  };
}

#endif
