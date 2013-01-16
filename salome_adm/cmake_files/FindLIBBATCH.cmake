# Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
#
# Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
# CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License.
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

# ------

MESSAGE(STATUS "Check for libbatch ...")

# ------

IF(LIBBATCH_IS_MANDATORY STREQUAL 0)
  SET(LIBBATCH_IS_MANDATORY 0)
  SET(LIBBATCH_IS_OPTIONAL 1)
ENDIF(LIBBATCH_IS_MANDATORY STREQUAL 0)
IF(LIBBATCH_IS_OPTIONAL STREQUAL 0)
  SET(LIBBATCH_IS_MANDATORY 1)
  SET(LIBBATCH_IS_OPTIONAL 0)
ENDIF(LIBBATCH_IS_OPTIONAL STREQUAL 0)
IF(NOT LIBBATCH_IS_MANDATORY AND NOT LIBBATCH_IS_OPTIONAL)
  SET(LIBBATCH_IS_MANDATORY 0)
  SET(LIBBATCH_IS_OPTIONAL 1)
ENDIF(NOT LIBBATCH_IS_MANDATORY AND NOT LIBBATCH_IS_OPTIONAL)

# ------

SET(LIBBATCH_STATUS 1)
IF(WITHOUT_LIBBATCH OR WITH_LIBBATCH STREQUAL 0)
  SET(LIBBATCH_STATUS 0)
  MESSAGE(STATUS "libbatch disabled from command line.")
ENDIF(WITHOUT_LIBBATCH OR WITH_LIBBATCH STREQUAL 0)

# ------

IF(LIBBATCH_STATUS)
  IF(WITH_LIBBATCH)
    SET(LIBBATCH_ROOT_USER ${WITH_LIBBATCH})
  ELSE(WITH_LIBBATCH)
    SET(LIBBATCH_ROOT_USER $ENV{LIBBATCH_ROOT_DIR})
  ENDIF(WITH_LIBBATCH)
ENDIF(LIBBATCH_STATUS)

# -----

IF(LIBBATCH_STATUS)
  IF(LIBBATCH_ROOT_USER)
    SET(LIBBATCH_FIND_PATHS_OPTION NO_DEFAULT_PATH)
  ELSE(LIBBATCH_ROOT_USER)
    SET(LIBBATCH_FIND_PATHS_OPTION)
  ENDIF(LIBBATCH_ROOT_USER)
ENDIF(LIBBATCH_STATUS)

# -----

IF(LIBBATCH_STATUS)
  IF(LIBBATCH_ROOT_USER)
    SET(LIBBATCH_INCLUDE_PATHS ${LIBBATCH_ROOT_USER} ${LIBBATCH_ROOT_USER}/include)
  ELSE(LIBBATCH_ROOT_USER)
    SET(LIBBATCH_INCLUDE_PATHS)
  ENDIF(LIBBATCH_ROOT_USER)
  SET(LIBBATCH_INCLUDE_TO_FIND Batch/BatchManager.hxx)
  FIND_PATH(LIBBATCH_INCLUDE_DIR ${LIBBATCH_INCLUDE_TO_FIND} PATHS ${LIBBATCH_INCLUDE_PATHS} ${LIBBATCH_FIND_PATHS_OPTION})
  IF(LIBBATCH_INCLUDE_DIR)
    IF(LIBBATCH_ROOT_USER)
      SET(LIBBATCH_INCLUDES -I${LIBBATCH_INCLUDE_DIR}) # to be removed
      SET(LIBBATCH_INCLUDE_DIRS ${LIBBATCH_INCLUDE_DIR})       
    ENDIF(LIBBATCH_ROOT_USER)
    MESSAGE(STATUS "${LIBBATCH_INCLUDE_TO_FIND} found in ${LIBBATCH_INCLUDE_DIR}")
  ELSE(LIBBATCH_INCLUDE_DIR)
    SET(LIBBATCH_STATUS 0)
    IF(LIBBATCH_ROOT_USER)
      MESSAGE(STATUS "${LIBBATCH_INCLUDE_TO_FIND} not found in ${LIBBATCH_INCLUDE_PATHS}, check your LIBBATCH installation.")
    ELSE(LIBBATCH_ROOT_USER)
      MESSAGE(STATUS "${LIBBATCH_INCLUDE_TO_FIND} not found on system, try to use WITH_LIBBATCH option or LIBBATCH_ROOT environment variable.")
    ENDIF(LIBBATCH_ROOT_USER)
  ENDIF(LIBBATCH_INCLUDE_DIR)
ENDIF(LIBBATCH_STATUS)

# ----

IF(LIBBATCH_STATUS)
  IF(LIBBATCH_ROOT_USER)
    SET(LIBBATCH_LIB_PATHS ${LIBBATCH_ROOT_USER}/lib)
  ELSE(LIBBATCH_ROOT_USER)
    SET(LIBBATCH_LIB_PATHS)
  ENDIF(LIBBATCH_ROOT_USER)
ENDIF(LIBBATCH_STATUS)

IF(LIBBATCH_STATUS)
  IF(WINDOWS)
    FIND_LIBRARY(LIBBATCH_LIB Batch PATHS ${LIBBATCH_LIB_PATHS} ${LIBBATCH_FIND_PATHS_OPTION})
  ELSE(WINDOWS)
    FIND_LIBRARY(LIBBATCH_LIB Batch PATHS ${LIBBATCH_LIB_PATHS} ${LIBBATCH_FIND_PATHS_OPTION})
  ENDIF(WINDOWS)
  SET(LIBBATCH_LIBS)
  IF(LIBBATCH_LIB)
    SET(LIBBATCH_LIBS ${LIBBATCH_LIBS} ${LIBBATCH_LIB})
    MESSAGE(STATUS "libbatch lib found: ${LIBBATCH_LIB}")
  ELSE(LIBBATCH_LIB)
    SET(LIBBATCH_STATUS 0)
    IF(LIBBATCH_ROOT_USER)
      MESSAGE(STATUS "libbatch lib not found in ${LIBBATCH_LIB_PATHS}, check your LIBBATCH installation.")
    ELSE(LIBBATCH_ROOT_USER)
      MESSAGE(STATUS "libbatch lib not found on system, try to use WITH_LIBBATCH option or LIBBATCH_ROOT environment variable.")
    ENDIF(LIBBATCH_ROOT_USER)
  ENDIF(LIBBATCH_LIB)
ENDIF(LIBBATCH_STATUS)

# ----

IF(LIBBATCH_STATUS)
  SET(WITH_LIBBATCH 1)
ELSE(LIBBATCH_STATUS)
  IF(LIBBATCH_IS_MANDATORY)
    MESSAGE(FATAL_ERROR "libbatch not found ... mandatory ... abort")
  ELSE(LIBBATCH_IS_MANDATORY)
    MESSAGE(STATUS "libbatch not found ... optional ... disabled")
  ENDIF(LIBBATCH_IS_MANDATORY)
ENDIF(LIBBATCH_STATUS)

# ------
