
#include "NamingServiceTest.hxx"
#include "Utils_ORB_INIT.hxx"
#include "Utils_SINGLETON.hxx"

#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <cstdio>

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
 *
 */
// ============================================================================

NSTEST_echo_i::NSTEST_echo_i()
{
  _num=-1;
}

NSTEST_echo_i::NSTEST_echo_i(CORBA::Long num)
{
  _num=num;
}

NSTEST_echo_i::~NSTEST_echo_i()
{
}

CORBA::Long NSTEST_echo_i::getId()
{
  return _num;
}

NSTEST_aFactory_i::NSTEST_aFactory_i()
{
  _num=0;
}

NSTEST_aFactory_i::~NSTEST_aFactory_i()
{
}

NSTEST::echo_ptr NSTEST_aFactory_i::createInstance()
{
  NSTEST_echo_i * anEcho = new NSTEST_echo_i(_num);
  _num++;
  NSTEST::echo_var anEchoRef = anEcho->_this();
  return anEchoRef._retn();
}

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

  // --- Create an NSTEST::factory

  CORBA::Object_var obj = _orb->resolve_initial_references("RootPOA");
  ASSERT(!CORBA::is_nil(obj));
  _root_poa = PortableServer::POA::_narrow(obj);
  _pman = _root_poa->the_POAManager();
  _myFactory  = new NSTEST_aFactory_i();
  _myFactoryId = _root_poa->activate_object(_myFactory);
  _factoryRef = _myFactory->_this();
  _pman->activate();
  
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
 *  Test default constructor: must be followed by a call to init_orb(ORB)
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
 *  Test constructor with ORB parameter
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

// ============================================================================
/*!
 * Test Register and resolve of a single CORBA object with absolute pathname,
 * without subdirectories
 */
// ============================================================================

void
NamingServiceTest::testRegisterResolveAbsNoPath()
{
  _NS.Register(_factoryRef,"/nstest_factory");
  CORBA::Object_var obj = _NS.Resolve("/nstest_factory");
  CPPUNIT_ASSERT(!CORBA::is_nil(obj));
  NSTEST::aFactory_var myFactory = NSTEST::aFactory::_narrow(obj);
  CPPUNIT_ASSERT(!CORBA::is_nil(myFactory));
}

// ============================================================================
/*!
 * Test Register and resolve of a single CORBA object with absolute pathname,
 * in a subdirectory
 */
// ============================================================================

void
NamingServiceTest::testRegisterResolveAbsWithPath()
{
  CORBA::Object_var obj = _NS.Resolve("/nstest_factory");
  CPPUNIT_ASSERT(!CORBA::is_nil(obj));
  NSTEST::aFactory_var myFactory = NSTEST::aFactory::_narrow(obj);
  CPPUNIT_ASSERT(!CORBA::is_nil(myFactory));
  NSTEST::echo_var anEchoRef = myFactory->createInstance();
  _NS.Register(anEchoRef,"/nstest/echo_0");
  obj = _NS.Resolve("/nstest/echo_0");
  CPPUNIT_ASSERT(!CORBA::is_nil(obj));
  NSTEST::echo_var anEchoRef2 = NSTEST::echo::_narrow(obj);
  CPPUNIT_ASSERT(!CORBA::is_nil(anEchoRef2));
}

// ============================================================================
/*!
 * Test resolve with a name not known
 */
// ============================================================================

void
NamingServiceTest::testResolveBadName()
{
  CORBA::Object_var obj = _NS.Resolve("/notRegisteredName");
  CPPUNIT_ASSERT(CORBA::is_nil(obj));
  obj = _NS.Resolve("/nstest/notRegisteredName");
  CPPUNIT_ASSERT(CORBA::is_nil(obj));
  obj = _NS.Resolve("/unknownPath/notRegisteredName");
  CPPUNIT_ASSERT(CORBA::is_nil(obj));
  obj = _NS.Resolve("/anUnknown/ComplicatedPath/notRegisteredName");
  CPPUNIT_ASSERT(CORBA::is_nil(obj));
}

// ============================================================================
/*!
 * Test register and resolve multiple objects, test resolveFirst
 */
// ============================================================================

#define NB_OBJS 10

void
NamingServiceTest::testResolveFirst()
{
  CORBA::Object_var obj = _NS.Resolve("/nstest_factory");
  CPPUNIT_ASSERT(!CORBA::is_nil(obj));
  NSTEST::aFactory_var myFactory = NSTEST::aFactory::_narrow(obj);
  CPPUNIT_ASSERT(!CORBA::is_nil(myFactory));
  int ref[NB_OBJS];
    for (int i=0; i<NB_OBJS; i++)
      {
	NSTEST::echo_var anEchoRef = myFactory->createInstance();
	ref[i] = anEchoRef->getId();
	string name = "/nstestfirst/echo_";
	char anum[10];
	sprintf(anum,"%d",ref[i]);
	name += anum;
	_NS.Register(anEchoRef,name.c_str());
      }
    for (int i=0; i<NB_OBJS; i++)
      {
	string name = "/nstestfirst/echo_";
	char anum[10];
	sprintf(anum,"%d",ref[i]);
	name += anum;
	CORBA::Object_var obj = _NS.Resolve(name.c_str());
	CPPUNIT_ASSERT(!CORBA::is_nil(obj));
	NSTEST::echo_var anEchoRef = NSTEST::echo::_narrow(obj);
	CPPUNIT_ASSERT(!CORBA::is_nil(anEchoRef));
	CPPUNIT_ASSERT(anEchoRef->getId() == ref[i]);
      }
    string name = "/nstestfirst/echo";
    obj = _NS.ResolveFirst(name.c_str());
    CPPUNIT_ASSERT(!CORBA::is_nil(obj));
    NSTEST::echo_var anEchoRef = NSTEST::echo::_narrow(obj);
    CPPUNIT_ASSERT(!CORBA::is_nil(anEchoRef));
    CPPUNIT_ASSERT(anEchoRef->getId() == ref[0]);
}
