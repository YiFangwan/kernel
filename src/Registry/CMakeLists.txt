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
  ${OMNIORB_INCLUDE_DIR}
  ${CMAKE_BINARY_DIR}/salome_adm
  ${CMAKE_BINARY_DIR}/idl
  ${CMAKE_SOURCE_DIR}/src/Basics
  ${CMAKE_SOURCE_DIR}/src/SALOMELocalTrace
  ${CMAKE_SOURCE_DIR}/src/NamingService
  ${CMAKE_SOURCE_DIR}/src/Utils
)

ADD_DEFINITIONS(${OMNIORB_DEFINITIONS})

SET(_link_LIBRARIES
  ${PLATFORM_LIBS}
  ${PTHREAD_LIBRARIES}
  ${OMNIORB_LIBRARIES}
  OpUtil
  SALOMELocalTrace
  SalomeIDLKernel 
  SalomeNS 
)

SET(Registry_SOURCES
  RegistryConnexion.cxx
  RegistryService.cxx
)

SET(Registry_HEADERS
  RegistryConnexion.hxx
  RegistryService.hxx
  SALOME_Registry.hxx
)

ADD_LIBRARY(Registry ${Registry_SOURCES})
TARGET_LINK_LIBRARIES(Registry ${_link_LIBRARIES})
INSTALL(TARGETS Registry EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

ADD_EXECUTABLE(SALOME_Registry_Server SALOME_Registry_Server.cxx)
TARGET_LINK_LIBRARIES(SALOME_Registry_Server ${_link_LIBRARIES} Registry)
INSTALL(TARGETS SALOME_Registry_Server EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_BINS})

INSTALL(FILES ${Registry_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})

UNSET(Registry_SOURCES)
UNSET(Registry_HEADERS)
UNSET(_link_LIBRARIES)
