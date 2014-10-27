# Copyright (C) 2012-2014  CEA/DEN, EDF R&D, OPEN CASCADE
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
# 

INCLUDE_DIRECTORIES(
  ${OMNIORB_INCLUDE_DIR}
  ${PTHREAD_INCLUDE_DIR}
  ${PROJECT_BINARY_DIR}/idl
  ${PROJECT_BINARY_DIR}/salome_adm
  ${CMAKE_CURRENT_SOURCE_DIR}/../Utils
  ${CMAKE_CURRENT_SOURCE_DIR}/../NamingService
)

ADD_DEFINITIONS(${OMNIORB_DEFINITIONS})

SET(COMMON_LIBS SalomeNS SalomeIDLKernel ${OMNIORB_LIBRARIES})

SET(SalomeSDS_SOURCES
  SALOMESDS_Exception.cxx
  SALOMESDS_RefCountServ.cxx
  SALOMESDS_AnyDataServer.cxx
  SALOMESDS_BasicDataServer.cxx
  SALOMESDS_DataScopeServer.cxx
  SALOMESDS_DataServerManager.cxx
  SALOMESDS_StringDataServer.cxx
  )

ADD_LIBRARY(SalomeSDS ${SalomeSDS_SOURCES})
TARGET_LINK_LIBRARIES(SalomeSDS ${COMMON_LIBS} ${PTHREAD_LIBRARIES})
INSTALL(TARGETS SalomeSDS EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

FILE(GLOB COMMON_HEADERS_HXX "${CMAKE_CURRENT_SOURCE_DIR}/*.hxx")

INSTALL(FILES ${COMMON_HEADERS_HXX} DESTINATION ${SALOME_INSTALL_HEADERS})
