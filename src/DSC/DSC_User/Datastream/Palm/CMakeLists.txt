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
  ${CMAKE_SOURCE_DIR}/src/DSC/DSC_User
  ${CMAKE_SOURCE_DIR}/src/DSC/DSC_User/Datastream
  ${CMAKE_SOURCE_DIR}/src/DSC/DSC_Basic
  ${CMAKE_SOURCE_DIR}/src/Container
  ${CMAKE_SOURCE_DIR}/src/GenericObj
  ${CMAKE_SOURCE_DIR}/src/Notification
  ${CMAKE_SOURCE_DIR}/src/SALOMELocalTrace
  ${CMAKE_SOURCE_DIR}/src/Basics
  ${CMAKE_SOURCE_DIR}/src/Utils
)

ADD_DEFINITIONS(${OMNIORB_DEFINITIONS})

SET(_link_LIBRARIES
  ${PLATFORM_LIBS}
  ${PTHREAD_LIBRARIES}
  ${OMNIORB_LIBRARIES}
)

ADD_LIBRARY(SalomePalm palm_port_factory.cxx)
TARGET_LINK_LIBRARIES(SalomePalm ${_link_LIBRARIES} SalomeDSCSuperv)

INSTALL(TARGETS SalomePalm EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

IF(SALOME_BUILD_TESTS)
  ADD_EXECUTABLE(test_DataIdContainer_Palm test_DataIdContainer.cxx)
  TARGET_LINK_LIBRARIES(test_DataIdContainer_Palm ${_link_LIBRARIES})
ENDIF()

SET(SalomePalm_HEADERS
  PalmCouplingPolicy.hxx
  palm_data_seq_short_port_provides.hxx
  palm_data_short_port_provides.hxx
  palm_port_factory.hxx
)

INSTALL(FILES ${SalomePalm_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})

UNSET(SalomePalm_HEADERS)
UNSET(_link_LIBRARIES)
