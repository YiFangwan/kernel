#  Copyright (C) 2007-2010  CEA/DEN, EDF R&D, OPEN CASCADE
#
#  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
#  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
#  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

# ------

MESSAGE(STATUS "Check for doxygen ...")

# ------

IF(DOXYGEN_IS_MANDATORY STREQUAL 0)
  SET(DOXYGEN_IS_MANDATORY 0)
  SET(DOXYGEN_IS_OPTIONAL 1)
ENDIF(DOXYGEN_IS_MANDATORY STREQUAL 0)
IF(DOXYGEN_IS_OPTIONAL STREQUAL 0)
  SET(DOXYGEN_IS_MANDATORY 1)
  SET(DOXYGEN_IS_OPTIONAL 0)
ENDIF(DOXYGEN_IS_OPTIONAL STREQUAL 0)
IF(NOT DOXYGEN_IS_MANDATORY AND NOT DOXYGEN_IS_OPTIONAL)
  SET(DOXYGEN_IS_MANDATORY 0)
  SET(DOXYGEN_IS_OPTIONAL 1)
ENDIF(NOT DOXYGEN_IS_MANDATORY AND NOT DOXYGEN_IS_OPTIONAL)

# ------

SET(DOXYGEN_STATUS 1)
IF(WITHOUT_DOXYGEN OR WITH_DOXYGEN STREQUAL 0)
  SET(DOXYGEN_STATUS 0)
  MESSAGE(STATUS "doxygen disabled from command line.")
ENDIF(WITHOUT_DOXYGEN OR WITH_DOXYGEN STREQUAL 0)

# ------

IF(DOXYGEN_STATUS)
  IF(WITH_DOXYGEN)
    SET(DOXYGEN_ROOT_USER ${WITH_DOXYGEN})
  ENDIF(WITH_DOXYGEN)
  IF(NOT DOXYGEN_ROOT_USER)
    SET(DOXYGEN_ROOT_USER $ENV{DOXYGEN_ROOT})
  ENDIF(NOT DOXYGEN_ROOT_USER)
  IF(NOT DOXYGEN_ROOT_USER)
    SET(DOXYGEN_ROOT_USER $ENV{DOXYGENHOME})
  ENDIF(NOT DOXYGEN_ROOT_USER)
ENDIF(DOXYGEN_STATUS)

# ------

IF(DOXYGEN_STATUS)
  SET(DOXYGEN_EXECUTABLE_TO_FIND doxygen)
  IF(DOXYGEN_ROOT_USER)
    FIND_PROGRAM(DOXYGEN_EXECUTABLE ${DOXYGEN_EXECUTABLE_TO_FIND} PATHS ${DOXYGEN_ROOT_USER}/bin NO_DEFAULT_PATH)
  ELSE(DOXYGEN_ROOT_USER)
    FIND_PROGRAM(DOXYGEN_EXECUTABLE ${DOXYGEN_EXECUTABLE_TO_FIND})
  ENDIF(DOXYGEN_ROOT_USER)
  IF(DOXYGEN_EXECUTABLE)
    MESSAGE(STATUS "${DOXYGEN_EXECUTABLE_TO_FIND} found: ${DOXYGEN_EXECUTABLE}")
  ELSE(DOXYGEN_EXECUTABLE)
    MESSAGE(STATUS "${DOXYGEN_EXECUTABLE_TO_FIND} not found, try to use WITH_DOXYGEN option or DOXYGEN_ROOT (or DOXYGENHOME) environment variable")
    SET(DOXYGEN_STATUS 0)
  ENDIF(DOXYGEN_EXECUTABLE)
ENDIF(DOXYGEN_STATUS)

# ----

IF(DOXYGEN_STATUS)
  SET(DOXYGEN_IS_OK 1)
ELSE(DOXYGEN_STATUS)
  SET(DOXYGEN_IS_OK 0)
  IF(DOXYGEN_IS_MANDATORY)
    MESSAGE(FATAL_ERROR "doxygen not found ... mandatory ... abort")
  ELSE(DOXYGEN_IS_MANDATORY)
    MESSAGE(STATUS "doxygen not found ... optional ... disabled")
  ENDIF(DOXYGEN_IS_MANDATORY)
ENDIF(DOXYGEN_STATUS)

# ----
