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
  ${LIBXML2_INCLUDE_DIR}
  ${CMAKE_BINARY_DIR}/salome_adm
  ${CMAKE_BINARY_DIR}/idl
  ${CMAKE_SOURCE_DIR}/src/Basics
  ${CMAKE_SOURCE_DIR}/src/SALOMELocalTrace
  ${CMAKE_SOURCE_DIR}/src/NamingService
  ${CMAKE_SOURCE_DIR}/src/Utils
)

ADD_DEFINITIONS(${LIBXML2_DEFINITIONS} ${OMNIORB_DEFINITIONS})

SET(_link_LIBRARIES
  ${PLATFORM_LIBS}
  ${PTHREAD_LIBRARIES}
  ${LIBXML2_LIBRARIES}
  ${OMNIORB_LIBRARIES}
  SalomeNS
  OpUtil
  SALOMELocalTrace
  SALOMEBasics
  SalomeIDLKernel
)

SET(SalomeCatalog_SOURCES
  SALOME_ModuleCatalog_Handler.cxx
  SALOME_ModuleCatalog_Parser_IO.cxx
  SALOME_ModuleCatalog_impl.cxx
  SALOME_ModuleCatalog_Acomponent_impl.cxx
)

SET(SalomeCatalog_HEADERS
  SALOME_ModuleCatalog.hxx
  SALOME_ModuleCatalog_Acomponent_impl.hxx
  SALOME_ModuleCatalog_impl.hxx
)

ADD_LIBRARY(SalomeCatalog ${SalomeCatalog_SOURCES})
TARGET_LINK_LIBRARIES(SalomeCatalog ${_link_LIBRARIES})
INSTALL(TARGETS SalomeCatalog EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

ADD_EXECUTABLE(SALOME_ModuleCatalog_Server SALOME_ModuleCatalog_Server.cxx)
TARGET_LINK_LIBRARIES(SALOME_ModuleCatalog_Server ${_link_LIBRARIES} SalomeCatalog)

ADD_EXECUTABLE(SALOME_ModuleCatalog_Client SALOME_ModuleCatalog_Client.cxx)
TARGET_LINK_LIBRARIES(SALOME_ModuleCatalog_Client ${_link_LIBRARIES} SalomeCatalog)

INSTALL(TARGETS SALOME_ModuleCatalog_Server SALOME_ModuleCatalog_Client 
        EXPORT ${PROJECT_NAME}TargetGroup
        DESTINATION ${SALOME_INSTALL_BINS})

IF(SALOME_BUILD_TESTS)
  SALOME_INSTALL_SCRIPTS(TestModuleCatalog.py ${SALOME_INSTALL_SCRIPT_PYTHON})
ENDIF()

INSTALL(FILES ${SalomeCatalog_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})

UNSET(SalomeCatalog_SOURCES)
UNSET(SalomeCatalog_HEADERS)
UNSET(_link_LIBRARIES)
