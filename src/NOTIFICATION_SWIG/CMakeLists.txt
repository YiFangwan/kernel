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

INCLUDE(${SWIG_USE_FILE})

INCLUDE_DIRECTORIES(
  ${PTHREAD_INCLUDE_DIR}
  ${PYTHON_INCLUDE_DIRS} 
  ${OMNIORB_INCLUDE_DIR}
  ${CMAKE_BINARY_DIR}/salome_adm
  ${CMAKE_SOURCE_DIR}/src/Basics
  ${CMAKE_SOURCE_DIR}/src/SALOMELocalTrace
  ${CMAKE_SOURCE_DIR}/src/Notification
  ${CMAKE_CURRENT_SOURCE_DIR}
)

ADD_DEFINITIONS(${OMNIORB_DEFINITIONS})

SET(_link_LIBRARIES
  ${PLATFORM_LIBS}
  ${PTHREAD_LIBRARIES}
  ${OMNIORB_LIBRARIES}
  ${PYTHON_LIBRARIES}
  SalomeNotification
)

# SWIG module name differs from file name - avoid unnecessary rebuild: 
SET_SOURCE_FILES_PROPERTIES(NOTIFICATION.i PROPERTIES SWIG_MODULE_NAME libNOTIFICATION)
SET_SOURCE_FILES_PROPERTIES(NOTIFICATION.i PROPERTIES CPLUSPLUS ON)
SET_SOURCE_FILES_PROPERTIES(NOTIFICATION.i PROPERTIES SWIG_FLAGS "-py3")
SET_SOURCE_FILES_PROPERTIES(NOTIFICATIONPYTHON_wrap.cxx PROPERTIES COMPILE_FLAGS "${OMNIORB_DEFINITIONS} -DHAVE_CONFIG_H")

IF(${CMAKE_VERSION} VERSION_LESS "3.8.0") 
  SWIG_ADD_MODULE(libNOTIFICATION python NOTIFICATION.i NOTIFICATION_Swig.cxx)
ELSE()
  SWIG_ADD_LIBRARY(libNOTIFICATION LANGUAGE python SOURCES NOTIFICATION.i NOTIFICATION_Swig.cxx)
ENDIF()

SWIG_LINK_LIBRARIES(libNOTIFICATION ${_link_LIBRARIES})

IF(WIN32)
  SET_TARGET_PROPERTIES(_libNOTIFICATION PROPERTIES DEBUG_OUTPUT_NAME _libNOTIFICATION_d)
ENDIF()

INSTALL(TARGETS _libNOTIFICATION DESTINATION ${SALOME_INSTALL_PYTHON})
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/libNOTIFICATION.py DESTINATION ${SALOME_INSTALL_PYTHON})

UNSET(_link_LIBRARIES)
