lib_LTLIBRARIES = libSalomeDSCSupervBasic.la

libSalomeDSCSupervBasic_la_SOURCES  = basic_port_factory.cxx data_short_port_uses.cxx data_short_port_provides.cxx 
libSalomeDSCSupervBasic_la_CXXFLAGS = -I$(top_srcdir)/src/DSC_User \
				      -I$(top_srcdir)/src/DSC_Basic\
				      -I$(top_builddir)/src/DSC_User \
				      -I@KERNEL_ROOT_DIR@/include/salome \
				      -I$(top_builddir)/idl  \
				      @OMNIORB_INCLUDES@ 

##################################################################################################################

include_HEADERS = basic_port_factory.hxx data_short_port_provides.hxx data_short_port_uses.hxx

CLEANFILES = *.hh *SK.cc
