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
  ${HDF5_INCLUDE_DIRS}
  ${MPI_INCLUDE_DIRS}
  ${PYTHON_INCLUDE_DIRS}
  ${CMAKE_BINARY_DIR}/salome_adm
  ${CMAKE_BINARY_DIR}/idl
  ${CMAKE_SOURCE_DIR}/src/Basics
  ${CMAKE_SOURCE_DIR}/src/SALOMELocalTrace
  ${CMAKE_SOURCE_DIR}/src/NamingService
  ${CMAKE_SOURCE_DIR}/src/Utils
  ${CMAKE_SOURCE_DIR}/src/Registry
  ${CMAKE_SOURCE_DIR}/src/Notification
  ${CMAKE_SOURCE_DIR}/src/ResourcesManager
  ${CMAKE_SOURCE_DIR}/src/HDFPersist
  ${CMAKE_SOURCE_DIR}/src/GenericObj
  ${CMAKE_SOURCE_DIR}/src/SALOMETraceCollector/Test
)

ADD_DEFINITIONS(${HDF5_DEFINITIONS} ${MPI_DEFINITIONS} ${OMNIORB_DEFINITIONS})

SET(_link_LIBRARIES
  ${PLATFORM_LIBS}
  ${PTHREAD_LIBRARIES}
  ${HDF5_LIBRARIES}
  ${MPI_LIBRARIES}
  ${OMNIORB_LIBRARIES}
  ${PYTHON_LIBRARIES}
  Registry
  SalomeNotification
  SalomeResourcesManager
  ResourcesManager
  SalomeNS
  OpUtil
  SALOMELocalTrace
  SALOMEBasics
  SalomeHDFPersist
  SalomeGenericObj
  SalomeIDLKernel
)

IF(SALOME_PACO_PARALLEL)
  INCLUDE_DIRECTORIES(${PACO_INCLUDE_DIRS})
  ADD_DEFINITIONS(-DWITH_PACO_PARALLEL)
  LIST(APPEND _link_LIBRARIES ${PACO_LIBRARIES})
ENDIF()

SET(SalomeContainer_SOURCES
  Component_i.cxx
  Container_i.cxx
  SALOME_FileTransfer_i.cxx
  SALOME_FileRef_i.cxx
  Container_init_python.cxx
  SALOME_ContainerManager.cxx
  Salome_file_i.cxx
  SALOME_CPythonHelper.cxx
)

ADD_LIBRARY(SalomeContainer ${SalomeContainer_SOURCES})
TARGET_LINK_LIBRARIES(SalomeContainer ${_link_LIBRARIES})
INSTALL(TARGETS SalomeContainer EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

SET(Container_SOURCES
  SALOME_Container.cxx
  SALOME_Container_SignalsHandler.cxx
)

ADD_EXECUTABLE(SALOME_Container ${Container_SOURCES})
TARGET_LINK_LIBRARIES(SALOME_Container ${_link_LIBRARIES} SalomeContainer)
INSTALL(TARGETS SALOME_Container EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_BINS})

IF(SALOME_BUILD_TESTS)
  ADD_EXECUTABLE(TestSalome_file TestSalome_file.cxx)
  TARGET_LINK_LIBRARIES(TestSalome_file ${_link_LIBRARIES} SalomeContainer)
ENDIF()

SET(Container_SCRIPTS
  SALOME_ComponentPy.py
  SALOME_PyNode.py
  SALOME_Container.py
)

SALOME_INSTALL_SCRIPTS("${Container_SCRIPTS}" ${SALOME_INSTALL_SCRIPT_PYTHON})

SET(Container_HEADERS
  Container_init_python.hxx
  SALOME_Component_i.hxx
  SALOME_Container.hxx
  SALOME_Container_i.hxx
  SALOME_ContainerManager.hxx
  SALOME_CPythonHelper.hxx
  Salome_file_i.hxx
  SALOME_FileRef_i.hxx
  SALOME_FileTransfer_i.hxx
)

INSTALL(FILES ${Container_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})

UNSET(SalomeContainer_SOURCES)
UNSET(Container_SOURCES)
UNSET(Container_SCRIPTS)
UNSET(Container_HEADERS)
UNSET(_link_LIBRARIES)
