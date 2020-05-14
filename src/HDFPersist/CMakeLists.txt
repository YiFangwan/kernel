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
  ${HDF5_INCLUDE_DIRS}
  ${MPI_INCLUDE_DIRS}
  ${CMAKE_SOURCE_DIR}/src/Basics
)

ADD_DEFINITIONS(${HDF5_DEFINITIONS})

SET(_link_LIBRARIES
  ${PLATFORM_LIBS}
  ${HDF5_LIBRARIES}
  ${MPI_LIBRARIES}
  SALOMEBasics
)

SET(SalomeHDFPersist_SOURCES
  HDFarrayClose.c
  HDFarrayCreate.c
  HDFarrayGetDim.c
  HDFarrayGetType.c
  HDFarrayGetTypeId.c
  HDFarrayGetnDim.c
  HDFattrClose.c
  HDFattrCreate.c
  HDFattrGetSize.c
  HDFattrGetType.c
  HDFattrOpen.c
  HDFattrRead.c
  HDFattrWrite.c
  HDFdatasetClose.c
  HDFdatasetCreate.c
  HDFdatasetGetDim.c
  HDFdatasetGetOrder.c
  HDFdatasetGetSize.c
  HDFdatasetGetType.c
  HDFdatasetGetnDim.c
  HDFdatasetOpen.c
  HDFdatasetRead.c
  HDFdatasetWrite.c
  HDFerrorModeLock.c
  HDFfileClose.c
  HDFfileCreate.c
  HDFfileMount.c
  HDFfileOpen.c
  HDFfileUmount.c
  HDFgroupClose.c
  HDFgroupCreate.c
  HDFgroupOpen.c
  HDFnObjects.c
  HDFobjectIdentify.c
  HDFobjectType.c

  HDFarray.cc
  HDFascii.cc
  HDFattribute.cc
  HDFcontainerObject.cc
  HDFconvert.cc
  HDFdataset.cc
  HDFexplorer.cc
  HDFfile.cc
  HDFgroup.cc
  HDFinternalObject.cc
  HDFobject.cc
)

SET(SalomeHDFPersist_HEADERS
  HDFOI.hxx
  HDFarray.hxx
  HDFascii.hxx
  HDFattribute.hxx
  HDFcontainerObject.hxx
  HDFconvert.hxx
  HDFdataset.hxx
  HDFexception.hxx
  HDFexplorer.hxx
  HDFexport.hxx
  HDFfile.hxx
  HDFgroup.hxx
  HDFinternalObject.hxx
  HDFobject.hxx
  HDFtypes.h
  hdfi.h
)

ADD_LIBRARY(SalomeHDFPersist ${SalomeHDFPersist_SOURCES})
TARGET_LINK_LIBRARIES(SalomeHDFPersist ${_link_LIBRARIES})
INSTALL(TARGETS SalomeHDFPersist EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

INSTALL(FILES ${SalomeHDFPersist_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})

UNSET(SalomeHDFPersist_SOURCES)
UNSET(SalomeHDFPersist_HEADERS)
UNSET(_link_LIBRARIES)
