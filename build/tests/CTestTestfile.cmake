# CMake generated Testfile for 
# Source directory: /Users/ayadav/workspace/github/vnelogging/tests
# Build directory: /Users/ayadav/workspace/github/vnelogging/build/tests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(vnelogging.test "/Users/ayadav/workspace/github/vnelogging/build/bin/TestVneLogging")
set_tests_properties(vnelogging.test PROPERTIES  _BACKTRACE_TRIPLES "/Users/ayadav/workspace/github/vnelogging/tests/CMakeLists.txt;123;add_test;/Users/ayadav/workspace/github/vnelogging/tests/CMakeLists.txt;0;")
subdirs("../external/googletest")
