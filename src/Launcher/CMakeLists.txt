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

IF(SALOME_BUILD_TESTS)
  ADD_SUBDIRECTORY(Test)
ENDIF(SALOME_BUILD_TESTS)

INCLUDE_DIRECTORIES(
  ${LIBXML2_INCLUDE_DIR}
  ${OMNIORB_INCLUDE_DIR}
  ${PYTHON_INCLUDE_DIRS}
  ${PTHREAD_INCLUDE_DIR}
  ${Boost_INCLUDE_DIR}
  ${LIBBATCH_INCLUDE_DIRS}
  ${CMAKE_BINARY_DIR}/salome_adm
  ${CMAKE_BINARY_DIR}/idl
  ${CMAKE_SOURCE_DIR}/src/Basics
  ${CMAKE_SOURCE_DIR}/src/SALOMELocalTrace
  ${CMAKE_SOURCE_DIR}/src/NamingService
  ${CMAKE_SOURCE_DIR}/src/Utils
  ${CMAKE_SOURCE_DIR}/src/Registry
  ${CMAKE_SOURCE_DIR}/src/Notification
  ${CMAKE_SOURCE_DIR}/src/Container
  ${CMAKE_SOURCE_DIR}/src/ResourcesManager
  ${CMAKE_SOURCE_DIR}/src/SALOMESDS
)

ADD_DEFINITIONS(${LIBXML2_DEFINITIONS} ${BOOST_DEFINITIONS} ${OMNIORB_DEFINITIONS})
IF(SALOME_USE_LIBBATCH)
  ADD_DEFINITIONS(-DWITH_LIBBATCH)
ENDIF(SALOME_USE_LIBBATCH)

SET(Launcher_SOURCES
  SALOME_Launcher_Parser.cxx
  SALOME_Launcher_Handler.cxx
  Launcher_Job.cxx
  Launcher_Job_Command.cxx
  Launcher_Job_CommandSALOME.cxx
  Launcher_Job_SALOME.cxx
  Launcher_Job_PythonSALOME.cxx
  Launcher_Job_YACSFile.cxx
  Launcher.cxx
  Launcher_XML_Persistence.cxx
)

SET(Launcher_LIBRARIES
  ${PLATFORM_LIBS}
  ${PTHREAD_LIBRARIES}
  ${LIBXML2_LIBRARIES}
  ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY}
  ${LIBBATCH_LIBRARIES}
  ResourcesManager
)

ADD_LIBRARY(Launcher ${Launcher_SOURCES})
TARGET_LINK_LIBRARIES(Launcher ${Launcher_LIBRARIES})
INSTALL(TARGETS Launcher EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

IF(SALOME_BUILD_TESTS)
  # TestLauncher needs only (and must need only)
  # libxml, libbatch, ResourceManager and Launcher libraries!
  ADD_EXECUTABLE(TestLauncher TestLauncher.cxx)
  TARGET_LINK_LIBRARIES(TestLauncher ${Launcher_LIBRARIES} Launcher)
  INSTALL(TARGETS TestLauncher DESTINATION ${SALOME_INSTALL_BINS})
  INSTALL(FILES testLauncher.xml DESTINATION ${SALOME_KERNEL_INSTALL_RES_DATA})
ENDIF()

SET(SalomeLauncher_SOURCES
  BatchTest.cxx
  SALOME_Launcher.cxx
  SALOME_ExternalServerLauncher.cxx
  SALOME_LauncherException.cxx
  SALOME_ExternalServerHandler.cxx
)

SET(SalomeLauncher_LIBRARIES
  ${PLATFORM_LIBS}
  ${PTHREAD_LIBRARIES}
  ${OMNIORB_LIBRARIES}
  ${LIBXML2_LIBRARIES}
  ${PYTHON_LIBRARIES}
  ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY}
  ${LIBBATCH_LIBRARIES}
  Registry
  SalomeNotification
  SalomeContainer
  SalomeResourcesManager
  SalomeNS
  OpUtil
  SALOMELocalTrace
  SALOMEBasics
  SalomeIDLKernel
  Launcher
)

ADD_LIBRARY(SalomeLauncher ${SalomeLauncher_SOURCES})
TARGET_LINK_LIBRARIES(SalomeLauncher ${SalomeLauncher_LIBRARIES})
INSTALL(TARGETS SalomeLauncher EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})
  
ADD_EXECUTABLE(SALOME_LauncherServer SALOME_LauncherServer.cxx)
TARGET_LINK_LIBRARIES(SALOME_LauncherServer ${SalomeLauncher_LIBRARIES} SalomeLauncher SalomeSDS)
INSTALL(TARGETS SALOME_LauncherServer EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_BINS})

SET(Launcher_HEADERS
  Launcher.hxx
  Launcher_Job.hxx
  Launcher_Job_Command.hxx
  Launcher_Job_CommandSALOME.hxx
  Launcher_Job_PythonSALOME.hxx
  Launcher_Job_SALOME.hxx
  Launcher_Job_YACSFile.hxx
  Launcher_Utils.hxx
  SALOME_Launcher.hxx
  SALOME_Launcher_Parser.hxx
  SALOME_Launcher_defs.hxx
  SALOME_ExternalServerLauncher.hxx
  SALOME_LauncherException.hxx
)
INSTALL(FILES ${Launcher_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})

SET(Launcher_SCRIPTS
  launcher_proxy.py
)
SALOME_INSTALL_SCRIPTS("${Launcher_SCRIPTS}" ${SALOME_INSTALL_PYTHON})

SET(Launcher_SOURCES)
SET(Launcher_LIBRARIES)
SET(SalomeLauncher_SOURCES)
SET(SalomeLauncher_LIBRARIES)
SET(Launcher_HEADERS)
SET(Launcher_SCRIPTS)
