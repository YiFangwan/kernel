
#ifndef _NAMINGSERVICETEST_HXX_
#define _NAMINGSERVICETEST_HXX_

#include <cppunit/extensions/HelperMacros.h>
#include "SALOME_NamingService.hxx"

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(nstest)

class NSTEST_echo_i : public virtual POA_NSTEST::echo,
		      public virtual PortableServer::RefCountServantBase
{
public:
  NSTEST_echo_i();
  NSTEST_echo_i(CORBA::Long num);
  ~NSTEST_echo_i();
  CORBA::Long getId();
private:
  int _num;
};

class NSTEST_aFactory_i : public virtual POA_NSTEST::aFactory,
			  public virtual PortableServer::RefCountServantBase
{
public:
  NSTEST_aFactory_i();
  ~NSTEST_aFactory_i();
  NSTEST::echo_ptr createInstance();
private:
  int _num;
};

class NamingServiceTest : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE( NamingServiceTest );
  CPPUNIT_TEST( testConstructorDefault );
  CPPUNIT_TEST( testConstructorOrb );
  CPPUNIT_TEST( testRegisterResolveAbsNoPath );
  CPPUNIT_TEST( testRegisterResolveAbsWithPath );
  CPPUNIT_TEST( testResolveBadName );
  CPPUNIT_TEST( testResolveFirst );
  CPPUNIT_TEST_SUITE_END();

public:

  void setUp();
  void tearDown();

  void testConstructorDefault();
  void testConstructorOrb();
  void testRegisterResolveAbsNoPath();
  void testRegisterResolveAbsWithPath();
  void testResolveBadName();
  void testResolveFirst();

protected:
  CORBA::ORB_var _orb;
  SALOME_NamingService _NS;

  PortableServer::POA_var _root_poa;
  PortableServer::POAManager_var _pman;
  PortableServer::ObjectId_var _myFactoryId;
  NSTEST_aFactory_i * _myFactory;
  CORBA::Object_var _factoryRef;
};

#endif
