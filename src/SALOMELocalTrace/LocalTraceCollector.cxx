//  Copyright (C) 2004  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
//  File   : LocalTraceCollector.cxx
//  Author : Paul RASCLE (EDF)
//  Module : KERNEL
//  $Header$

#include <iostream>
#include <fstream>

#include "LocalTraceCollector.hxx"

using namespace std;

LocalTraceCollector* LocalTraceCollector::_singleton = 0;
int LocalTraceCollector::_threadToClose = 0;
pthread_t LocalTraceCollector::_threadId = 0;
int LocalTraceCollector::_toFile = 0;           // public, set by user

// ============================================================================
/*!
 *  guarantees a unique object instance of the class (singleton)
 *  Must be done at first because not thread safe: 
 *  _singleton is not protected by a mutex
 *  LocalTraceCollector instance is created, then LocalTraceBufferPool
 *  instance is created immediately in the same thread (not thread safe, too).
 *  After thet, a separate thread for loop to print traces is launched.
 */
// ============================================================================

LocalTraceCollector* LocalTraceCollector::instance()
{
  if (_singleton == 0)
    {
      _singleton = new LocalTraceCollector();
      LocalTraceBufferPool* initTraceBuffer = LocalTraceBufferPool::instance();
      pthread_t traceThread;
      int bid;
      int ret = pthread_create(&traceThread, NULL,
			       LocalTraceCollector::run, (void *)bid);
    }
  return _singleton;
}

// ============================================================================
/*!
 *  In a separate thread, loop to print traces.
 *  Loop until there is no more buffer to print,
 *  and no ask for end from destructor.
 *  Get a buffer. If type = ABORT then exit application with message.
 */
// ============================================================================

void* LocalTraceCollector::run(void *bid)
{
  if (! _threadId)  // only one run
    { 
      _threadId = pthread_self();
      LocalTraceBufferPool* myTraceBuffer = LocalTraceBufferPool::instance();
      LocalTrace_TraceInfo myTrace;

      // if trace in file requested, opens a file with append mode
      // so, several processes can share the same file

      ofstream traceFile;
      if (_toFile)
	{
	  const char *fileName = "/tmp/tracetest.log";
	  traceFile.open(fileName, ios::out | ios::app);
	  if (!traceFile)
	    {
	      cerr << "impossible to open trace file "<< fileName << endl;
	      exit (1);
	    }
	}

      // Loop until there is no more buffer to print,
      // and no ask for end from destructor.

      while ((!_threadToClose) || myTraceBuffer->toCollect() )
	{
	  int fullBuf = myTraceBuffer->retrieve(myTrace);
	  if (myTrace.traceType == ABORT_MESS)
	    {
	      cout << flush ;
	      cerr << "INTERRUPTION from thread " << myTrace.threadId
		   << " : " <<  myTrace.trace;
	      cerr << flush ; 
	      exit(1);     
	    }
	  else
	    {
	      if (_toFile)
		traceFile << myTrace.position << " thread " << myTrace.threadId
			  << " full " << fullBuf <<" " << myTrace.trace;
	      else
		cout << " th. " << myTrace.threadId
		     << " " << fullBuf <<" " << myTrace.trace;
	    }
	}

      if (_toFile) traceFile.close();
    }
  pthread_exit(NULL);
  //return 0; // just for warning
}

// ============================================================================
/*!
 *  Destructor: wait until printing thread ends (LocalTraceCollector::run)
 */
// ============================================================================

LocalTraceCollector:: ~LocalTraceCollector()
{
  _threadToClose = 1;
  if (_threadId)
    {
      int ret = pthread_join(_threadId, NULL);
      if (ret) cout << "error close LocalTraceCollector : "<< ret << endl;
      else cout << "LocalTraceCollector destruction OK" << endl;
    }
  LocalTraceBufferPool* myTraceBuffer = LocalTraceBufferPool::instance();
  delete myTraceBuffer;
}

// ============================================================================
/*!
 *  Constructor: need of LocalTraceBufferPool object initialization here,
 *  before creation of thread that access LocalTraceBufferPool: if done after,
 *  simutaneous call to LocalTraceBufferPool::instance() from different
 *  threads may lead to multiple instances (instance method not mutex protected)
 */
// ============================================================================

LocalTraceCollector::LocalTraceCollector()
{
  _threadId=0;
  LocalTraceBufferPool *myTraceBuffer = LocalTraceBufferPool::instance();
}


