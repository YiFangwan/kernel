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

INCLUDE_DIRECTORIES(${PTHREAD_INCLUDE_DIR})
IF(NOT SALOME_LIGHT_ONLY)
  INCLUDE_DIRECTORIES(${OMNIORB_INCLUDE_DIR})
ENDIF()
INCLUDE_DIRECTORIES(
  ${CMAKE_BINARY_DIR}/salome_adm
  ${CMAKE_SOURCE_DIR}/src/Basics
  ${CMAKE_SOURCE_DIR}/src/SALOMELocalTrace
)
IF(NOT SALOME_LIGHT_ONLY)
  INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/idl)
ENDIF()

IF(NOT SALOME_LIGHT_ONLY)
  ADD_DEFINITIONS(${OMNIORB_DEFINITIONS})
ENDIF()

SET(_link_LIBRARIES ${PTHREAD_LIBRARIES})
IF(NOT SALOME_LIGHT_ONLY)
  SET(_link_LIBRARIES ${_link_LIBRARIES} ${OMNIORB_LIBRARIES})
ENDIF()
SET(_link_LIBRARIES ${_link_LIBRARIES} SALOMELocalTrace)
IF(NOT SALOME_LIGHT_ONLY)
  SET(_link_LIBRARIES ${_link_LIBRARIES} SalomeIDLKernel)
ENDIF()

SET(OpUtil_SOURCES
  Utils_SALOME_Exception.cxx
  duplicate.cxx
)  
IF(NOT SALOME_LIGHT_ONLY)
  SET(OpUtil_SOURCES ${OpUtil_SOURCES}
      Utils_CommException.cxx
      Utils_DESTRUCTEUR_GENERIQUE.cxx
      Utils_ExceptHandlers.cxx
      Utils_Identity.cxx
      Utils_Mutex.cxx
      Utils_ORB_INIT.cxx
      Utils_Timer.cxx
  )
ENDIF()

SET(OpUtil_HEADERS
  OpUtil.hxx
  Utils_SALOME_Exception.hxx
)
IF(NOT SALOME_LIGHT_ONLY)
  SET(OpUtil_HEADERS ${OpUtil_HEADERS}
  SALOME_Utils.hxx
  Utils_CommException.hxx
  Utils_CorbaException.hxx
  Utils_DESTRUCTEUR_GENERIQUE.hxx
  Utils_ExceptHandlers.hxx
  Utils_Identity.hxx
  Utils_Mutex.hxx
  Utils_ORB_INIT.hxx
  Utils_SINGLETON.hxx
  Utils_Timer.hxx
  )
ENDIF()

IF(NOT SALOME_LIGHT_ONLY)
  SET(OpUtil_SCRIPTS
    Utils_Identity.py 
    SALOME_utilities.py
  )
ENDIF()

ADD_LIBRARY(OpUtil ${OpUtil_SOURCES})
TARGET_LINK_LIBRARIES(OpUtil ${_link_LIBRARIES})
INSTALL(TARGETS OpUtil EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

INSTALL(FILES ${OpUtil_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})

SALOME_INSTALL_SCRIPTS("${OpUtil_SCRIPTS}" ${SALOME_INSTALL_SCRIPT_PYTHON})

UNSET(OpUtil_SOURCES)
UNSET(OpUtil_HEADERS)
UNSET(OpUtil_SCRIPTS)
UNSET(_link_LIBRARIES)

IF(SALOME_BUILD_TESTS)
  ADD_SUBDIRECTORY(Test)
ENDIF()
