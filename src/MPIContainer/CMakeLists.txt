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
  ${MPI_CXX_INCLUDE_PATH}
  ${PTHREAD_INCLUDE_DIR}
  ${PYTHON_INCLUDE_DIRS}
  ${OMNIORB_INCLUDE_DIR}
  ${CMAKE_BINARY_DIR}/salome_adm
  ${CMAKE_BINARY_DIR}/idl
  ${CMAKE_SOURCE_DIR}/src/Basics
  ${CMAKE_SOURCE_DIR}/src/SALOMELocalTrace
  ${CMAKE_SOURCE_DIR}/src/NamingService
  ${CMAKE_SOURCE_DIR}/src/Utils
  ${CMAKE_SOURCE_DIR}/src/Registry
  ${CMAKE_SOURCE_DIR}/src/Notification
  ${CMAKE_SOURCE_DIR}/src/ResourcesManager
  ${CMAKE_SOURCE_DIR}/src/Container
  ${CMAKE_SOURCE_DIR}/src/GenericObj
)

SET(_link_LIBRARIES
  ${PLATFORM_LIBS}
  ${PTHREAD_LIBRARIES}
  ${OMNIORB_LIBRARIES}
  ${MPI_CXX_LIBRARIES}
  ${PYTHON_LIBRARIES}
  Registry
  SalomeNotification
  SalomeResourcesManager
  SalomeNS
  OpUtil
  SALOMELocalTrace
  SALOMEBasics
  SalomeContainer
  SalomeGenericObj
  SalomeIDLKernel
)

ADD_DEFINITIONS(${MPI_DEFINITIONS} ${OMNIORB_DEFINITIONS})

ADD_LIBRARY(SalomeMPIContainer MPIObject_i.cxx MPIContainer_i.cxx)
TARGET_LINK_LIBRARIES(SalomeMPIContainer ${_link_LIBRARIES})
INSTALL(TARGETS SalomeMPIContainer EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

ADD_EXECUTABLE(SALOME_MPIContainer SALOME_MPIContainer.cxx)
TARGET_LINK_LIBRARIES(SALOME_MPIContainer ${_link_LIBRARIES} SalomeMPIContainer)
INSTALL(TARGETS SALOME_MPIContainer EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_BINS})

ADD_EXECUTABLE(getMPIImplementation getMPIImplementation.cxx)
TARGET_LINK_LIBRARIES(getMPIImplementation ${MPI_CXX_LIBRARIES})
INSTALL(TARGETS getMPIImplementation EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_BINS})

IF(SALOME_BUILD_TESTS)
  ADD_EXECUTABLE(testMPI2 testMPI2.cxx)
  TARGET_LINK_LIBRARIES(testMPI2 ${MPI_CXX_LIBRARIES})
  INSTALL(TARGETS testMPI2 DESTINATION ${SALOME_INSTALL_BINS})
  INSTALL(FILES launch_testMPI2.csh
          PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
          DESTINATION ${SALOME_INSTALL_SCRIPT_SCRIPTS})
ENDIF()

INSTALL(FILES MPIContainer_i.hxx MPIObject_i.hxx DESTINATION ${SALOME_INSTALL_HEADERS})

UNSET(_link_LIBRARIES)
