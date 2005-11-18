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

# Program files
bin_PROGRAMS = SALOME_Logger_Server
SALOME_Logger_Server_SOURCES = SALOME_Logger_Server_main.cxx
SALOME_Logger_Server_LDADD   = libSalomeLoggerServer.la

# Libraries targets
lib_LTLIBRARIES = libSalomeLoggerServer.la
libSalomeLoggerServer_la_SOURCES =\
	SALOME_Trace.cxx \
	SALOME_Logger_Server.cxx \
	Logger.hh LoggerSK.cc
libSalomeLoggerServer_la_LDFLAGS  = -no-undefined -version-info=0:0:0

BUILT_SOURCES = Logger.hh LoggerSK.cc

# These variables defines the building process of CORBA files
OMNIORB_IDL         = @OMNIORB_IDL@
OMNIORB_IDLCXXFLAGS = @OMNIORB_IDLCXXFLAGS@
OMNIORB_IDLPYFLAGS  = @OMNIORB_IDLPYFLAGS@ -I$(top_builddir)/salome/idl
IDLCXXFLAGS = -bcxx @IDLCXXFLAGS@ -I$(top_builddir)/salome/idl

Logger.hh LoggerSK.cc: $(top_srcdir)/idl/Logger.idl
	$(OMNIORB_IDL) $(IDLCXXFLAGS) $(OMNIORB_IDLCXXFLAGS) $<

#LIB_SERVER_IDL = Logger.idl 

#salomedatadir   = 
#salomedata_DATA = SALOME_Trace.py

# Executables targets
#bin_SCRIPTS = SALOME_Trace.py
#bin_PROGRAMS = SALOME_Logger_Server

#EXPORT_PYSCRIPTS = SALOME_Trace.py


#BIN_SERVER_IDL = Logger.idl 
#BIN_CLIENT_IDL = Logger.idl
