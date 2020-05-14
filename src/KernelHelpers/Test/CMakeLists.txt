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
  ${OMNIORB_INCLUDE_DIR}
  ${PTHREAD_INCLUDE_DIR}
  ${CMAKE_BINARY_DIR}/salome_adm
  ${CMAKE_BINARY_DIR}/idl
  ${CMAKE_SOURCE_DIR}/src/Basics/Test
  ${CMAKE_SOURCE_DIR}/src/NamingService
  ${CMAKE_SOURCE_DIR}/src/SALOMELocalTrace
  ${CMAKE_SOURCE_DIR}/src/Basics
  ${CMAKE_SOURCE_DIR}/src/Utils
  ${CMAKE_SOURCE_DIR}/src/LifeCycleCORBA
  ${CMAKE_SOURCE_DIR}/src/Container
  ${CMAKE_SOURCE_DIR}/src/Notification
  ${CMAKE_SOURCE_DIR}/src/GenericObj
  ${CMAKE_SOURCE_DIR}/src/KernelHelpers
)

ADD_DEFINITIONS(${CPPUNIT_DEFINITIONS} ${OMNIORB_DEFINITIONS})

SET(SalomeKernelHelpersTest_LIBS
  ${PLATFORM_LIBS}
  ${PTHREAD_LIBRARIES}
  ${OMNIORB_LIBRARIES}
  ${CPPUNIT_LIBRARIES}
  SalomeKernelHelpers
  SalomeNS
  SALOMELocalTrace
  SALOMEBasics
  OpUtil
  SalomeLifeCycleCORBA
  SalomeContainer
  SalomeNotification
  SalomeGenericObj
  SalomeIDLKernel
)

SET(LOCAL_TEST_DIR ${KERNEL_TEST_DIR}/KernelHelpers)

ADD_LIBRARY(SalomeKernelHelpersTest KernelHelpersUnitTests.cxx)
TARGET_LINK_LIBRARIES(SalomeKernelHelpersTest ${SalomeKernelHelpersTest_LIBS})
INSTALL(TARGETS SalomeKernelHelpersTest DESTINATION ${KERNEL_TEST_LIB})

ADD_EXECUTABLE(TestKernelHelpers TestKernelHelpers.cxx)
TARGET_LINK_LIBRARIES(TestKernelHelpers ${SalomeKernelHelpersTest_LIBS} SalomeKernelHelpersTest)
INSTALL(TARGETS TestKernelHelpers DESTINATION ${LOCAL_TEST_DIR})

INSTALL(FILES TestKernelHelpers.py DESTINATION ${LOCAL_TEST_DIR})

INSTALL(FILES CTestTestfileInstall.cmake
        DESTINATION ${LOCAL_TEST_DIR}
        RENAME CTestTestfile.cmake)

SET(LOCAL_TEST_DIR)
UNSET(SalomeKernelHelpersTest_LIBS)
