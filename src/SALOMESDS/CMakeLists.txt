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
# Author : Anthony GEAY (EDF R&D)

INCLUDE_DIRECTORIES(
  ${OMNIORB_INCLUDE_DIR}
  ${PTHREAD_INCLUDE_DIR}
  ${PYTHON_INCLUDE_DIRS}
  ${CMAKE_BINARY_DIR}/idl
  ${CMAKE_BINARY_DIR}/salome_adm
  ${CMAKE_SOURCE_DIR}/src/Utils
  ${CMAKE_SOURCE_DIR}/src/NamingService
  ${CMAKE_SOURCE_DIR}/src/Launcher
  ${CMAKE_SOURCE_DIR}/src/Container
  ${CMAKE_SOURCE_DIR}/src/ResourcesManager
)

ADD_DEFINITIONS(${OMNIORB_DEFINITIONS})

SET(_link_LIBRARIES
  ${PLATFORM_LIBS}
  ${PTHREAD_LIBRARIES}
  ${OMNIORB_LIBRARIES}
  ${PYTHON_LIBRARIES}
  SalomeNS
  SalomeContainer
  SalomeIDLKernel
)

SET(SalomeSDS_SOURCES
  SALOMESDS_Exception.cxx
  SALOMESDS_RefCountServ.cxx
  SALOMESDS_BasicDataServer.cxx
  SALOMESDS_DataScopeServer.cxx
  SALOMESDS_RequestSwitcher.cxx
  SALOMESDS_DataServerManager.cxx
  SALOMESDS_PickelizedPyObjServer.cxx
  SALOMESDS_PickelizedPyObjRdOnlyServer.cxx
  SALOMESDS_PickelizedPyObjRdExtServer.cxx
  SALOMESDS_PickelizedPyObjRdExtInitServer.cxx
  SALOMESDS_PickelizedPyObjRdWrServer.cxx
  SALOMESDS_Transaction.cxx
  SALOMESDS_KeyWaiter.cxx
  SALOMESDS_Sha1Keeper.cxx
)

ADD_LIBRARY(SalomeSDS ${SalomeSDS_SOURCES})
TARGET_LINK_LIBRARIES(SalomeSDS ${_link_LIBRARIES})
INSTALL(TARGETS SalomeSDS EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

ADD_EXECUTABLE(SALOME_DataScopeServer SALOME_DataScopeServer.cxx)
TARGET_LINK_LIBRARIES(SALOME_DataScopeServer ${_link_LIBRARIES} SalomeSDS)
INSTALL(TARGETS SALOME_DataScopeServer EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_BINS})

INSTALL(FILES SalomeSDSClt.py SALOMEWrappedStdType.py DESTINATION ${SALOME_INSTALL_BINS})

SET(SalomeSDS_HEADERS
  SALOMESDS_Auto.hxx
  SALOMESDS_AutoRefCountPtr.hxx
  SALOMESDS_BasicDataServer.hxx
  SALOMESDS_DataScopeServer.hxx
  SALOMESDS_DataServerManager.hxx
  SALOMESDS_Defines.hxx
  SALOMESDS_Exception.hxx
  SALOMESDS_KeyWaiter.hxx
  SALOMESDS_PickelizedPyObjRdExtInitServer.hxx
  SALOMESDS_PickelizedPyObjRdExtServer.hxx
  SALOMESDS_PickelizedPyObjRdOnlyServer.hxx
  SALOMESDS_PickelizedPyObjRdWrServer.hxx
  SALOMESDS_PickelizedPyObjServer.hxx
  SALOMESDS_RefCountServ.hxx
  SALOMESDS_RequestSwitcher.hxx
  SALOMESDS_Sha1Keeper.hxx
  SALOMESDS_Transaction.hxx
  SALOMESDS_TrustTransaction.hxx
)

INSTALL(FILES ${SalomeSDS_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})

IF(SALOME_BUILD_TESTS)
  SET(LOCAL_TEST_DIR ${KERNEL_TEST_DIR}/SALOMESDS)
  INSTALL(FILES TestSalomeSDS.py TestSalomeSDSHelper0.py DESTINATION ${LOCAL_TEST_DIR})
  INSTALL(FILES CTestTestfileInstall.cmake DESTINATION ${LOCAL_TEST_DIR} RENAME CTestTestfile.cmake)
ENDIF()

UNSET(LOCAL_TEST_DIR)
UNSET(SalomeSDS_SOURCES)
UNSET(SalomeSDS_HEADERS)
UNSET(_link_LIBRARIES)
