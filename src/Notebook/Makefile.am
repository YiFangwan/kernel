#  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
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
#  File   : Makefile.am
#  Author : Alexandre SOLOVYOV
#  Module : KERNEL
#
include $(top_srcdir)/salome_adm/unix/make_common_starter.am

#
# ===============================================================
# Files to be installed
# ===============================================================
#
# header files  
salomeinclude_HEADERS = \
    SALOME_Parameter.hxx   \
    SALOME_Notebook.hxx    \
    SALOME_NotebookDriver.hxx \
    SALOME_Eval.hxx        \
    SALOME_EvalExpr.hxx    \
    SALOME_EvalParser.hxx  \
    SALOME_EvalSet.hxx     \
    SALOME_EvalVariant.hxx \
    SALOME_ParameterizedObject.hxx
    
# Scripts to be installed
dist_salomescript_PYTHON=

#
# ===============================================================
# Local definitions
# ===============================================================
#

# This local variable defines the list of CPPFLAGS common to all target in this package.
COMMON_CPPFLAGS = \
	-I$(srcdir)/../GenericObj \
	-I$(srcdir)/../SALOMELocalTrace \
	-I$(srcdir)/../Basics \
	-I$(srcdir)/../Utils \
	-I$(srcdir)/../TOOLSDS \
	-I$(top_builddir)/idl \
	@CORBA_CXXFLAGS@ @CORBA_INCLUDES@

# This local variable defines the list of dependant libraries common to all target in this package.
COMMON_LIBS = ../TOOLSDS/libTOOLSDS.la


#LDFLAGS+=  -lSalomeGenericObj -lSalomeLifeCycleCORBA

#
# ===============================================================
# Libraries targets
# ===============================================================
#
lib_LTLIBRARIES = libSalomeNotebook.la
libSalomeNotebook_la_SOURCES = \
    SALOME_Parameter.cxx \
    SALOME_Notebook.cxx  \
    SALOME_NotebookDriver.cxx \
    SALOME_EvalExpr.cxx  \
    SALOME_EvalParser.cxx  \
    SALOME_EvalSet.cxx  \
    SALOME_EvalVariant.cxx \
    SALOME_ParameterizedObject.cxx

libSalomeNotebook_la_CPPFLAGS = $(COMMON_CPPFLAGS)
libSalomeNotebook_la_LDFLAGS  = -Wl,-E -no-undefined -version-info=0:0:0 @LDEXPDYNFLAGS@
libSalomeNotebook_la_LIBADD    = $(COMMON_LIBS)
