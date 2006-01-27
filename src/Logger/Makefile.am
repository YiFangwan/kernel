#  SALOME Logger : CORBA server managing trace output
#
#  Copyright (C) 2003  CEA/DEN, EDF R&D
#
#  File   : Makefile.am
#  Author : Guillaume BOULANT, CSSI
#  Module : KERNEL
#  $Header$

include $(top_srcdir)/salome_adm/unix/make_common_starter.am

# header files  
salomeinclude_HEADERS = \
	SALOME_Trace.hxx

#
# This makefile defines a executable target (SALOME_Logger_Server) and
# a library target (libSalomeLoggerServer.la). The executable depends
# on the library.
#

# Program files
bin_PROGRAMS = SALOME_Logger_Server
SALOME_Logger_Server_SOURCES = SALOME_Logger_Server_main.cxx
SALOME_Logger_Server_LDADD   = libSalomeLoggerServer.la
SALOME_Logger_Server_CPPFLAGS =\
	-I$(top_builddir)/salome_adm/unix \
	-I$(top_builddir)/idl \
	@CORBA_CXXFLAGS@ @CORBA_INCLUDES@

# The BUILT_SOURCES defines the list of files generated from the idl
# interface (Logger.idl in this case).
BUILT_SOURCES = $(top_builddir)/idl/Logger.hh $(top_builddir)/idl/LoggerSK.cc

# Libraries targets
lib_LTLIBRARIES = libSalomeLoggerServer.la
libSalomeLoggerServer_la_SOURCES =\
	SALOME_Trace.cxx \
	SALOME_Logger_Server.cxx \
	$(BUILT_SOURCES)

libSalomeLoggerServer_la_CPPFLAGS =\
	-I$(top_builddir)/salome_adm/unix \
	-I$(top_builddir)/idl \
	@CORBA_CXXFLAGS@ @CORBA_INCLUDES@
libSalomeLoggerServer_la_LDFLAGS = -no-undefined -version-info=0:0:0

libSalomeLoggerServer_la_LIBADD  = @CORBA_LIBS@

# Scripts target
salomescript_DATA = SALOME_Trace.py


# _CS_gbo For a better management of dependencies, we should define a
# building target linking the generated files to idl source files in
# this directory (for a future version)
# These variables defines the building process of CORBA files
#OMNIORB_IDL         = @OMNIORB_IDL@
#OMNIORB_IDLCXXFLAGS = @OMNIORB_IDLCXXFLAGS@
#OMNIORB_IDLPYFLAGS  = @OMNIORB_IDLPYFLAGS@ -I$(top_builddir)/salome/idl
#IDLCXXFLAGS = -bcxx @IDLCXXFLAGS@ -I$(top_builddir)/salome/idl

#Logger.hh LoggerSK.cc: $(top_srcdir)/idl/Logger.idl
#	$(OMNIORB_IDL) $(IDLCXXFLAGS) $(OMNIORB_IDLCXXFLAGS) $<

#LIB_SERVER_IDL = Logger.idl 


# Executables targets
#bin_SCRIPTS = SALOME_Trace.py
#bin_PROGRAMS = SALOME_Logger_Server

#EXPORT_PYSCRIPTS = SALOME_Trace.py


#BIN_SERVER_IDL = Logger.idl 
#BIN_CLIENT_IDL = Logger.idl
