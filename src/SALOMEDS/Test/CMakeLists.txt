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
  ${Boost_INCLUDE_DIR}
  ${PTHREAD_INCLUDE_DIR}
  ${HDF5_INCLUDE_DIRS}
  ${CMAKE_BINARY_DIR}/salome_adm
  ${CMAKE_BINARY_DIR}/idl
  ${CMAKE_SOURCE_DIR}/src/Basics
  ${CMAKE_SOURCE_DIR}/src/SALOMELocalTrace
  ${CMAKE_SOURCE_DIR}/src/SALOMETraceCollector
  ${CMAKE_SOURCE_DIR}/src/NamingService
  ${CMAKE_SOURCE_DIR}/src/Utils
  ${CMAKE_SOURCE_DIR}/src/ResourcesManager
  ${CMAKE_SOURCE_DIR}/src/LifeCycleCORBA
  ${CMAKE_SOURCE_DIR}/src/SALOMEDS
  ${CMAKE_SOURCE_DIR}/src/SALOMEDSClient
  ${CMAKE_SOURCE_DIR}/src/DF
  ${CMAKE_SOURCE_DIR}/src/SALOMEDSImpl
  ${CMAKE_SOURCE_DIR}/src/GenericObj
)

ADD_DEFINITIONS(${CPPUNIT_DEFINITIONS} ${OMNIORB_DEFINITIONS} ${HDF5_DEFINITIONS} ${BOOST_DEFINITIONS})
IF(WIN32)
  ADD_DEFINITIONS(-DNOGDI)
ENDIF(WIN32)

SET(_link_LIBRARIES
  ${PLATFORM_LIBS}
  ${PTHREAD_LIBRARIES}
  ${HDF5_C_LIBRARIES}
  ${OMNIORB_LIBRARIES}
  ${CPPUNIT_LIBRARIES}
  SALOMEBasics
  SalomeResourcesManager
  SalomeContainer
  SalomeNS
  Registry
  SalomeNotification
  OpUtil
  SALOMELocalTrace
  DF
  SalomeDSImpl
  SalomeDSClient
  SalomeDS
  SalomeGenericObj
  SalomeIDLKernel
)

SET(LOCAL_TEST_DIR ${KERNEL_TEST_DIR}/SALOMEDS)

ADD_LIBRARY(SALOMEDSTest SALOMEDSTest.cxx)
TARGET_LINK_LIBRARIES(SALOMEDSTest ${_link_LIBRARIES})
INSTALL(TARGETS SALOMEDSTest DESTINATION ${KERNEL_TEST_LIB})

ADD_EXECUTABLE(TestSALOMEDS TestSALOMEDS.cxx)
TARGET_LINK_LIBRARIES(TestSALOMEDS ${_link_LIBRARIES} SALOMEDSTest)
INSTALL(TARGETS TestSALOMEDS DESTINATION ${LOCAL_TEST_DIR})

INSTALL(FILES TestSALOMEDS.py DESTINATION ${LOCAL_TEST_DIR})

INSTALL(FILES CTestTestfileInstall.cmake
        DESTINATION ${LOCAL_TEST_DIR}
        RENAME CTestTestfile.cmake)

UNSET(LOCAL_TEST_DIR)
UNSET(_link_LIBRARIES)
