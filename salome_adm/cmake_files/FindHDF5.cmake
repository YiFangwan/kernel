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

MESSAGE(STATUS "Check for hdf5 ...")

# ------

IF(HDF5_IS_MANDATORY STREQUAL 0)
  SET(HDF5_IS_MANDATORY 0)
  SET(HDF5_IS_OPTIONAL 1)
ENDIF(HDF5_IS_MANDATORY STREQUAL 0)
IF(HDF5_IS_OPTIONAL STREQUAL 0)
  SET(HDF5_IS_MANDATORY 1)
  SET(HDF5_IS_OPTIONAL 0)
ENDIF(HDF5_IS_OPTIONAL STREQUAL 0)
IF(NOT HDF5_IS_MANDATORY AND NOT HDF5_IS_OPTIONAL)
  SET(HDF5_IS_MANDATORY 1)
  SET(HDF5_IS_OPTIONAL 0)
ENDIF(NOT HDF5_IS_MANDATORY AND NOT HDF5_IS_OPTIONAL)

# ------

SET(HDF5_STATUS 1)
IF(WITHOUT_HDF5 OR WITH_HDF5 STREQUAL 0)
  SET(HDF5_STATUS 0)
  MESSAGE(STATUS "hdf5 disabled from command line.")
ENDIF(WITHOUT_HDF5 OR WITH_HDF5 STREQUAL 0)

# ------

IF(HDF5_STATUS)
  IF(WITH_HDF5)
    SET(HDF5_ROOT_USER ${WITH_HDF5})
  ENDIF(WITH_HDF5)
  IF(NOT HDF5_ROOT_USER)
    SET(HDF5_ROOT_USER $ENV{HDF5_ROOT})
  ENDIF(NOT HDF5_ROOT_USER)
  IF(NOT HDF5_ROOT_USER)
    SET(HDF5_ROOT_USER $ENV{HDF5HOME})
  ENDIF(NOT HDF5_ROOT_USER)
ENDIF(HDF5_STATUS)

# ------

IF(HDF5_STATUS)
  IF(NOT HDF5_ROOT_USER)
    FIND_PROGRAM(HDF5_DUMP h5dump)
    IF(HDF5_DUMP)
      MESSAGE(STATUS "h5dump found: ${HDF5_DUMP}")
      IF(HDF5_DUMP STREQUAL /usr/bin/h5dump)
      ELSE(HDF5_DUMP STREQUAL /usr/bin/h5dump)
	SET(HDF5_ROOT_USER ${HDF5_DUMP})
	GET_FILENAME_COMPONENT(HDF5_ROOT_USER ${HDF5_ROOT_USER} PATH)
	GET_FILENAME_COMPONENT(HDF5_ROOT_USER ${HDF5_ROOT_USER} PATH)
      ENDIF(HDF5_DUMP STREQUAL /usr/bin/h5dump)
    ELSE(HDF5_DUMP)
      MESSAGE(STATUS "h5dump not found, try to use WITH_HDF5 option or HDF5_ROOT (or HDF5HOME) environment variable")
      SET(HDF5_STATUS 0)
    ENDIF(HDF5_DUMP)
  ENDIF(NOT HDF5_ROOT_USER)
ENDIF(HDF5_STATUS)

# ------

IF(HDF5_STATUS)
  IF(HDF5_ROOT_USER)
    SET(HDF5_FIND_PATHS_OPTION NO_DEFAULT_PATH)
  ELSE(HDF5_ROOT_USER)
    SET(HDF5_FIND_PATHS_OPTION)
  ENDIF(HDF5_ROOT_USER)
ENDIF(HDF5_STATUS)

# ------

IF(HDF5_STATUS)
  IF(HDF5_ROOT_USER)
    SET(HDF5_INCLUDE_PATHS ${HDF5_ROOT_USER}/include)
  ELSE(HDF5_ROOT_USER)
    SET(HDF5_INCLUDE_PATHS)
  ENDIF(HDF5_ROOT_USER)
  SET(HDF5_INCLUDE_TO_FIND H5public.h)
  FIND_PATH(HDF5_INCLUDE_DIR ${HDF5_INCLUDE_TO_FIND} PATHS ${HDF5_INCLUDE_PATHS} ${HDF5_FIND_PATHS_OPTION})
  IF(HDF5_INCLUDE_DIR)
    IF(HDF5_ROOT_USER)
      SET(HDF5_INCLUDES -I${HDF5_INCLUDE_DIR})# to be removed
      IF(${HDF5_INCLUDE_DIR} STREQUAL /usr/include)
	SET(HDF5_INCLUDE_DIRS)
      ELSE(${HDF5_INCLUDE_DIR} STREQUAL /usr/include)
	SET(HDF5_INCLUDE_DIRS ${HDF5_INCLUDE_DIR})
      ENDIF(${HDF5_INCLUDE_DIR} STREQUAL /usr/include)
    ENDIF(HDF5_ROOT_USER)
    SET(HDF5_FLAGS "-DH5_USE_16_API -D_HDF5USEDLL_")
    SET(HDF5_INCLUDES -DH5_USE_16_API -D_HDF5USEDLL_ ${HDF5_INCLUDES})# to be removed
    MESSAGE(STATUS "${HDF5_INCLUDE_TO_FIND} found in ${HDF5_INCLUDE_DIR}")
  ELSE(HDF5_INCLUDE_DIR)
    SET(HDF5_STATUS 0)
    IF(HDF5_ROOT_USER)
      MESSAGE(STATUS "${HDF5_INCLUDE_TO_FIND} not found in ${HDF5_INCLUDE_PATHS}, check your HDF5 installation.")
    ELSE(HDF5_ROOT_USER)
      MESSAGE(STATUS "${HDF5_INCLUDE_TO_FIND} not found on system, try to use WITH_HDF5 option or HDF5_ROOT (or HDF5HOME) environment variable.")
    ENDIF(HDF5_ROOT_USER)
  ENDIF(HDF5_INCLUDE_DIR)
ENDIF(HDF5_STATUS)

# ----

IF(HDF5_STATUS)
  IF(HDF5_ROOT_USER)
    IF(WINDOWS)
      SET(HDF5_LIB_PATHS ${HDF5_ROOT_USER}/dll ${HDF5_ROOT_USER}/lib)
    ELSE(WINDOWS)
      SET(HDF5_LIB_PATHS ${HDF5_ROOT_USER}/lib)
    ENDIF(WINDOWS)
  ELSE(HDF5_ROOT_USER)
    SET(HDF5_LIB_PATHS)
  ENDIF(HDF5_ROOT_USER)
  IF(WINDOWS)
    IF(CMAKE_BUILD_TYPE STREQUAL Release)
      FIND_LIBRARY(HDF5_LIB hdf5dll PATHS ${HDF5_LIB_PATHS} ${HDF5_FIND_PATHS_OPTION})
    ELSE(CMAKE_BUILD_TYPE STREQUAL Release)
      FIND_LIBRARY(HDF5_LIB hdf5ddll PATHS ${HDF5_LIB_PATHS} ${HDF5_FIND_PATHS_OPTION})
    ENDIF(CMAKE_BUILD_TYPE STREQUAL Release)
  ELSE(WINDOWS)
    FIND_LIBRARY(HDF5_LIB hdf5 PATHS ${HDF5_LIB_PATHS} ${HDF5_FIND_PATHS_OPTION})
  ENDIF(WINDOWS)
  SET(HDF5_LIBS)
  IF(HDF5_LIB)
    SET(HDF5_LIBS ${HDF5_LIBS} ${HDF5_LIB})
    MESSAGE(STATUS "hdf5 lib found: ${HDF5_LIB}")
  ELSE(HDF5_LIB)
    SET(HDF5_STATUS 0)
    IF(HDF5_ROOT_USER)
      MESSAGE(STATUS "hdf5 lib not found in ${HDF5_LIB_PATHS}, check your HDF5 installation.")
    ELSE(HDF5_ROOT_USER)
      MESSAGE(STATUS "hdf5 lib not found on system, try to use WITH_HDF5 option or HDF5_ROOT (or HDF5HOME) environment variable.")
    ENDIF(HDF5_ROOT_USER)
  ENDIF(HDF5_LIB)
ENDIF(HDF5_STATUS)

IF(HDF5_STATUS)
  FILE(TO_NATIVE_PATH "${HDF5_INCLUDE_DIR}/H5public.h" HDF5_H5PUBLIC_FILE)
  EXECUTE_PROCESS(
    COMMAND ${PYTHON_EXECUTABLE} -c "import re,sys ; f=file(sys.argv[1]) ; s=f.read() ; c=re.compile('^#[\\s]*include[\\s]*\\\"(?P<nm>[A-Za-z0-9\\-]+\\.h)\\\"',re.M) ; sys.stdout.write(c.search(s).group('nm'))"
            ${HDF5_H5PUBLIC_FILE}
    OUTPUT_VARIABLE HDF5_H5PUBLIC_DEFS_FILE
    )
  FILE(TO_NATIVE_PATH "${HDF5_INCLUDE_DIR}/${HDF5_H5PUBLIC_DEFS_FILE}" HDF5_H5PUBLIC_DEFS_FILE)
  EXECUTE_PROCESS(
    COMMAND ${PYTHON_EXECUTABLE} -c "import re,sys ; f=file(sys.argv[1]) ; s=f.read() ; c=re.compile('^#[\\s]*define[\\s]+H5_HAVE_PARALLEL[\\s]+(?P<nm>[\\d]+)',re.M) ; m=c.search(s); exec('if m: sys.stdout.write(m.group(\\'nm\\'))') ; exec('if not m: sys.stdout.write(\\'0\\')')"
            ${HDF5_H5PUBLIC_DEFS_FILE}
    OUTPUT_VARIABLE HDF5_HAVE_PARALLEL
    )
  IF(HDF5_HAVE_PARALLEL)
    MESSAGE(STATUS "hdf5 is parallel")
    SET(HDF5_INCLUDES ${HDF5_INCLUDES} -I${MPI_INCLUDE_DIRS})       # to remove after "cmakeization"
    SET(HDF5_INCLUDE_DIRS ${HDF5_INCLUDE_DIRS} ${MPI_INCLUDE_DIRS})
    SET(HDF5_FLAGS "${HDF5_FLAGS} ${MPI_FLAGS}")
    SET(HDF5_LIBS ${HDF5_LIBS} ${MPI_LIBS})
  ENDIF(HDF5_HAVE_PARALLEL)
ENDIF(HDF5_STATUS)

# ----

IF(HDF5_STATUS)
  SET(HDF5_CPPFLAGS ${HDF5_INCLUDES})# to be removed
ELSE(HDF5_STATUS)
  IF(HDF5_IS_MANDATORY)
    MESSAGE(FATAL_ERROR "hdf5 not found ... mandatory ... abort")
  ELSE(HDF5_IS_MANDATORY)
    MESSAGE(STATUS "hdf5 not found ... optional ... disabled")
  ENDIF(HDF5_IS_MANDATORY)
ENDIF(HDF5_STATUS)

# ------

#SET(ZLIB_ROOT $ENV{ZLIB_ROOT})
#STRING(LENGTH "${ZLIB_ROOT}" NB)
#IF(${NB} GREATER 0)
#  FIND_LIBRARY(ZLIB_LIBS zlib ${ZLIB_ROOT}/lib)
#  SET(HDF5_LIBS ${HDF5_LIBS} ${ZLIB_LIBS})
#ENDIF(${NB} GREATER 0)

#SET(SZIP_ROOT $ENV{SZIP_ROOT})
#STRING(LENGTH "${SZIP_ROOT}" NB)
#IF(${NB} GREATER 0)
#  FIND_LIBRARY(SZIP_LIBS szlib ${SZIP_ROOT}/lib)
#  SET(HDF5_LIBS ${HDF5_LIBS} ${SZIP_LIBS})
#ENDIF(${NB} GREATER 0)

# ------
