include $(top_srcdir)/salome_adm/unix/make_common_starter.am

#
# ===============================================================
# Files to be installed
# ===============================================================
#
# header files  
salomeinclude_HEADERS = SALOMEDS_Tool.hxx 

#
# ===============================================================
# Local definitions
# ===============================================================
#
LIB_SERVER_IDL = SALOMEDS.idl SALOMEDS_Attributes.idl SALOME_Exception.idl SALOME_GenericObj.idl

# Sources built from idl files
BUILT_SOURCES =\
	$(top_builddir)/idl/SALOMEDS.hh $(top_builddir)/idl/SALOMEDSSK.cc \
	$(top_builddir)/idl/SALOMEDS_Attributes.hh $(top_builddir)/idl/SALOMEDS_AttributesSK.cc \
	$(top_builddir)/idl/SALOME_GenericObj.hh $(top_builddir)/idl/SALOME_GenericObjSK.cc \
	$(top_builddir)/idl/SALOME_Exception.hh $(top_builddir)/idl/SALOME_ExceptionSK.cc

# This local variable defines the list of CPPFLAGS common to all target in this package.
COMMON_CPPFLAGS=\
	-I$(srcdir)/../Basics \
	-I$(srcdir)/../Utils \
	-I$(srcdir)/../SALOMELocalTrace \
	-I$(top_builddir)/salome_adm/unix \
	-I$(top_builddir)/idl \
	@CORBA_CXXFLAGS@ @CORBA_INCLUDES@ \
	@CAS_CPPFLAGS@ @CAS_CXXFLAGS@ \
	@HDF5_INCLUDES@ \
	@BOOST_CPPFLAGS@

# This local variable defines the list of dependant libraries common to all target in this package.
COMMON_LIBS =\
	../Utils/libOpUtil.la \
	@CAS_KERNEL@

#
# ===============================================================
# Libraries targets
# ===============================================================
#
lib_LTLIBRARIES = libTOOLSDS.la
libTOOLSDS_la_SOURCES = 	\
	SALOMEDS_Tool.cxx \
	$(BUILT_SOURCES)

libTOOLSDS_la_CPPFLAGS = $(COMMON_CPPFLAGS)
libTOOLSDS_la_LDFLAGS  = -no-undefined -version-info=0:0:0
libTOOLSDS_la_LIBADD    = $(COMMON_LIBS)

#
# ===============================================================
# Executables targets
# ===============================================================
#
# Nothing to build


