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

ADD_SUBDIRECTORY(Basics)
ADD_SUBDIRECTORY(SALOMELocalTrace)
ADD_SUBDIRECTORY(HDFPersist)
ADD_SUBDIRECTORY(Utils)

IF(NOT SALOME_LIGHT_ONLY)
  ADD_SUBDIRECTORY(AppQuickStart)
  ADD_SUBDIRECTORY(KERNEL_PY)
  ADD_SUBDIRECTORY(DF)
  ADD_SUBDIRECTORY(Logger)
  ADD_SUBDIRECTORY(SALOMETraceCollector)
  ADD_SUBDIRECTORY(NamingService)
  ADD_SUBDIRECTORY(GenericObj)
  ADD_SUBDIRECTORY(Registry)
  ADD_SUBDIRECTORY(ModuleCatalog)
  ADD_SUBDIRECTORY(ResourcesManager)
  ADD_SUBDIRECTORY(Notification)
  ADD_SUBDIRECTORY(NOTIFICATION_SWIG)
  ADD_SUBDIRECTORY(Container)
  ADD_SUBDIRECTORY(SALOMESDS)
  ADD_SUBDIRECTORY(Launcher)
  ADD_SUBDIRECTORY(Launcher_SWIG)
  ADD_SUBDIRECTORY(LifeCycleCORBA)
  ADD_SUBDIRECTORY(LifeCycleCORBA_SWIG)
  ADD_SUBDIRECTORY(SALOMEDSClient)
  ADD_SUBDIRECTORY(TOOLSDS)
  ADD_SUBDIRECTORY(KernelHelpers)
  ADD_SUBDIRECTORY(SALOMEDSImpl)
  ADD_SUBDIRECTORY(SALOMEDS)
  ADD_SUBDIRECTORY(ModuleGenerator)
  ADD_SUBDIRECTORY(Communication)
  ADD_SUBDIRECTORY(Communication_SWIG)
  ADD_SUBDIRECTORY(DSC)

  IF(SALOME_BUILD_TESTS)
    ADD_SUBDIRECTORY(TestContainer)
  ENDIF()

  IF(SALOME_USE_MPI)
    ADD_SUBDIRECTORY(MPIContainer)
    IF(SALOME_BUILD_TESTS)
      ADD_SUBDIRECTORY(TestMPIContainer)
    ENDIF()
  ENDIF()

  IF(SALOME_PACO_PARALLEL)
    ADD_SUBDIRECTORY(ParallelContainer)
  ENDIF()
ENDIF()

ADD_SUBDIRECTORY(UnitTests)
