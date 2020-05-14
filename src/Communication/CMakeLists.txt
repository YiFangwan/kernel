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
  ${MPI_CXX_INCLUDE_PATH}
  ${CMAKE_BINARY_DIR}/salome_adm
  ${CMAKE_BINARY_DIR}/idl
  ${CMAKE_SOURCE_DIR}/src/Basics
  ${CMAKE_SOURCE_DIR}/src/SALOMELocalTrace
  ${CMAKE_SOURCE_DIR}/src/Utils
)

ADD_DEFINITIONS(${OMNIORB_DEFINITIONS} ${MPI_DEFINITIONS})

SET(_link_LIBRARIES
  ${PLATFORM_LIBS}
  ${PTHREAD_LIBRARIES}
  ${OMNIORB_LIBRARIES}
  ${MPI_CXX_LIBRARIES}
  SALOMEBasics
  OpUtil
  SALOMELocalTrace
  SalomeIDLKernel
)

SET(SalomeCommunication_SOURCES
  SALOME_Comm_i.cxx
  SALOME_Matrix_i.cxx
  SenderFactory.cxx
  MultiCommException.cxx
  SALOMEMultiComm.cxx
  ReceiverFactory.cxx
  MatrixClient.cxx
)

ADD_LIBRARY(SalomeCommunication ${SalomeCommunication_SOURCES})
TARGET_LINK_LIBRARIES(SalomeCommunication ${_link_LIBRARIES})
INSTALL(TARGETS SalomeCommunication EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

SET(SalomeCommunication_HEADERS
  MatrixClient.hxx
  MultiCommException.hxx
  Receiver.hxx
  ReceiverFactory.hxx
  Receivers.hxx
  SALOMEMultiComm.hxx
  SALOME_Comm_i.hxx
  SALOME_Communication.hxx
  SALOME_Matrix_i.hxx
  SenderFactory.hxx
)

INSTALL(FILES ${SalomeCommunication_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})

UNSET(SalomeCommunication_SOURCES)
UNSET(SalomeCommunication_HEADERS)
UNSET(_link_LIBRARIES)
