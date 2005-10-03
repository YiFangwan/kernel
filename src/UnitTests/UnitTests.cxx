
#include <cppunit/CompilerOutputter.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TextTestProgressListener.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
//#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/TestRunner.h>
#include <stdexcept>

#include "SALOMELocalTraceTest.hxx"
#include "SALOMETraceCollectorTest.hxx"

#include "UtilsTest.hxx"

int main(int argc, char* argv[])
{
  // Create the event manager and test controller
  CPPUNIT_NS::TestResult controller;

  // Add a listener that colllects test result
  CPPUNIT_NS::TestResultCollector result;
  controller.addListener( &result );        

  // Add a listener that print dots as test run.
#ifdef WIN32
  CPPUNIT_NS::TextTestProgressListener progress;
#else
  CPPUNIT_NS::BriefTestProgressListener progress;
#endif
  controller.addListener( &progress );      

  // Get the top level suite from the registry
  //CppUnit::Test *suite =CppUnit::TestFactoryRegistry::getRegistry().makeTest();
  CPPUNIT_NS::Test *suite =
    CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest();

  // Adds the test to the list of test to run
  //CppUnit::TextUi::TestRunner runner;
  CPPUNIT_NS::TestRunner runner;
  runner.addTest( suite );
  runner.run( controller);
  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, std::cerr );
  outputter.write(); 
  
  // Change the default outputter to a compiler error format outputter
  //   runner.setOutputter( new CppUnit::CompilerOutputter( &runner.result(),
  //                                                        std::cerr ) );

  // Run the tests.
  //bool wasSucessful = runner.run();
  bool wasSucessful = result.wasSuccessful();

  // Return error code 1 if the one of test failed.
  return wasSucessful ? 0 : 1;
}
