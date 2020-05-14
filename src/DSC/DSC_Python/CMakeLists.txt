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
  ${OMNIORB_INCLUDE_DIR}
  ${PYTHON_INCLUDE_DIRS}
  ${NUMPY_INCLUDE_DIR}
  ${PTHREAD_INCLUDE_DIR}
  ${Boost_INCLUDE_DIRS}
  ${CMAKE_BINARY_DIR}/salome_adm
  ${CMAKE_BINARY_DIR}/idl
  ${CMAKE_SOURCE_DIR}/src/DSC/DSC_User
  ${CMAKE_SOURCE_DIR}/src/DSC/DSC_User/Datastream
  ${CMAKE_SOURCE_DIR}/src/DSC/DSC_User/Datastream/Palm
  ${CMAKE_SOURCE_DIR}/src/DSC/DSC_User/Datastream/Calcium
  ${CMAKE_BINARY_DIR}/src/DSC/DSC_User/Datastream/Calcium
  ${CMAKE_SOURCE_DIR}/src/DSC/DSC_User/Basic
  ${CMAKE_SOURCE_DIR}/src/DSC/DSC_Basic
  ${CMAKE_SOURCE_DIR}/src/SALOMELocalTrace
  ${CMAKE_SOURCE_DIR}/src/Basics
  ${CMAKE_SOURCE_DIR}/src/Utils
  ${CMAKE_SOURCE_DIR}/src/Container
  ${CMAKE_SOURCE_DIR}/src/GenericObj
  ${CMAKE_SOURCE_DIR}/src/Notification
)

ADD_DEFINITIONS(${OMNIORB_DEFINITIONS} ${BOOST_DEFINITIONS} ${NUMPY_DEFINITIONS})

SET(_link_LIBRARIES
  ${PLATFORM_LIBS}
  ${PTHREAD_LIBRARIES}
  ${PYTHON_LIBRARIES}
  ${OMNIORB_LIBRARIES}
  CalciumC
  SalomeDatastream
  SalomeDSCSupervBasic
  SalomeDSCSuperv
  SalomeDSCContainer
  SalomeContainer
)

SET_SOURCE_FILES_PROPERTIES(calcium.i PROPERTIES CPLUSPLUS ON)
SET_SOURCE_FILES_PROPERTIES(calcium.i PROPERTIES SWIG_FLAGS "-py3")
SET(CMAKE_SWIG_FLAGS "-noexcept")

IF(${CMAKE_VERSION} VERSION_LESS "3.8.0") 
  SWIG_ADD_MODULE(calcium python calcium.i)
ELSE()
  SWIG_ADD_LIBRARY(calcium LANGUAGE python SOURCES calcium.i)
ENDIF()
SWIG_LINK_LIBRARIES(calcium ${_link_LIBRARIES})

IF(WIN32)
  SET_TARGET_PROPERTIES(_calcium PROPERTIES DEBUG_OUTPUT_NAME _calcium_d)
ENDIF()

INSTALL(TARGETS _calcium DESTINATION ${SALOME_INSTALL_PYTHON})

SET(SCRIPTS
  dsccalcium.py
  ${CMAKE_CURRENT_BINARY_DIR}/calcium.py
)

SALOME_INSTALL_SCRIPTS("${SCRIPTS}" ${SALOME_INSTALL_PYTHON} EXTRA_DPYS "${SWIG_MODULE_calcium_REAL_NAME}")

UNSET(SCRIPTS)
UNSET(_link_LIBRARIES)
