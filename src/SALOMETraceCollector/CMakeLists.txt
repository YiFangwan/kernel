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
  ${CMAKE_CURRENT_SOURCE_DIR}/../Basics
  ${CMAKE_CURRENT_SOURCE_DIR}/../SALOMELocalTrace
)

ADD_DEFINITIONS(${OMNIORB_DEFINITIONS})

SET(_link_LIBRARIES
  ${PLATFORM_LIBS}
  ${PTHREAD_LIBRARIES}
  ${OMNIORB_LIBRARIES}
  SalomeIDLKernel
  SALOMELocalTrace
)

SET(TraceCollector_SOURCES
  SALOMETraceCollector.cxx
  TraceCollector_WaitForServerReadiness.cxx
)

SET(TraceCollector_HEADERS
  SALOMETraceCollector.hxx
  TraceCollector_WaitForServerReadiness.hxx
)

ADD_LIBRARY(with_loggerTraceCollector ${TraceCollector_SOURCES})
TARGET_LINK_LIBRARIES(with_loggerTraceCollector ${_link_LIBRARIES})
INSTALL(TARGETS with_loggerTraceCollector EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

INSTALL(FILES ${TraceCollector_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})

UNSET(TraceCollector_SOURCES)
UNSET(TraceCollector_HEADERS)
UNSET(_link_LIBRARIES)

IF(SALOME_BUILD_TESTS)
  ADD_SUBDIRECTORY(Test)
ENDIF()
