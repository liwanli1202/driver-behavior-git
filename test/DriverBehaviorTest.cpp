
#include <iostream>
#include <assert.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/TestResult.h>
#include <cppunit/XmlOutputter.h>

using namespace std;
using namespace CppUnit;

int main(int argc, char* argv[])
{
    // Get the top level suite from the registry
    CppUnit::Test *suite = CppUnit::TestFactoryRegistry::getRegistry().makeTest();
    assert(suite);

    // Informs test-listener about test results
    TestResult testresult;

    // Register listener for collecting the test-results
	TestResultCollector collectedresults;
	testresult.addListener(&collectedresults);

    // register listener for per-test progress output
	BriefTestProgressListener progress;
    testresult.addListener(&progress);    // Adds the test to the list of test to run

    // Insert test-suite at test-runner by registry
    TestRunner testrunner;
    testrunner.addTest(TestFactoryRegistry::getRegistry().makeTest());
    testrunner.run(testresult);

    // Output results in compiler-format
    CompilerOutputter compilerOutputter(&collectedresults, std::cerr);
    compilerOutputter.write();

//    CppUnit::TextUi::TestRunner runner;
//    runner.addTest(suite);

//    // Change the default outputter to a compiler error format outputter
//    runner.setOutputter( new CppUnit::CompilerOutputter( &runner.result(),
//                                                         std::cerr ) );

    // Output XML for Jenkins CPPunit plugin
    ofstream xmlFileOut("cppunit-reports/cppunitDriverBehaviorTestResults.xml");
    XmlOutputter xmlOut(&collectedresults, xmlFileOut);
    xmlOut.write();

    // Return error code 1 if the one of test failed.
    return collectedresults.wasSuccessful() ? 0 : 1;
}
