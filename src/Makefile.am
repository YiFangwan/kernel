#  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
#  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 
#
#
#
#  File   : Makefile.am
#  Author : Guillaume Boulant (CSSI)
#  Module : KERNEL
# $Header$

SUBDIRS = \
 Basics \
 SALOMELocalTrace \
 CASCatch \
 HDFPersist \
 Logger \
 SALOMETraceCollector \
 Utils \
 NamingService \
 GenericObj \
 Registry \
 ModuleCatalog \
 DataTypeCatalog \
 RessourcesCatalog \
 ResourcesManager \
 Notification \
 NOTIFICATION_SWIG \
 Container \
 TestContainer \
 LifeCycleCORBA \
 LifeCycleCORBA_SWIG \
 SALOMEDSClient \
 TOOLSDS \
 SALOMEDSImpl \
 SALOMEDS \
 KERNEL_PY \
 ModuleGenerator \
 Communication 

#
# The following packages are built under conditions.
# A condition is defined using the macro AM_CONDITIONAL (see the
# check_cppunit for example with CPPUNIT_IS_OK.
#

if CPPUNIT_IS_OK
 SUBDIRS+= \
 Basics/Test \
 SALOMELocalTrace/Test \
 Logger/Test \
 SALOMETraceCollector/Test \
 Utils/Test \
 LifeCycleCORBA/Test \
 UnitTests
endif


if MPI_IS_OK
  SUBDIRS+= MPIContainer TestMPIContainer
endif

if WITH_BATCH
  SUBDIRS += Batch Batch_SWIG
endif


