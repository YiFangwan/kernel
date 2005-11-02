
#ifndef _NAMINGSERVICETEST_HXX_
#define _NAMINGSERVICETEST_HXX_

#include <cppunit/extensions/HelperMacros.h>
#include "SALOME_NamingService.hxx"

class NamingServiceTest : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE( NamingServiceTest );
  CPPUNIT_TEST( testConstructorDefault );
  CPPUNIT_TEST( testConstructorOrb );
//   CPPUNIT_TEST(  );
  CPPUNIT_TEST_SUITE_END();

public:

  void setUp();
  void tearDown();

  void testConstructorDefault();
  void testConstructorOrb();
//   void testFindOrLoad_Component_();

protected:
  CORBA::ORB_var _orb;
  SALOME_NamingService _NS;
};

#endif
