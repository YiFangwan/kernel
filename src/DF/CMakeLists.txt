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

ADD_DEFINITIONS(${BOOST_DEFINITIONS})

SET(DF_SOURCES
  DF_Attribute.cxx
  DF_Label.cxx
  DF_Document.cxx
  DF_Application.cxx
  DF_ChildIterator.cxx
  DF_Container.cxx
)

SET(DF_HEADERS
  DF_Application.hxx
  DF_Attribute.hxx
  DF_ChildIterator.hxx
  DF_Container.hxx
  DF_Document.hxx
  DF_Label.hxx
  DF_definitions.hxx
)

ADD_LIBRARY(DF ${DF_SOURCES})
INSTALL(TARGETS DF EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

INSTALL(FILES ${DF_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})

IF(SALOME_BUILD_TESTS)
  INCLUDE_DIRECTORIES(
    ${CPPUNIT_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/src/Basics/Test
  )
  SET(LOCAL_TEST_DIR ${KERNEL_TEST_DIR}/DF)

  ADD_EXECUTABLE(TestDF testDF.cxx)
  TARGET_LINK_LIBRARIES(TestDF ${CPPUNIT_LIBRARIES} DF SALOMEBasics)
  INSTALL(TARGETS TestDF DESTINATION ${LOCAL_TEST_DIR})

  INSTALL(FILES TestDF.py DESTINATION ${LOCAL_TEST_DIR})

  INSTALL(FILES CTestTestfileInstall.cmake
          DESTINATION ${LOCAL_TEST_DIR}
          RENAME CTestTestfile.cmake)
ENDIF()

UNSET(DF_SOURCES)
UNSET(DF_HEADERS)
UNSET(LOCAL_TEST_DIR)
