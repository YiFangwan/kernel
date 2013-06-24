# Copyright (C) 2013  CEA/DEN, EDF R&D, OPEN CASCADE
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

# LIBBATCH detection for Salome

include(FindPackageHandleStandardArgs)
set(LIBBATCH_ROOT_DIR $ENV{LIBBATCH_ROOT_DIR} CACHE PATH "Path to libbatch directory")

if(EXISTS ${LIBBATCH_ROOT_DIR})
  if(WINDOWS)
    set(PREV_CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_FIND_LIBRARY_SUFFIXES})
    set(CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_IMPORT_LIBRARY_SUFFIX})
  endif(WINDOWS)
  find_package(libbatch QUIET CONFIG PATHS ${LIBBATCH_ROOT_DIR}/lib NO_DEFAULT_PATH)
endif(EXISTS ${LIBBATCH_ROOT_DIR})

find_package(libbatch QUIET CONFIG)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(libbatch DEFAULT_MSG
  LIBBATCH_ROOT_DIR LIBBATCH_INCLUDE_DIRS LIBBATCH_LIBRARIES
  LIBBATCH_RUNTIME_LIBRARY_DIRS LIBBATCH_PYTHONPATH)

message("LIBBATCH_LIBRARIES ${LIBBATCH_LIBRARIES}")
message("LIBBATCH_INCLUDE_DIRS ${LIBBATCH_INCLUDE_DIRS}")
message("LIBBATCH_RUNTIME_LIBRARY_DIRS ${LIBBATCH_RUNTIME_LIBRARY_DIRS}")
message("LIBBATCH_PYTHONPATH ${LIBBATCH_PYTHONPATH}")

if(WINDOWS)
  set(CMAKE_FIND_LIBRARY_SUFFIXES ${PREV_CMAKE_FIND_LIBRARY_SUFFIXES})
endif(WINDOWS)
