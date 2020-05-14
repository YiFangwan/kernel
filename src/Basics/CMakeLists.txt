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
)

SET(_link_LIBRARIES
  ${PLATFORM_LIBS}
  ${PTHREAD_LIBRARIES}
)

SET(SALOMEBasics_SOURCES
  Basics_DirUtils.cxx
  Basics_GenericDestructor.cxx
  Basics_Utils.cxx
)

SET(SALOMEBasics_HEADERS
  Basics_DirUtils.hxx
  Basics_GenericDestructor.hxx
  Basics_OCCTVersion.hxx
  Basics_Utils.hxx
  SALOME_Basics.hxx
)

ADD_LIBRARY(SALOMEBasics ${SALOMEBasics_SOURCES})
TARGET_LINK_LIBRARIES(SALOMEBasics ${_link_LIBRARIES})
INSTALL(TARGETS SALOMEBasics EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

INSTALL(FILES ${SALOMEBasics_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})

UNSET(SALOMEBasics_SOURCES)
UNSET(SALOMEBasics_HEADERS)
UNSET(_link_LIBRARIES)

ADD_SUBDIRECTORY(Test)
