# Copyright (C) 2012-2014  CEA/DEN, EDF R&D, OPEN CASCADE
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
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8 FATAL_ERROR)
PROJECT(Salome<Name_of_Application> C CXX)

# Ensure a proper linker behavior:
CMAKE_POLICY(SET CMP0003 NEW)

# Versioning
# ===========
# Project name, upper case
STRING(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UC)

SET(${PROJECT_NAME_UC}_MAJOR_VERSION 0)
SET(${PROJECT_NAME_UC}_MINOR_VERSION 0)
SET(${PROJECT_NAME_UC}_PATCH_VERSION 0)
SET(${PROJECT_NAME_UC}_VERSION
  ${${PROJECT_NAME_UC}_MAJOR_VERSION}.${${PROJECT_NAME_UC}_MINOR_VERSION}.${${PROJECT_NAME_UC}_PATCH_VERSION})

# Find KERNEL
# ===========
SET(KERNEL_ROOT_DIR $ENV{KERNEL_ROOT_DIR} CACHE PATH "Path to the Salome KERNEL")
IF(EXISTS ${KERNEL_ROOT_DIR})
  LIST(APPEND CMAKE_MODULE_PATH "${KERNEL_ROOT_DIR}/salome_adm/cmake_files")
  INCLUDE(SalomeMacros)
  FIND_PACKAGE(SalomeKERNEL REQUIRED)
  KERNEL_WITH_CORBA() # check whether KERNEL builded with CORBA
ELSE(EXISTS ${KERNEL_ROOT_DIR})
  MESSAGE(FATAL_ERROR "We absolutely need a Salome KERNEL, please define KERNEL_ROOT_DIR")
ENDIF(EXISTS ${KERNEL_ROOT_DIR})

# User options
# ============
#OPTION(SALOME_BUILD_TESTS "Build SALOME tests" ON)
OPTION(SALOME_BUILD_DOC "Generate SALOME <Name_of_Application> documentation" ON)

#IF(SALOME_BUILD_TESTS)
#  ENABLE_TESTING()
#ENDIF()

# KERNEL optionals:
IF(SALOME_BUILD_DOC)
  FIND_PACKAGE(SalomePython)
  FIND_PACKAGE(SalomeSphinx)
  SALOME_LOG_OPTIONAL_PACKAGE(Sphinx SALOME_BUILD_DOC)
ENDIF()

# Directories
# (default values taken from KERNEL)
# ===========
SET(SALOME_INSTALL_SCRIPT_PYTHON "${SALOME_INSTALL_SCRIPT_PYTHON}" CACHE PATH 
   "Install path: SALOME Python scripts")

# Specific to [NAME_OF_APPLICATION]:
SET(SALOME_[NAME_OF_APPLICATION]_INSTALL_RES_DATA "${SALOME_INSTALL_RES}/(name_of_application)" CACHE PATH 
    "Install path: SALOME PYHELLO specific data")

MARK_AS_ADVANCED(SALOME_INSTALL_SCRIPT_PYTHON)


# Sources 
# ========
ADD_SUBDIRECTORY( src )
ADD_SUBDIRECTORY( resources )
ADD_SUBDIRECTORY( doc )
