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
  ${LIBXML2_INCLUDE_DIR}
  ${CMAKE_BINARY_DIR}/salome_adm
  ${CMAKE_BINARY_DIR}/idl
  ${CMAKE_SOURCE_DIR}/src/Basics
  ${CMAKE_SOURCE_DIR}/src/SALOMELocalTrace
  ${CMAKE_SOURCE_DIR}/src/NamingService
  ${CMAKE_SOURCE_DIR}/src/Utils
)

ADD_DEFINITIONS(${OMNIORB_DEFINITIONS} ${LIBXML2_DEFINITIONS})

SET(ResourcesManager_SOURCES
  SALOME_ResourcesCatalog_Parser.cxx
  SALOME_ResourcesCatalog_Handler.cxx
  SALOME_LoadRateManager.cxx
  ResourcesManager.cxx
)

SET(ResourcesManager_LIBRARIES
  ${PLATFORM_LIBS}
  ${PTHREAD_LIBRARIES}
  ${LIBXML2_LIBRARIES}
  SALOMEBasics
)

ADD_LIBRARY(ResourcesManager ${ResourcesManager_SOURCES})
TARGET_LINK_LIBRARIES(ResourcesManager ${ResourcesManager_LIBRARIES})
INSTALL(TARGETS ResourcesManager
        EXPORT ${PROJECT_NAME}TargetGroup
        DESTINATION ${SALOME_INSTALL_LIBS})

SET(SalomeResourcesManager_SOURCES
  SALOME_ResourcesManager.cxx
  SALOME_ResourcesManager_Client.cxx
  SALOME_ResourcesManager_Common.cxx
)

SET(SalomeResourcesManager_LIBRARIES
  ${PLATFORM_LIBS}
  ${PTHREAD_LIBRARIES}
  ${LIBXML2_LIBRARIES}
  ${OMNIORB_LIBRARIES}
  SALOMEBasics
  SalomeNS
  OpUtil
  SalomeIDLKernel
  ResourcesManager
)

ADD_LIBRARY(SalomeResourcesManager ${SalomeResourcesManager_SOURCES})
TARGET_LINK_LIBRARIES(SalomeResourcesManager ${SalomeResourcesManager_LIBRARIES})
INSTALL(TARGETS SalomeResourcesManager
        EXPORT ${PROJECT_NAME}TargetGroup
        DESTINATION ${SALOME_INSTALL_LIBS})

SET(ResourcesManager_HEADERS
  ResourcesManager.hxx
  ResourcesManager_Defs.hxx
  SALOME_LoadRateManager.hxx
  SALOME_ResourcesCatalog_Parser.hxx
  SALOME_ResourcesManager.hxx
  SALOME_ResourcesManager_Client.hxx
  SALOME_ResourcesManager_Common.hxx
)

INSTALL(FILES ${ResourcesManager_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})

UNSET(ResourcesManager_SOURCES)
UNSET(ResourcesManager_LIBRARIES)
UNSET(SalomeResourcesManager_SOURCES)
UNSET(SalomeResourcesManager_LIBRARIES)
UNSET(ResourcesManager_HEADERS)
