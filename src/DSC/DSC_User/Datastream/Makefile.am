lib_LTLIBRARIES = libSalomeDatastream.la

libSalomeDatastream_la_SOURCES        = fake.cc
#DatastreamException_cxx_CXXFLAGS               = -I@KERNEL_ROOT_DIR@/include/salome
#nodist_libSalomeDatastream_la_SOURCES = $(top_builddir)/idl/Datastream_PortsSK.cc
libSalomeDatastream_la_CXXFLAGS       = -I$(top_srcdir)/src/DSC_User 
#					         -I$(top_srcdir)/src/DSC_User/Datastream/Palm \
#				                 -I@KERNEL_ROOT_DIR@/include/salome \
#				                 -I$(top_builddir)/idl  \
#				                  @OMNIORB_INCLUDES@

libSalomeDatastream_la_LIBADD           = Palm/libSalomePalm.la Calcium/libSalomeCalcium.la
libSalomeDatastream_la_LDFLAGS          = -no-undefined -version-info 0:0:0
##################################################################################################################

include_HEADERS = GenericPort.hxx GenericUsesPort.hxx DataIdFilter.hxx CorbaTypeManipulator.hxx \
	CouplingPolicy.hxx DisplayPair.hxx \
	FindKeyPredicate.hxx AdjacentFunctor.hxx IteratorTraits.hxx ConstTraits.hxx \
	lambda.hpp DatastreamException.hxx

CLEANFILES = *.hh *SK.cc

SUBDIRS = Palm Calcium
