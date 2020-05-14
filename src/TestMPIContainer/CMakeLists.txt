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
  ${CMAKE_SOURCE_DIR}/src/MPIContainer
  ${CMAKE_SOURCE_DIR}/src/GenericObj
)

ADD_DEFINITIONS(${OMNIORB_DEFINITIONS} ${MPI_DEFINITIONS})

SET(_link_LIBRARIES
  ${PLATFORM_LIBS}
  ${PTHREAD_LIBRARIES}
  ${OMNIORB_LIBRARIES}
  ${MPI_CXX_LIBRARIES}
  Registry
  SalomeNotification
  SalomeResourcesManager
  SalomeNS
  OpUtil
  SALOMELocalTrace
  SALOMEBasics
  SalomeContainer
  SalomeMPIContainer
  SalomeGenericObj
  SalomeIDLKernel
)

ADD_LIBRARY(SalomeTestMPIComponentEngine TestMPIComponentEngine.cxx)
TARGET_LINK_LIBRARIES(SalomeTestMPIComponentEngine ${_link_LIBRARIES})
INSTALL(TARGETS SalomeTestMPIComponentEngine DESTINATION ${SALOME_INSTALL_LIBS})

ADD_EXECUTABLE(TestMPIContainer TestMPIContainer.cxx)
TARGET_LINK_LIBRARIES(TestMPIContainer ${_link_LIBRARIES} SalomeTestMPIComponentEngine)
INSTALL(TARGETS TestMPIContainer DESTINATION ${SALOME_INSTALL_BINS})

UNSET(_link_LIBRARIES)
