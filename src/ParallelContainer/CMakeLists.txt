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
  ${MPI_CXX_INCLUDE_PATH}
  ${PYTHON_INCLUDE_DIRS}
  ${PACO_INCLUDE_DIRS}
  ${CMAKE_BINARY_DIR}/salome_adm
  ${CMAKE_BINARY_DIR}/idl
  ${CMAKE_SOURCE_DIR}/src/Container
  ${CMAKE_SOURCE_DIR}/src/Notification
  ${CMAKE_SOURCE_DIR}/src/SALOMELocalTrace
  ${CMAKE_SOURCE_DIR}/src/Basics
  ${CMAKE_SOURCE_DIR}/src/NamingService
  ${CMAKE_SOURCE_DIR}/src/Registry
  ${CMAKE_SOURCE_DIR}/src/Utils
  ${CMAKE_SOURCE_DIR}/src/GenericObj
  ${CMAKE_SOURCE_DIR}/src/HDFPersist
  ${CMAKE_SOURCE_DIR}/src/SALOMETraceCollector
)

ADD_DEFINITIONS(${OMNIORB_DEFINITIONS} ${MPI_DEFINITIONS})

SET(_link_LIBRARIES
  ${PLATFORM_LIBS}
  ${PTHREAD_LIBRARIES}
  ${OMNIORB_LIBRARIES}
  ${PACO_LIBRARIES}
  ${PYTHON_LIBRARIES}
  ${MPI_CXX_LIBRARIES}
  SalomeContainer
  SalomeNS
  SALOMELocalTrace
  SALOMEBasics
  SalomeHDFPersist
  SalomeParallelIDLKernel
  SalomeIDLKernel
)

SET(SalomeParallelContainer_SOURCES
  SALOME_ParallelComponent_i.cxx
  SALOME_ParallelContainer_i.cxx
  SALOME_ParallelContainerProxy_i.cxx
  Parallel_Salome_file_i.cxx
  SALOME_ParallelGlobalProcessVar_i.cxx
)

ADD_LIBRARY(SalomeParallelContainer ${SalomeParallelContainer_SOURCES})
TARGET_LINK_LIBRARIES(SalomeParallelContainer ${_link_LIBRARIES})
INSTALL(TARGETS SalomeParallelContainer EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

IF(SALOME_USE_MPI)
  ADD_EXECUTABLE(SALOME_ParallelContainerProxyMpi SALOME_ParallelContainerProxyMpi.cxx)
  TARGET_LINK_LIBRARIES(SALOME_ParallelContainerProxyMpi ${_link_LIBRARIES} SalomeParallelContainer)

  ADD_EXECUTABLE(SALOME_ParallelContainerNodeMpi SALOME_ParallelContainerNodeMpi.cxx)
  TARGET_LINK_LIBRARIES(SALOME_ParallelContainerNodeMpi ${_link_LIBRARIES} SalomeParallelContainer)

  INSTALL(TARGETS SALOME_ParallelContainerProxyMpi SALOME_ParallelContainerNodeMpi
          EXPORT ${PROJECT_NAME}TargetGroup 
          DESTINATION ${SALOME_INSTALL_BINS})
ENDIF(SALOME_USE_MPI)

ADD_EXECUTABLE(SALOME_ParallelContainerProxyDummy SALOME_ParallelContainerProxyDummy.cxx)
TARGET_LINK_LIBRARIES(SALOME_ParallelContainerProxyDummy ${_link_LIBRARIES} SalomeParallelContainer)

ADD_EXECUTABLE(SALOME_ParallelContainerNodeDummy SALOME_ParallelContainerNodeDummy.cxx)
TARGET_LINK_LIBRARIES(SALOME_ParallelContainerNodeDummy ${_link_LIBRARIES} SalomeParallelContainer)

INSTALL(TARGETS SALOME_ParallelContainerNodeDummy SALOME_ParallelContainerProxyDummy DESTINATION ${SALOME_INSTALL_BINS})

SET(SalomeParallelContainer_HEADERS
  Parallel_Salome_file_i.hxx
  SALOME_ParallelComponent_i.hxx
  SALOME_ParallelContainer_i.hxx
  SALOME_ParallelContainerProxy_i.hxx
  SALOME_ParallelGlobalProcessVar_i.hxx
)

INSTALL(FILES ${SalomeParallelContainer_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})

UNSET(SalomeParallelContainer_SOURCES)
UNSET(SalomeParallelContainer_HEADERS)
UNSET(_link_LIBRARIES)
