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
  ${Boost_INCLUDE_DIR}
  ${CMAKE_BINARY_DIR}/salome_adm
  ${CMAKE_BINARY_DIR}/idl
)

ADD_DEFINITIONS(${OMNIORB_DEFINITIONS} ${BOOST_DEFINITIONS})

SET(_link_LIBRARIES
  ${PLATFORM_LIBS}
  ${PTHREAD_LIBRARIES}
  ${OMNIORB_LIBRARIES}
  SalomeIDLKernel
)

SET(SalomeDSClient_HEADERS
  SALOMEDSClient_AttributeComment.hxx
  SALOMEDSClient_AttributeDrawable.hxx
  SALOMEDSClient_AttributeExpandable.hxx
  SALOMEDSClient_AttributeExternalFileDef.hxx
  SALOMEDSClient_AttributeFileType.hxx
  SALOMEDSClient_AttributeFlags.hxx
  SALOMEDSClient_AttributeGraphic.hxx
  SALOMEDSClient_AttributeInteger.hxx
  SALOMEDSClient_AttributeIOR.hxx
  SALOMEDSClient_AttributeLocalID.hxx
  SALOMEDSClient_AttributeName.hxx
  SALOMEDSClient_AttributeOpened.hxx
  SALOMEDSClient_AttributeParameter.hxx
  SALOMEDSClient_AttributePersistentRef.hxx
  SALOMEDSClient_AttributePixMap.hxx
  SALOMEDSClient_AttributePythonObject.hxx
  SALOMEDSClient_AttributeReal.hxx
  SALOMEDSClient_AttributeSelectable.hxx
  SALOMEDSClient_AttributeSequenceOfInteger.hxx
  SALOMEDSClient_AttributeSequenceOfReal.hxx
  SALOMEDSClient_AttributeString.hxx
  SALOMEDSClient_AttributeStudyProperties.hxx
  SALOMEDSClient_AttributeTable.hxx
  SALOMEDSClient_AttributeTableOfInteger.hxx
  SALOMEDSClient_AttributeTableOfReal.hxx
  SALOMEDSClient_AttributeTableOfString.hxx
  SALOMEDSClient_AttributeTarget.hxx
  SALOMEDSClient_AttributeTextColor.hxx
  SALOMEDSClient_AttributeTextHighlightColor.hxx
  SALOMEDSClient_AttributeTreeNode.hxx
  SALOMEDSClient_AttributeUserID.hxx
  SALOMEDSClient_ChildIterator.hxx
  SALOMEDSClient_ClientFactory.hxx
  SALOMEDSClient_definitions.hxx
  SALOMEDSClient_GenericAttribute.hxx
  SALOMEDSClient.hxx
  SALOMEDSClient_IParameters.hxx
  SALOMEDSClient_Observer.hxx
  SALOMEDSClient_SComponent.hxx
  SALOMEDSClient_SComponentIterator.hxx
  SALOMEDSClient_SObject.hxx
  SALOMEDSClient_StudyBuilder.hxx
  SALOMEDSClient_Study.hxx
  SALOMEDSClient_UseCaseBuilder.hxx
  SALOMEDSClient_UseCaseIterator.hxx
)

ADD_LIBRARY(SalomeDSClient SALOMEDSClient_ClientFactory.cxx)
TARGET_LINK_LIBRARIES(SalomeDSClient ${_link_LIBRARIES})
INSTALL(TARGETS SalomeDSClient EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

INSTALL(FILES ${SalomeDSClient_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})

UNSET(SalomeDSClient_HEADERS)
UNSET(_link_LIBRARIES)
