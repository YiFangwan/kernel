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
  ${Boost_INCLUDE_DIR}
  ${HDF5_INCLUDE_DIRS}
  ${PTHREAD_INCLUDE_DIR}
  ${CMAKE_SOURCE_DIR}/src/Basics
  ${CMAKE_SOURCE_DIR}/src/Basics/Test
  ${CMAKE_SOURCE_DIR}/src/DF
  ${CMAKE_SOURCE_DIR}/src/SALOMEDSImpl
  ${CMAKE_BINARY_DIR}/idl
)

ADD_DEFINITIONS(${CPPUNIT_DEFINITIONS} ${BOOST_DEFINITIONS})

SET(_link_LIBRARIES
  ${PLATFORM_LIBS}
  ${PTHREAD_LIBRARIES}
  ${HDF5_C_LIBRARIES}
  ${CPPUNIT_LIBRARIES}
  SALOMEBasics
  SALOMELocalTrace
  SalomeHDFPersist
  DF
  SalomeDSImpl
)  

SET(LOCAL_TEST_DIR ${KERNEL_TEST_DIR}/SALOMEDSImpl)

ADD_LIBRARY(SALOMEDSImplTest SALOMEDSImplTest.cxx)
TARGET_LINK_LIBRARIES(SALOMEDSImplTest ${_link_LIBRARIES})
INSTALL(TARGETS SALOMEDSImplTest DESTINATION ${KERNEL_TEST_LIB})

ADD_EXECUTABLE(TestSALOMEDSImpl TestSALOMEDSImpl.cxx)
TARGET_LINK_LIBRARIES(TestSALOMEDSImpl ${_link_LIBRARIES} SALOMEDSImplTest)
INSTALL(TARGETS TestSALOMEDSImpl DESTINATION ${LOCAL_TEST_DIR})

INSTALL(FILES TestSALOMEDSImpl.py DESTINATION ${LOCAL_TEST_DIR})

INSTALL(FILES CTestTestfileInstall.cmake
        DESTINATION ${LOCAL_TEST_DIR}
        RENAME CTestTestfile.cmake)

UNSET(_link_LIBRARIES)
UNSET(LOCAL_TEST_DIR)
