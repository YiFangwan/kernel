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
  ${OMNIORB_INCLUDE_DIR}
  ${PTHREAD_INCLUDE_DIR}
  ${CMAKE_BINARY_DIR}/salome_adm
  ${CMAKE_BINARY_DIR}/idl
  ${CMAKE_SOURCE_DIR}/src/Basics
  ${CMAKE_SOURCE_DIR}/src/SALOMELocalTrace
  ${CMAKE_SOURCE_DIR}/src/Utils
)

ADD_DEFINITIONS(${OMNIORB_DEFINITIONS})

SET(_link_LIBRARIES
  ${PLATFORM_LIBS}
  ${PTHREAD_LIBRARIES}
  ${OMNIORB_LIBRARIES}
  SALOMEBasics
  OpUtil
  SalomeIDLKernel
)

SET(SalomeNS_SOURCES
  NamingService_WaitForServerReadiness.cxx
  SALOME_NamingService.cxx
  ServiceUnreachable.cxx
)

SET(SalomeNS_HEADERS
  NamingService_WaitForServerReadiness.hxx
  SALOME_NamingService_defs.hxx
  SALOME_NamingService.hxx
  ServiceUnreachable.hxx
)

ADD_LIBRARY(SalomeNS ${SalomeNS_SOURCES})
TARGET_LINK_LIBRARIES(SalomeNS ${_link_LIBRARIES})
INSTALL(TARGETS SalomeNS EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

SALOME_INSTALL_SCRIPTS(SALOME_NamingServicePy.py ${SALOME_INSTALL_SCRIPT_PYTHON})

INSTALL(FILES ${SalomeNS_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})

UNSET(SalomeNS_SOURCES)
UNSET(SalomeNS_HEADERS)
UNSET(_link_LIBRARIES)

IF(SALOME_BUILD_TESTS)
  ADD_SUBDIRECTORY(Test)
ENDIF()
