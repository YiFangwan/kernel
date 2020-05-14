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
  ${CMAKE_SOURCE_DIR}/src/NamingService
  ${CMAKE_SOURCE_DIR}/src/SALOMELocalTrace
  ${CMAKE_SOURCE_DIR}/src/Basics
  ${CMAKE_SOURCE_DIR}/src/Utils
  ${CMAKE_SOURCE_DIR}/src/LifeCycleCORBA
  ${CMAKE_SOURCE_DIR}/src/Container
  ${CMAKE_SOURCE_DIR}/src/Notification
  ${CMAKE_SOURCE_DIR}/src/GenericObj
)

ADD_DEFINITIONS(${OMNIORB_DEFINITIONS})

SET(SalomeKernelHelpers_LIBS
  ${PLATFORM_LIBS}
  ${PTHREAD_LIBRARIES}
  ${OMNIORB_LIBRARIES}
  SalomeNS
  SALOMELocalTrace
  SALOMEBasics
  OpUtil
  SalomeLifeCycleCORBA
  SalomeContainer
  SalomeNotification
  SalomeGenericObj
  SalomeIDLKernel
)

SET(SalomeKernelHelpers_SOURCES
  SALOME_KernelServices.cxx
  SALOME_StudyEditor.cxx
  SALOMEDS_DriverDefaultImpl.cxx
)

SET(SalomeKernelHelpers_HEADERS
  KernelHelpers.hxx
  SALOMEDS_DriverDefaultImpl.hxx
  SALOME_KernelServices.hxx
  SALOME_StudyEditor.hxx
)

ADD_LIBRARY(SalomeKernelHelpers ${SalomeKernelHelpers_SOURCES})
TARGET_LINK_LIBRARIES(SalomeKernelHelpers ${SalomeKernelHelpers_LIBS})
INSTALL(TARGETS SalomeKernelHelpers EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

INSTALL(FILES ${SalomeKernelHelpers_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})

IF(SALOME_BUILD_TESTS)
  ADD_EXECUTABLE(KernelHelpersUseCases KernelHelpersUseCases.cxx)
  TARGET_LINK_LIBRARIES(KernelHelpersUseCases ${SalomeKernelHelpers_LIBS} SalomeKernelHelpers)
  INSTALL(TARGETS KernelHelpersUseCases DESTINATION ${SALOME_INSTALL_BINS})
ENDIF()

UNSET(SalomeKernelHelpers_SOURCES)
UNSET(SalomeKernelHelpers_HEADERS)

IF(SALOME_BUILD_TESTS)
  ADD_SUBDIRECTORY(Test)
ENDIF()
