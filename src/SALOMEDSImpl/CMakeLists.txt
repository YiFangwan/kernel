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
  ${HDF5_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIR}
  ${CMAKE_BINARY_DIR}/salome_adm
  ${CMAKE_BINARY_DIR}
  ${CMAKE_SOURCE_DIR}/src/Basics
  ${CMAKE_SOURCE_DIR}/src/SALOMELocalTrace
  ${CMAKE_SOURCE_DIR}/src/HDFPersist
  ${CMAKE_SOURCE_DIR}/src/DF
)

ADD_DEFINITIONS(${HDF5_DEFINITIONS} ${BOOST_DEFINITIONS})

SET(_link_LIBRARIES
  ${PLATFORM_LIBS}
  ${PTHREAD_LIBRARIES}
  ${HDF5_C_LIBRARIES}
  SALOMEBasics
  SALOMELocalTrace
  SalomeHDFPersist
  DF
)

SET(SalomeDSImpl_SOURCES
  SALOMEDSImpl_Tool.cxx
  SALOMEDSImpl_Callback.cxx
  SALOMEDSImpl_StudyHandle.cxx
  SALOMEDSImpl_GenericAttribute.cxx
  SALOMEDSImpl_SObject.cxx
  SALOMEDSImpl_SComponent.cxx
  SALOMEDSImpl_ChildIterator.cxx
  SALOMEDSImpl_AttributeIOR.cxx
  SALOMEDSImpl_AttributeExternalFileDef.cxx
  SALOMEDSImpl_AttributeFileType.cxx
  SALOMEDSImpl_AttributePersistentRef.cxx
  SALOMEDSImpl_AttributeSequenceOfReal.cxx
  SALOMEDSImpl_AttributeSequenceOfInteger.cxx
  SALOMEDSImpl_AttributeDrawable.cxx
  SALOMEDSImpl_AttributeSelectable.cxx
  SALOMEDSImpl_AttributeExpandable.cxx
  SALOMEDSImpl_AttributeOpened.cxx
  SALOMEDSImpl_AttributeFlags.cxx
  SALOMEDSImpl_AttributeGraphic.cxx
  SALOMEDSImpl_AttributeTextColor.cxx
  SALOMEDSImpl_AttributeTextHighlightColor.cxx
  SALOMEDSImpl_AttributePixMap.cxx
  SALOMEDSImpl_AttributeLocalID.cxx
  SALOMEDSImpl_AttributeTarget.cxx
  SALOMEDSImpl_AttributeTableOfInteger.cxx
  SALOMEDSImpl_AttributeTableOfReal.cxx
  SALOMEDSImpl_AttributeTableOfString.cxx
  SALOMEDSImpl_AttributeStudyProperties.cxx
  SALOMEDSImpl_AttributePythonObject.cxx
  SALOMEDSImpl_AttributeReal.cxx
  SALOMEDSImpl_AttributeInteger.cxx
  SALOMEDSImpl_AttributeUserID.cxx
  SALOMEDSImpl_AttributeTreeNode.cxx
  SALOMEDSImpl_AttributeName.cxx
  SALOMEDSImpl_AttributeComment.cxx
  SALOMEDSImpl_AttributeReference.cxx
  SALOMEDSImpl_AttributeParameter.cxx
  SALOMEDSImpl_AttributeString.cxx
  SALOMEDSImpl_ChildNodeIterator.cxx
  SALOMEDSImpl_UseCaseBuilder.cxx
  SALOMEDSImpl_UseCaseIterator.cxx
  SALOMEDSImpl_SComponentIterator.cxx
  SALOMEDSImpl_StudyBuilder.cxx
  SALOMEDSImpl_Study.cxx
  SALOMEDSImpl_IParameters.cxx
  SALOMEDSImpl_TMPFile.cxx
  SALOMEDSImpl_GenericVariable.cxx
  SALOMEDSImpl_ScalarVariable.cxx
) 

SET(SalomeDSImpl_HEADERS
  SALOMEDSImpl_AttributeComment.hxx
  SALOMEDSImpl_AttributeDrawable.hxx
  SALOMEDSImpl_AttributeExpandable.hxx
  SALOMEDSImpl_AttributeExternalFileDef.hxx
  SALOMEDSImpl_AttributeFileType.hxx
  SALOMEDSImpl_AttributeFlags.hxx
  SALOMEDSImpl_AttributeGraphic.hxx
  SALOMEDSImpl_AttributeInteger.hxx
  SALOMEDSImpl_AttributeIOR.hxx
  SALOMEDSImpl_AttributeLocalID.hxx
  SALOMEDSImpl_AttributeName.hxx
  SALOMEDSImpl_AttributeOpened.hxx
  SALOMEDSImpl_AttributeParameter.hxx
  SALOMEDSImpl_AttributePersistentRef.hxx
  SALOMEDSImpl_AttributePixMap.hxx
  SALOMEDSImpl_AttributePythonObject.hxx
  SALOMEDSImpl_AttributeReal.hxx
  SALOMEDSImpl_AttributeReference.hxx
  SALOMEDSImpl_AttributeSelectable.hxx
  SALOMEDSImpl_AttributeSequenceOfInteger.hxx
  SALOMEDSImpl_AttributeSequenceOfReal.hxx
  SALOMEDSImpl_Attributes.hxx
  SALOMEDSImpl_AttributeString.hxx
  SALOMEDSImpl_AttributeStudyProperties.hxx
  SALOMEDSImpl_AttributeTable.hxx
  SALOMEDSImpl_AttributeTableOfInteger.hxx
  SALOMEDSImpl_AttributeTableOfReal.hxx
  SALOMEDSImpl_AttributeTableOfString.hxx
  SALOMEDSImpl_AttributeTarget.hxx
  SALOMEDSImpl_AttributeTextColor.hxx
  SALOMEDSImpl_AttributeTextHighlightColor.hxx
  SALOMEDSImpl_AttributeTreeNode.hxx
  SALOMEDSImpl_AttributeUserID.hxx
  SALOMEDSImpl_Callback.hxx
  SALOMEDSImpl_ChildIterator.hxx
  SALOMEDSImpl_ChildNodeIterator.hxx
  SALOMEDSImpl_Defines.hxx
  SALOMEDSImpl_Driver.hxx
  SALOMEDSImpl_GenericAttribute.hxx
  SALOMEDSImpl_GenericVariable.hxx
  SALOMEDSImpl_IParameters.hxx
  SALOMEDSImpl_ScalarVariable.hxx
  SALOMEDSImpl_SComponent.hxx
  SALOMEDSImpl_SComponentIterator.hxx
  SALOMEDSImpl_SObject.hxx
  SALOMEDSImpl_StudyBuilder.hxx
  SALOMEDSImpl_Study.hxx
  SALOMEDSImpl_TMPFile.hxx
  SALOMEDSImpl_UseCaseBuilder.hxx
  SALOMEDSImpl_UseCaseIterator.hxx
)
#  SALOMEDSImpl_Tool.hxx
#  SALOMEDSImpl_StudyHandle.hxx

ADD_LIBRARY(SalomeDSImpl ${SalomeDSImpl_SOURCES})
TARGET_LINK_LIBRARIES(SalomeDSImpl ${_link_LIBRARIES})
INSTALL(TARGETS SalomeDSImpl EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

IF(SALOME_BUILD_TESTS)
  ADD_EXECUTABLE(testDS testDS.cxx)
  TARGET_LINK_LIBRARIES(testDS ${_link_LIBRARIES} SalomeDSImpl)
  INSTALL(TARGETS testDS DESTINATION ${SALOME_INSTALL_BINS})
ENDIF()

INSTALL(FILES ${SalomeDSImpl_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})

UNSET(_link_LIBRARIES)
UNSET(SalomeDSImpl_SOURCES)
UNSET(SalomeDSImpl_HEADERS)

IF(SALOME_BUILD_TESTS)
  ADD_SUBDIRECTORY(Test)
ENDIF()
