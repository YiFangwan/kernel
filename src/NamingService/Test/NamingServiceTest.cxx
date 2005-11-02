
#include "NamingServiceTest.hxx"
#include "Utils_ORB_INIT.hxx"
#include "Utils_SINGLETON.hxx"

#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

using namespace std;

// --- uncomment to have some traces on standard error
//     (useful only when adding new tests...)
//#define _DEVDEBUG_

#ifdef _DEVDEBUG_
#define MYDEVTRACE {std::cerr << __FILE__ << " [" << __LINE__ << "] : ";}
#define DEVTRACE(msg) {MYDEVTRACE; std::cerr<<msg<<std::endl<<std::flush;}
#else
#define MYDEVTRACE
#define DEVTRACE(msg)
#endif

#define TRACEFILE "/tmp/traceUnitTest.log"

// ============================================================================
/*!
 * Set Trace mecanism
 * - delete preexisting trace classes if any
 * - set trace on file
 * Get or initialize the orb
 * Create a SALOME_NamingService instance
 */
// ============================================================================

void 
NamingServiceTest::setUp()
{
  LocalTraceBufferPool* bp1 = LocalTraceBufferPool::instance();
  CPPUNIT_ASSERT(bp1);
  bp1->deleteInstance(bp1);

  // --- trace on file
  char *theFileName = TRACEFILE;

  string s = "file:";
  s += theFileName;
  //s="local";
  //s="with_logger";
  CPPUNIT_ASSERT(! setenv("SALOME_trace",s.c_str(),1)); // 1: overwrite

  ofstream traceFile;
  //  traceFile.open(theFileName, ios::out | ios::trunc);
  traceFile.open(theFileName, ios::out | ios::app);
  CPPUNIT_ASSERT(traceFile); // file created empty, then closed
  traceFile.close();

  bp1 = LocalTraceBufferPool::instance();
  CPPUNIT_ASSERT(bp1);

  // --- Get or initialize the orb

  int _argc = 1;
  char* _argv[] = {""};
  ORB_INIT &init = *SINGLETON_<ORB_INIT>::Instance() ;
  ASSERT(SINGLETON_<ORB_INIT>::IsAlreadyExisting());
  _orb = init(_argc , _argv ) ;

  // --- Create a SALOME_NamingService instance

  _NS.init_orb(_orb) ;
}

// ============================================================================
/*!
 *  - delete trace classes
 */
// ============================================================================

void 
NamingServiceTest::tearDown()
{

  LocalTraceBufferPool* bp1 = LocalTraceBufferPool::instance();
  CPPUNIT_ASSERT(bp1);
  bp1->deleteInstance(bp1);
}

// ============================================================================
/*!
 *
 */
// ============================================================================

void
NamingServiceTest::testConstructorDefault()
{
  SALOME_NamingService  NS;
  //CPPUNIT_ASSERT_THROW(NS.getIORaddr(),CORBA::Exception);
  NS.init_orb(_orb);
  char *root = NS.getIORaddr();
  CORBA::Object_var obj = _orb->string_to_object(root);
  CPPUNIT_ASSERT(!CORBA::is_nil(obj));
  CosNaming::NamingContext_var rootContext =
    CosNaming::NamingContext::_narrow(obj);
  CPPUNIT_ASSERT(!CORBA::is_nil(rootContext));
}

// ============================================================================
/*!
 *
 */
// ============================================================================

void
NamingServiceTest::testConstructorOrb()
{
  SALOME_NamingService  NS(_orb);
  char *root = NS.getIORaddr();
  CORBA::Object_var obj = _orb->string_to_object(root);
  CPPUNIT_ASSERT(!CORBA::is_nil(obj));
  CosNaming::NamingContext_var rootContext =
    CosNaming::NamingContext::_narrow(obj);
  CPPUNIT_ASSERT(!CORBA::is_nil(rootContext));
}



