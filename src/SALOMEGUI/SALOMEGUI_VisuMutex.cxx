

#include "SALOMEGUI_VisuMutex.hxx"
#include "utilities.h"

using namespace std;

namespace VISU
{
  static int mySCnt = 0;
  static int myQCnt = 0;
  //static QMutex localMutex;
  //static QMutex localSMutex;

  Mutex::Mutex(QMutex* theMutex, QApplication* theQApp, int theDelay) :
    myQApp(theQApp), myDelay(theDelay), myMutex(theMutex)
  {
    MESSAGE("-+-+-+-");
    isQAppLocked=theQApp->locked();
    isSessionLocked=theMutex->locked();
    MESSAGE(" ===== Mutex::Mutex : "<<isQAppLocked<<" "<<myQCnt<<" "<<isSessionLocked<<" "<<mySCnt);
    myQApp->lock(); 
    myQApp->syncX();
    myQCnt++;
    if (myQCnt > 1) MESSAGE( " --- myQCnt : " << myQCnt);
    MESSAGE ("- = - = -");
  }

  Mutex::~Mutex()
  {
    MESSAGE("~~~~~~~");
    myQCnt--; 
    myQApp->flushX(); 
    myQApp->unlock();
    MESSAGE(" ===== Mutex::~Mutex : "<<isQAppLocked<<" "<<myQCnt<<" "<<isSessionLocked<<" "<<mySCnt);
    MESSAGE("~ ~ ~ ~");
  }

  // ---------------------------------------------------------------------

  SMutex::SMutex(QMutex* theMutex, QApplication* theQApp, int theDelay) :
    myQApp(theQApp), myDelay(theDelay), myMutex(theMutex)
  {
    MESSAGE(" ooo 1");
    //localSMutex.lock();
    isSessionLocked=theMutex->locked();
    MESSAGE(" ///// SMutex::SMutex : "<<isSessionLocked<<" "<<mySCnt);
    if(!isSessionLocked && !mySCnt)
      {
	myMutex->lock();
	MESSAGE(" ooo 2");
      }
    mySCnt++;
    if (mySCnt > 1) MESSAGE( " --- mySCnt : " << mySCnt);
    //localSMutex.unlock();
    MESSAGE (" ooo 3");
  }

  SMutex::~SMutex()
  {
    MESSAGE(" _-_ 1");
    //localSMutex.lock();
    mySCnt--;
    if(!isSessionLocked && !mySCnt)
      {
	myMutex->unlock();
	MESSAGE(" _-_ 2");
      }
    MESSAGE(" ///// SMutex::~SMutex : "<<isSessionLocked<<" "<<mySCnt);
    //localSMutex.unlock();
    MESSAGE(" _-_ 3");
  }

}
