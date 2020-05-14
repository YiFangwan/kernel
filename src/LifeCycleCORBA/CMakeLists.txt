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
  ${PYTHON_INCLUDE_DIRS}
  ${CMAKE_BINARY_DIR}/salome_adm
  ${CMAKE_BINARY_DIR}/idl
  ${CMAKE_SOURCE_DIR}/src/Basics
  ${CMAKE_SOURCE_DIR}/src/SALOMELocalTrace
  ${CMAKE_SOURCE_DIR}/src/SALOMETraceCollector
  ${CMAKE_SOURCE_DIR}/src/NamingService
  ${CMAKE_SOURCE_DIR}/src/Utils
  ${CMAKE_SOURCE_DIR}/src/GenericObj
  ${CMAKE_SOURCE_DIR}/src/Container
  ${CMAKE_SOURCE_DIR}/src/ResourcesManager
  ${CMAKE_SOURCE_DIR}/src/Batch
  ${CMAKE_SOURCE_DIR}/src/Notification
  ${CMAKE_SOURCE_DIR}/src/Registry
  ${CMAKE_SOURCE_DIR}/src/Launcher
  ${CMAKE_SOURCE_DIR}/src/SALOMESDS
)

ADD_DEFINITIONS(${OMNIORB_DEFINITIONS})

SET(_link_LIBRARIES
  ${PLATFORM_LIBS}
  ${PTHREAD_LIBRARIES}
  ${OMNIORB_LIBRARIES}
  ${PYTHON_LIBRARIES}
  SalomeContainer
  SalomeResourcesManager
  SalomeNS
  SalomeSDS
  OpUtil
  SALOMELocalTrace
  SALOMEBasics
  SalomeGenericObj
  SalomeIDLKernel
  SalomeLauncher
  Registry
)

SET(SalomeLifeCycleCORBA_SOURCES
  SALOME_LifeCycleCORBA.cxx
  SALOME_FileTransferCORBA.cxx
)

SET(SalomeLifeCycleCORBA_HEADERS
  SALOME_LifeCycleCORBA.hxx
  SALOME_FileTransferCORBA.hxx
)

ADD_LIBRARY(SalomeLifeCycleCORBA ${SalomeLifeCycleCORBA_SOURCES})
TARGET_LINK_LIBRARIES(SalomeLifeCycleCORBA ${_link_LIBRARIES})
INSTALL(TARGETS SalomeLifeCycleCORBA EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

INSTALL(FILES ${SalomeLifeCycleCORBA_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})

IF(SALOME_BUILD_TESTS)
  ADD_EXECUTABLE(Test_LifeCycleCORBA Test_LifeCycleCORBA.cxx)
  TARGET_LINK_LIBRARIES(Test_LifeCycleCORBA ${_link_LIBRARIES} SalomeLifeCycleCORBA)

  ADD_EXECUTABLE(TestContainerManager TestContainerManager.cxx)
  TARGET_LINK_LIBRARIES(TestContainerManager SalomeLifeCycleCORBA ${_link_LIBRARIES})

  INSTALL(TARGETS Test_LifeCycleCORBA TestContainerManager DESTINATION ${SALOME_INSTALL_BINS})
ENDIF()

UNSET(SalomeLifeCycleCORBA_SOURCES)
UNSET(SalomeLifeCycleCORBA_HEADERS)
UNSET(_link_LIBRARIES)

IF(SALOME_BUILD_TESTS)
  ADD_SUBDIRECTORY(Test)
ENDIF()
