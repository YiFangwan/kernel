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
  ${CPPUNIT_INCLUDE_DIRS}
  ${PTHREAD_INCLUDE_DIR}
  ${CMAKE_BINARY_DIR}/salome_adm
  ${CMAKE_SOURCE_DIR}/src/Basics
  ${CMAKE_SOURCE_DIR}/src/Basics/Test
  ${CMAKE_SOURCE_DIR}/src/SALOMELocalTrace
  ${CMAKE_SOURCE_DIR}/src/SALOMELocalTrace/Test
)

ADD_DEFINITIONS(${CPPUNIT_DEFINITIONS})

SET(_link_LIBRARIES
  ${PLATFORM_LIBS}
  ${PTHREAD_LIBRARIES}
  ${CPPUNIT_LIBRARIES}
  SALOMEBasics
  SALOMELocalTrace
  SALOMELocalTraceTest
)

IF(NOT SALOME_LIGHT_ONLY)
  INCLUDE_DIRECTORIES(
    ${OMNIORB_INCLUDE_DIR}
    ${CMAKE_BINARY_DIR}/idl
    ${CMAKE_SOURCE_DIR}/src/SALOMETraceCollector
    ${CMAKE_SOURCE_DIR}/src/SALOMETraceCollector/Test
    ${CMAKE_SOURCE_DIR}/src/NamingService
    ${CMAKE_SOURCE_DIR}/src/NamingService/Test
    ${CMAKE_SOURCE_DIR}/src/Utils
    ${CMAKE_SOURCE_DIR}/src/Utils/Test
    ${CMAKE_SOURCE_DIR}/src/LifeCycleCORBA
    ${CMAKE_SOURCE_DIR}/src/LifeCycleCORBA/Test
    ${CMAKE_SOURCE_DIR}/src/SALOMDESImpl
    ${CMAKE_SOURCE_DIR}/src/SALOMEDSImpl/Test
    ${CMAKE_SOURCE_DIR}/src/SALOMDES
    ${CMAKE_SOURCE_DIR}/src/SALOMEDS/Test
    ${CMAKE_SOURCE_DIR}/src/Registry
    ${CMAKE_SOURCE_DIR}/src/Notification
    ${CMAKE_SOURCE_DIR}/src/ResourcesManager
  )
  ADD_DEFINITIONS(-DWITH_CORBA ${OMNIORB_DEFINITIONS})

  LIST(APPEND _link_LIBRARIES
    ${OMNIORB_LIBRARIES}
    Registry
    SalomeNotification
    SalomeResourcesManager
    SalomeNS
    NamingServiceTest
    SalomeContainer
    SALOMETraceCollectorTest
    OpUtil
    UtilsTest
    SalomeLifeCycleCORBA
    LifeCycleCORBATest
    SalomeDSImpl
    SALOMEDSImplTest
    SalomeDS
    SALOMEDSTest
    SalomeIDLKernel
  )
ENDIF()

SET(LOCAL_TEST_DIR ${KERNEL_TEST_DIR}/UnitTests)

ADD_EXECUTABLE(UnitTests UnitTests.cxx)
TARGET_LINK_LIBRARIES(UnitTests ${_link_LIBRARIES})
INSTALL(TARGETS UnitTests DESTINATION ${LOCAL_TEST_DIR})

INSTALL(FILES CTestTestfileInstall.cmake
        DESTINATION ${LOCAL_TEST_DIR}
        RENAME CTestTestfile.cmake)

INSTALL(PROGRAMS UnitTests.py DESTINATION ${LOCAL_TEST_DIR})

UNSET(_link_LIBRARIES)
