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
  ${CPPUNIT_INCLUDE_DIRS}ti
  ${OMNIORB_INCLUDE_DIR}
  ${CMAKE_BINARY_DIR}/salome_adm
  ${CMAKE_BINARY_DIR}/idl
  ${CMAKE_SOURCE_DIR}/src/Basics
  ${CMAKE_SOURCE_DIR}/src/Basics/Test
  ${CMAKE_SOURCE_DIR}/src/Utils
  ${CMAKE_SOURCE_DIR}/src/Utils/Test
  ${CMAKE_SOURCE_DIR}/src/SALOMELocalTrace
  ${CMAKE_SOURCE_DIR}/src/SALOMELocalTrace/Test
  ${CMAKE_SOURCE_DIR}/src/SALOMETraceCollector
  ${CMAKE_SOURCE_DIR}/src/SALOMETraceCollector/Test
  ${CMAKE_SOURCE_DIR}/src/NamingService
  ${CMAKE_SOURCE_DIR}/src/LifeCycleCORBA
)

SET(_link_LIBRARIES
  ${PLATFORM_LIBS}
  ${PTHREAD_LIBRARIES}
  ${OMNIORB_LIBRARIES}
  ${CPPUNIT_LIBRARIES}
  SalomeNS
  SalomeLifeCycleCORBA
)

ADD_DEFINITIONS(${CPPUNIT_DEFINITIONS} ${OMNIORB_DEFINITIONS})

SET(LOCAL_TEST_DIR ${KERNEL_TEST_DIR}/NamingService)

ADD_LIBRARY(NamingServiceTest NamingServiceTest.cxx)
TARGET_LINK_LIBRARIES(NamingServiceTest ${_link_LIBRARIES})
INSTALL(TARGETS NamingServiceTest DESTINATION ${KERNEL_TEST_LIB})

SET(_exe_LIBRARIES
  ${_link_LIBRARIES}
  NamingServiceTest
  SALOMETraceCollectorTest
  SALOMELocalTraceTest
  UtilsTest
)

ADD_EXECUTABLE(TestNamingService TestNamingService.cxx)
TARGET_LINK_LIBRARIES(TestNamingService ${_exe_LIBRARIES})
INSTALL(TARGETS TestNamingService DESTINATION ${LOCAL_TEST_DIR})

INSTALL(PROGRAMS TestNamingService.py DESTINATION ${LOCAL_TEST_DIR})

INSTALL(FILES CTestTestfileInstall.cmake
        DESTINATION ${LOCAL_TEST_DIR}
        RENAME CTestTestfile.cmake)

UNSET(LOCAL_TEST_DIR)
UNSET(_link_LIBRARIES)
UNSET(_exe_LIBRARIES)
