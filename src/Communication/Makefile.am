include $(top_srcdir)/salome_adm/unix/make_common_starter.am

#
# ===============================================================
# Files to be installed
# ===============================================================
#
# header files  
salomeinclude_HEADERS = \
	ReceiverFactory.hxx \
	SenderFactory.hxx \
	SALOMEMultiComm.hxx \
	MultiCommException.hxx \
	SALOME_Comm_i.hxx

# Scripts to be installed

#
# ===============================================================
# Local definitions
# ===============================================================
#
# Sources built from idl files
BUILT_SOURCES =\
	$(top_builddir)/idl/SALOME_Comm.hh $(top_builddir)/idl/SALOME_CommSK.cc \
	$(top_builddir)/idl/SALOME_Exception.hh $(top_builddir)/idl/SALOME_ExceptionSK.cc

# This local variable defines the list of CPPFLAGS common to all target in this package.
COMMON_CPPFLAGS=\
	-I$(srcdir)/../Basics \
	-I$(srcdir)/../SALOMELocalTrace \
	-I$(srcdir)/../Utils \
	-I$(top_builddir)/salome_adm/unix \
	-I$(top_builddir)/idl \
	@CORBA_CXXFLAGS@ @CORBA_INCLUDES@

# This local variable defines the list of dependant libraries common to all target in this package.
COMMON_LIBS = \
	../Utils/libOpUtil.la \
	../SALOMELocalTrace/libSALOMELocalTrace.la


# _CS_gbo The need for these flags depends on wether the swig
# interface is generated and if MPI is activated.
OPT_CPPFLAGS = @PYTHON_INCLUDES@  @MPI_INCLUDES@
OPT_LIBS     = @PYTHON_LIBS@ @MPI_LIBS@
OPT_LDFLAGS  = -Xlinker -export-dynamic

#
# ===============================================================
# Libraries targets
# ===============================================================
#
lib_LTLIBRARIES = libSalomeCommunication.la 
libSalomeCommunication_la_SOURCES =\
	SALOME_Comm_i.cxx \
	SenderFactory.cxx \
	MultiCommException.cxx \
	SALOMEMultiComm.cxx \
	ReceiverFactory.cxx \
	$(BUILT_SOURCES)

libSalomeCommunication_la_CPPFLAGS = $(COMMON_CPPFLAGS) $(OPT_CPPFLAGS)
libSalomeCommunication_la_LDFLAGS  = -no-undefined -version-info=0:0:0 $(OPT_LDFLAGS)
libSalomeCommunication_la_LIBADD    = $(COMMON_LIBS) $(OPT_LIBS)

#
# ===============================================================
# Executables targets
# ===============================================================
#
# Nothing to build
