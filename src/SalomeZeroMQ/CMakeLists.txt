# Copyright (C) 2012-2021  CEA/DEN, EDF R&D, OPEN CASCADE
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
  ${cppzmq_INCLUDE_DIR}
  ${ZeroMQ_INCLUDE_DIR}
  ${PYTHON_INCLUDE_DIRS}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/../Basics
)

# libraries to link to
SET(SalomeZeroQM_LIBRARIES
  ${cppzmq_LIBRARY}
  ${ZeroMQ_LIBRARY}
  )

SET(SCRIPTS SalomeZeroMQPy.py)

# Library
ADD_LIBRARY(SalomeZeroMQ SalomeZeroMQ.cxx)
TARGET_LINK_LIBRARIES(SalomeZeroMQ ${SalomeZeroQM_LIBRARIES})
INSTALL(TARGETS SalomeZeroMQ EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})
INSTALL(FILES SalomeZeroMQ.hxx SALOME_ZeroMQ.hxx DESTINATION ${SALOME_INSTALL_HEADERS})

# Executable scripts to be installed
SALOME_INSTALL_SCRIPTS("${SCRIPTS}" ${SALOME_INSTALL_PYTHON})

IF(SALOME_BUILD_TESTS)
  ADD_SUBDIRECTORY(Test)
ENDIF(SALOME_BUILD_TESTS)