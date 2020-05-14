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
  ${PTHREAD_INCLUDE_DIR}
  ${PYTHON_INCLUDE_DIRS}
  ${OMNIORB_INCLUDE_DIR}
  ${MPI_CXX_INCLUDE_PATH}
  ${CMAKE_BINARY_DIR}/salome_adm
  ${CMAKE_BINARY_DIR}/idl
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/src/Communication
  ${CMAKE_SOURCE_DIR}/src/Basics
  ${CMAKE_SOURCE_DIR}/src/SALOMELocalTrace
  ${CMAKE_SOURCE_DIR}/src/Utils
)

ADD_DEFINITIONS(${OMNIORB_DEFINITIONS} ${MPI_DEFINITIONS})

SET(_link_LIBRARIES
  ${PLATFORM_LIBS}
  ${PTHREAD_LIBRARIES}
  ${PYTHON_LIBRARIES}
  ${MPI_CXX_LIBRARIES}
  ${OMNIORB_LIBRARIES}
  SalomeCommunication
  SalomeIDLKernel
)

SET_SOURCE_FILES_PROPERTIES(libSALOME_Comm.i PROPERTIES CPLUSPLUS ON)
SET_SOURCE_FILES_PROPERTIES(libSALOME_Comm.i PROPERTIES SWIG_FLAGS "-py3")

IF(${CMAKE_VERSION} VERSION_LESS "3.8.0") 
  SWIG_ADD_MODULE(libSALOME_Comm python libSALOME_Comm.i)
ELSE()
  SWIG_ADD_LIBRARY(libSALOME_Comm LANGUAGE python SOURCES libSALOME_Comm.i)
ENDIF()
SWIG_LINK_LIBRARIES(libSALOME_Comm ${_link_LIBRARIES})

IF(WIN32)
  SET_TARGET_PROPERTIES(_libSALOME_Comm PROPERTIES DEBUG_OUTPUT_NAME _libSALOME_Comm_d)
ENDIF()

INSTALL(TARGETS _libSALOME_Comm DESTINATION ${SALOME_INSTALL_PYTHON})

INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/libSALOME_Comm.py DESTINATION ${SALOME_INSTALL_PYTHON})

UNSET(_link_LIBRARIES)
