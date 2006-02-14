#  SALOME NamingService : wrapping NamingService services
#
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
#  $Header$

include $(top_srcdir)/salome_adm/unix/make_common_starter.am

# header files  
salomeinclude_HEADERS =\
	SALOME_NamingService.hxx \
	ServiceUnreachable.hxx \
	NamingService_WaitForServerReadiness.hxx

# Scripts to be exported
dist_salomescript_DATA =\
	SALOME_NamingServicePy.py


# The IDL_SOURCES defines the list of files generated from the idl
# interface (SALOME_Exception.idl in this case).
#IDL_SOURCES = $(top_builddir)/idl/SALOME_Exception.hh $(top_builddir)/idl/SALOME_ExceptionSK.cc


# Libraries targets
lib_LTLIBRARIES        = libSalomeNS.la
libSalomeNS_la_SOURCES =\
	SALOME_NamingService.cxx \
	ServiceUnreachable.cxx \
	NamingService_WaitForServerReadiness.cxx
#	$(IDL_SOURCES)
# _CS_gbo We don't need to add the IDL_SOURCES files because the
# symbols needed from this sources are given by the linked libraries
# (libOptUtil.la). Meanwhile, the symbols could be duplicated here and
# no error would occur. An other reason to add it is that the
# dependencies could lose the symbols needed

libSalomeNS_la_LDFLAGS  = -no-undefined -version-info=0:0:0

libSalomeNS_la_CPPFLAGS = \
	-I$(srcdir)/../SALOMELocalTrace \
	-I$(srcdir)/../Basics \
	-I$(srcdir)/../Utils \
	-I$(top_builddir)/salome_adm/unix \
	-I$(top_builddir)/idl \
	@CORBA_CXXFLAGS@ @CORBA_INCLUDES@

libSalomeNS_la_LIBADD = ../Utils/libOpUtil.la
