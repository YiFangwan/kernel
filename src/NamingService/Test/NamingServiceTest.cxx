
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
 * a basic CORBA object implementation for use with namingService tests
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

// ============================================================================
/*!
 * a factory of CORBA objects for use with namingService tests
 */
// ============================================================================

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
 * Test Register and resolve of a single CORBA object with relative pathname,
 * without subdirectories
 */
// ============================================================================

void
NamingServiceTest::testRegisterResolveRelativeNoPath()
{
  CORBA::Object_var obj = _NS.Resolve("/nstest_factory");
  CPPUNIT_ASSERT(!CORBA::is_nil(obj));
  NSTEST::aFactory_var myFactory = NSTEST::aFactory::_narrow(obj);
  CPPUNIT_ASSERT(!CORBA::is_nil(myFactory));
  _NS.Create_Directory("/myContext");
  _NS.Change_Directory("/myContext");
  NSTEST::echo_var anEchoRef = myFactory->createInstance();
  _NS.Register(anEchoRef,"echo_0");
  CORBA::Object_var obj2 = _NS.Resolve("echo_0");
  CPPUNIT_ASSERT(!CORBA::is_nil(obj2));
  NSTEST::echo_var anEchoRef2 = NSTEST::echo::_narrow(obj2);
  CPPUNIT_ASSERT(!CORBA::is_nil(anEchoRef2));
  CORBA::Object_var obj3 = _NS.Resolve("/myContext/echo_0");
  CPPUNIT_ASSERT(!CORBA::is_nil(obj3));
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
  CORBA::Object_var obj2 = _NS.Resolve("/nstest/echo_0");
  CPPUNIT_ASSERT(!CORBA::is_nil(obj2));
  NSTEST::echo_var anEchoRef2 = NSTEST::echo::_narrow(obj2);
  CPPUNIT_ASSERT(!CORBA::is_nil(anEchoRef2));
}

// ============================================================================
/*!
 * Test Register and resolve of a single CORBA object with relative pathname,
 * in a subdirectory.
 * Relative Path is changed to the created subdirectory when Register()
 */
// ============================================================================

void
NamingServiceTest::testRegisterResolveRelativeWithPath()
{
  CORBA::Object_var obj = _NS.Resolve("/nstest_factory");
  CPPUNIT_ASSERT(!CORBA::is_nil(obj));
  NSTEST::aFactory_var myFactory = NSTEST::aFactory::_narrow(obj);
  CPPUNIT_ASSERT(!CORBA::is_nil(myFactory));
  _NS.Create_Directory("/myContext");
  _NS.Change_Directory("/myContext");
  NSTEST::echo_var anEchoRef = myFactory->createInstance();
  _NS.Register(anEchoRef,"subdir/echo_0");
  //CORBA::Object_var obj2 = _NS.Resolve("subdir/echo_0");
  CORBA::Object_var obj2 = _NS.Resolve("echo_0");
  CPPUNIT_ASSERT(!CORBA::is_nil(obj2));
  NSTEST::echo_var anEchoRef2 = NSTEST::echo::_narrow(obj2);
  CPPUNIT_ASSERT(!CORBA::is_nil(anEchoRef2));
  CORBA::Object_var obj3 = _NS.Resolve("/myContext/subdir/echo_0");
  CPPUNIT_ASSERT(!CORBA::is_nil(obj3));
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
  CORBA::Object_var obj2 = _NS.Resolve("/nstest/notRegisteredName");
  CPPUNIT_ASSERT(CORBA::is_nil(obj2));
  CORBA::Object_var obj3 = _NS.Resolve("/unknownPath/notRegisteredName");
  CPPUNIT_ASSERT(CORBA::is_nil(obj3));
  CORBA::Object_var obj4 =
    _NS.Resolve("/anUnknown/ComplicatedPath/notRegisteredName");
  CPPUNIT_ASSERT(CORBA::is_nil(obj4));
}

// ============================================================================
/*!
 * Test resolve with a name not known, with a relative path
 */
// ============================================================================

void
NamingServiceTest::testResolveBadNameRelative()
{
  _NS.Create_Directory("/myContext");
  _NS.Change_Directory("/myContext");
  CORBA::Object_var obj = _NS.Resolve("notRegisteredName");
  CPPUNIT_ASSERT(CORBA::is_nil(obj));
  CORBA::Object_var obj2 = _NS.Resolve("unknownPath/notRegisteredName");
  CPPUNIT_ASSERT(CORBA::is_nil(obj2));
  CORBA::Object_var obj3 =
    _NS.Resolve("anUnknown/ComplicatedPath/notRegisteredName");
  CPPUNIT_ASSERT(CORBA::is_nil(obj3));
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
	CORBA::Object_var obj2 = _NS.Resolve(name.c_str());
	CPPUNIT_ASSERT(!CORBA::is_nil(obj2));
	NSTEST::echo_var anEchoRef = NSTEST::echo::_narrow(obj2);
	CPPUNIT_ASSERT(!CORBA::is_nil(anEchoRef));
	CPPUNIT_ASSERT(anEchoRef->getId() == ref[i]);
      }
    string name = "/nstestfirst/echo";
    CORBA::Object_var obj3 = _NS.ResolveFirst(name.c_str());
    CPPUNIT_ASSERT(!CORBA::is_nil(obj3));
    NSTEST::echo_var anEchoRef2 = NSTEST::echo::_narrow(obj3);
    CPPUNIT_ASSERT(!CORBA::is_nil(anEchoRef2));
    CPPUNIT_ASSERT(anEchoRef2->getId() == ref[0]);
}

// ============================================================================
/*!
 * Test register and resolve multiple objects, test resolveFirst, relative path
 * Result not guaranteed
 */
// ============================================================================

void
NamingServiceTest::testResolveFirstRelative()
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
	string name = "/nstestfirstrel/echo_";
	char anum[10];
	sprintf(anum,"%d",ref[i]);
	name += anum;
	_NS.Register(anEchoRef,name.c_str());
      }
    for (int i=0; i<NB_OBJS; i++)
      {
	_NS.Change_Directory("/nstestfirstrel");
	string name = "echo_";
	char anum[10];
	sprintf(anum,"%d",ref[i]);
	name += anum;
	CORBA::Object_var obj2 = _NS.Resolve(name.c_str());
	CPPUNIT_ASSERT(!CORBA::is_nil(obj2));
	NSTEST::echo_var anEchoRef = NSTEST::echo::_narrow(obj2);
	CPPUNIT_ASSERT(!CORBA::is_nil(anEchoRef));
	CPPUNIT_ASSERT(anEchoRef->getId() == ref[i]);
      }
    _NS.Change_Directory("/nstestfirstrel");
    string name = "echo";
    CORBA::Object_var obj3 = _NS.ResolveFirst(name.c_str());
    CPPUNIT_ASSERT(CORBA::is_nil(obj3));
}

// ============================================================================
/*!
 * Test resolveFirst with unknown name
 */
// ============================================================================

void
NamingServiceTest::testResolveFirstUnknown()
{
  string name = "/notYeyRegistered";
  CORBA::Object_var obj2 = _NS.ResolveFirst(name.c_str());
  CPPUNIT_ASSERT(CORBA::is_nil(obj2));
  name = "/nstestfirst/notYeyRegistered";
  CORBA::Object_var obj3 = _NS.ResolveFirst(name.c_str());
  CPPUNIT_ASSERT(CORBA::is_nil(obj3));
  name = "/rrr/sss/ttt/notYeyRegistered";
  CORBA::Object_var obj4 = _NS.ResolveFirst(name.c_str());
  CPPUNIT_ASSERT(CORBA::is_nil(obj4));
}

// ============================================================================
/*!
 * Test resolveFirst with unknown name, relative Path
 */
// ============================================================================

void
NamingServiceTest::testResolveFirstUnknownRelative()
{
  _NS.Create_Directory("/myContext");
  _NS.Change_Directory("/myContext");
  string name = "RelnotYeyRegistered";
  CORBA::Object_var obj2 = _NS.ResolveFirst(name.c_str());
  CPPUNIT_ASSERT(CORBA::is_nil(obj2));
  name = "Relnstestfirst/notYeyRegistered";
  CORBA::Object_var obj3 = _NS.ResolveFirst(name.c_str());
  CPPUNIT_ASSERT(CORBA::is_nil(obj3));
  name = "Relrrr/sss/ttt/notYeyRegistered";
  CORBA::Object_var obj4 = _NS.ResolveFirst(name.c_str());
  CPPUNIT_ASSERT(CORBA::is_nil(obj4));
}

// ============================================================================
/*!
 * Test 
 */
// ============================================================================

void
NamingServiceTest::testResolveComponentOK()
{
  CPPUNIT_ASSERT(0);
}

// ============================================================================
/*!
 * Test 
 */
// ============================================================================

void
NamingServiceTest::testResolveComponentEmptyHostname()
{
  CPPUNIT_ASSERT(0);
}

// ============================================================================
/*!
 * Test 
 */
// ============================================================================

void
NamingServiceTest::testResolveComponentUnknownHostname()
{
  CPPUNIT_ASSERT(0);
}

// ============================================================================
/*!
 * Test 
 */
// ============================================================================

void
NamingServiceTest::testResolveComponentEmptyContainerName()
{
  CPPUNIT_ASSERT(0);
}

// ============================================================================
/*!
 * Test 
 */
// ============================================================================

void
NamingServiceTest::testResolveComponentUnknownContainerName()
{
  CPPUNIT_ASSERT(0);
}

// ============================================================================
/*!
 * Test 
 */
// ============================================================================

void
NamingServiceTest::testResolveComponentEmptyComponentName()
{
  CPPUNIT_ASSERT(0);
}

// ============================================================================
/*!
 * Test 
 */
// ============================================================================

void
NamingServiceTest::testResolveComponentUnknownComponentName()
{
  CPPUNIT_ASSERT(0);
}

// ============================================================================
/*!
 * Test 
 */
// ============================================================================

void
NamingServiceTest::testResolveComponentFalseNbproc()
{
  CPPUNIT_ASSERT(0);
}

// ============================================================================
/*!
 * Test 
 */
// ============================================================================

void
NamingServiceTest::testContainerName()
{
  CPPUNIT_ASSERT(0);
}

// ============================================================================
/*!
 * Test 
 */
// ============================================================================

void
NamingServiceTest::testContainerNameParams()
{
  CPPUNIT_ASSERT(0);
}

// ============================================================================
/*!
 * Test 
 */
// ============================================================================

void
NamingServiceTest::testBuildContainerNameForNS()
{
  CPPUNIT_ASSERT(0);
}

// ============================================================================
/*!
 * Test 
 */
// ============================================================================

void
NamingServiceTest::testBuildContainerNameForNSParams()
{
  CPPUNIT_ASSERT(0);
}

// ============================================================================
/*!
 * Test 
 */
// ============================================================================

void
NamingServiceTest::testFind()
{
  CPPUNIT_ASSERT(0);
}

// ============================================================================
/*!
 * Test 
 */
// ============================================================================

void
NamingServiceTest::testCreateDirectory()
{
  CPPUNIT_ASSERT(0);
}

// ============================================================================
/*!
 * Test 
 */
// ============================================================================

void
NamingServiceTest::testChangeDirectory()
{
  CPPUNIT_ASSERT(0);
}

// ============================================================================
/*!
 * Test 
 */
// ============================================================================

void
NamingServiceTest::testCurrentDirectory()
{
  CPPUNIT_ASSERT(0);
}

// ============================================================================
/*!
 * Test 
 */
// ============================================================================

void
NamingServiceTest::testList()
{
  CPPUNIT_ASSERT(0);
}

// ============================================================================
/*!
 * Test 
 */
// ============================================================================

void
NamingServiceTest::testListDirectory()
{
  CPPUNIT_ASSERT(0);
}

// ============================================================================
/*!
 * Test 
 */
// ============================================================================

void
NamingServiceTest::testListDirectoryRecurs()
{
  CPPUNIT_ASSERT(0);
}

// ============================================================================
/*!
 * Test 
 */
// ============================================================================

void
NamingServiceTest::testDestroyName()
{
  CPPUNIT_ASSERT(0);
}

// ============================================================================
/*!
 * Test 
 */
// ============================================================================

void
NamingServiceTest::testDestroyDirectory()
{
  CPPUNIT_ASSERT(0);
}

// ============================================================================
/*!
 * Test 
 */
// ============================================================================

void
NamingServiceTest::testDestroyFullDirectory()
{
  CPPUNIT_ASSERT(0);
}

// ============================================================================
/*!
 * Test 
 */
// ============================================================================

void
NamingServiceTest::testGetIorAddr()
{
  CPPUNIT_ASSERT(0);
}

// ============================================================================
/*!
 * Test 
 */
// ============================================================================

// void
// NamingServiceTest::()
// {
//   CPPUNIT_ASSERT(0);
// }

