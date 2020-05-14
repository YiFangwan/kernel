# Copyright (C) 2012-2020  CEA/DEN, EDF R&D, OPEN CASCADE
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

INCLUDE_DIRECTORIES(
  ${PTHREAD_INCLUDE_DIR}
  ${CPPUNIT_INCLUDE_DIRS}
  ${CMAKE_SOURCE_DIR}/src/Basics
  ${CMAKE_SOURCE_DIR}/src/Basics/Test
  ${CMAKE_SOURCE_DIR}/src/SALOMELocalTrace
  ${CMAKE_SOURCE_DIR}/src/SALOMELocalTrace/Test
  ${CMAKE_SOURCE_DIR}/src/Utils
)

ADD_DEFINITIONS(${CPPUNIT_DEFINITIONS})

SET(_link_LIBRARIES
  ${PLATFORM_LIBS}
  ${PTHREAD_LIBRARIES}
  ${CPPUNIT_LIBRARIES}
  OpUtil
)

SET(_exe_LIBRARIES
  ${_link_LIBRARIES}
  SALOMELocalTraceTest
)

SET(LOCAL_TEST_DIR ${KERNEL_TEST_DIR}/Utils)

ADD_LIBRARY(UtilsTest UtilsTest.cxx)
TARGET_LINK_LIBRARIES(UtilsTest ${_link_LIBRARIES})
INSTALL(TARGETS UtilsTest DESTINATION ${KERNEL_TEST_LIB})

ADD_EXECUTABLE(TestUtils TestUtils.cxx)
TARGET_LINK_LIBRARIES(TestUtils ${_exe_LIBRARIES} UtilsTest)
INSTALL(TARGETS TestUtils DESTINATION ${LOCAL_TEST_DIR})

INSTALL(FILES TestUtils.py DESTINATION ${LOCAL_TEST_DIR})

INSTALL(FILES CTestTestfileInstall.cmake
        DESTINATION ${LOCAL_TEST_DIR}
        RENAME CTestTestfile.cmake)

UNSET(_link_LIBRARIES)
UNSET(_exe_LIBRARIES)
UNSET(LOCAL_TEST_DIR)
