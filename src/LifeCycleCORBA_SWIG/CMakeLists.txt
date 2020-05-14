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

INCLUDE(${SWIG_USE_FILE})

INCLUDE_DIRECTORIES(
  ${PYTHON_INCLUDE_DIRS}
  ${PTHREAD_INCLUDE_DIR}
  ${OMNIORB_INCLUDE_DIR}
  ${CMAKE_BINARY_DIR}/salome_adm
  ${CMAKE_BINARY_DIR}/idl
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_SOURCE_DIR}/src/NamingService
  ${CMAKE_SOURCE_DIR}/src/LifeCycleCORBA
  ${CMAKE_SOURCE_DIR}/src/Basics
  ${CMAKE_SOURCE_DIR}/src/SALOMELocalTrace
  ${CMAKE_SOURCE_DIR}/src/Utils
)

ADD_DEFINITIONS(${OMNIORB_DEFINITIONS})

SET(_link_LIBRARIES
  ${PLATFORM_LIBS}
  ${PTHREAD_LIBRARIES}
  ${PYTHON_LIBRARIES}
  SalomeLifeCycleCORBA
)

SET_SOURCE_FILES_PROPERTIES(libSALOME_LifeCycleCORBA.i PROPERTIES CPLUSPLUS ON)
SET_SOURCE_FILES_PROPERTIES(libSALOME_LifeCycleCORBA.i PROPERTIES SWIG_FLAGS "-py3")

IF(${CMAKE_VERSION} VERSION_LESS "3.8.0") 
  SWIG_ADD_MODULE(libSALOME_LifeCycleCORBA python libSALOME_LifeCycleCORBA.i)  
ELSE()
  SWIG_ADD_LIBRARY(libSALOME_LifeCycleCORBA LANGUAGE python SOURCES libSALOME_LifeCycleCORBA.i)
ENDIF()

SWIG_LINK_LIBRARIES(libSALOME_LifeCycleCORBA ${_link_LIBRARIES})

IF(WIN32)
  SET_TARGET_PROPERTIES(_libSALOME_LifeCycleCORBA PROPERTIES DEBUG_OUTPUT_NAME _libSALOME_LifeCycleCORBA_d)
ENDIF()

INSTALL(TARGETS _libSALOME_LifeCycleCORBA DESTINATION ${SALOME_INSTALL_PYTHON})

SALOME_INSTALL_SCRIPTS(LifeCycleCORBA.py ${SALOME_INSTALL_SCRIPT_PYTHON})
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/libSALOME_LifeCycleCORBA.py DESTINATION ${SALOME_INSTALL_PYTHON})

IF(SALOME_BUILD_TESTS)
  SALOME_INSTALL_SCRIPTS(TestLifeCycleCORBA.py ${SALOME_INSTALL_SCRIPT_PYTHON})
ENDIF()

UNSET(_link_LIBRARIES)

IF(SALOME_BUILD_TESTS)
  ADD_SUBDIRECTORY(Test)
ENDIF()
