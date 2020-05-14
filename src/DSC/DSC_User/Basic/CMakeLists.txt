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
  ${CMAKE_SOURCE_DIR}/src/DSC/DSC_Basic
  ${CMAKE_SOURCE_DIR}/src/DSC/DSC_User
  ${CMAKE_SOURCE_DIR}/src/Container
  ${CMAKE_SOURCE_DIR}/src/GenericObj
  ${CMAKE_SOURCE_DIR}/src/Notification
  ${CMAKE_SOURCE_DIR}/src/SALOMELocalTrace
  ${CMAKE_SOURCE_DIR}/src/Basics
  ${CMAKE_SOURCE_DIR}/src/Utils)

ADD_DEFINITIONS(${OMNIORB_DEFINITIONS})

SET(_link_LIBRARIES
  ${PLATFORM_LIBS}
  ${PTHREAD_LIBRARIES}
  ${OMNIORB_LIBRARIES}
  SalomeDSCSuperv
)

SET(SalomeDSCSupervBasic_SOURCES
  basic_port_factory.cxx
  data_short_port_uses.cxx
  data_short_port_provides.cxx
)

ADD_LIBRARY(SalomeDSCSupervBasic ${SalomeDSCSupervBasic_SOURCES})
TARGET_LINK_LIBRARIES(SalomeDSCSupervBasic ${_link_LIBRARIES})
INSTALL(TARGETS SalomeDSCSupervBasic EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

SET(SalomeDSCSupervBasic_HEADERS
  basic_port_factory.hxx
  data_short_port_provides.hxx
  data_short_port_uses.hxx
)

INSTALL(FILES ${SalomeDSCSupervBasic_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})

UNSET(SalomeDSCSupervBasic_SOURCES)
SET(SalomeDSCSupervBasic_HEADERS)
UNSET(_link_LIBRARIES)
