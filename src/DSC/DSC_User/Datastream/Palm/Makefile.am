AM_CXXFLAGS       = -I$(top_srcdir)/src/DSC_User \
		    -I$(top_srcdir)/src/DSC_User/Datastream \
		    -I$(top_builddir)/src/DSC_User \
		    -I$(top_srcdir)/src/DSC_Basic \
		    -I@KERNEL_ROOT_DIR@/include/salome \
		    -I$(top_builddir)/idl  \
		    @OMNIORB_INCLUDES@ \
		    -I$(top_srcdir)/src/DSC_User/Datastream/Calcium

BUILT_SOURCES = SALOME_Component.hh SALOME_Exception.hh

%.hh : @KERNEL_ROOT_DIR@/idl/salome/%.idl
	@IDL@ @IDLGENFLAGS@ -I@KERNEL_ROOT_DIR@/idl/salome $<

noinst_LTLIBRARIES = libSalomePalm.la

libSalomePalm_la_SOURCES        = palm_port_factory.cxx 
nodist_libSalomePalm_la_SOURCES = $(top_builddir)/idl/Palm_PortsSK.cc
libSalomePalm_la_CXXFLAGS = -I$(top_srcdir)/src/DSC_User \
			    -I$(top_srcdir)/src/DSC_User/Datastream \
			    -I$(top_builddir)/src/DSC_User \
			    -I$(top_srcdir)/src/DSC_Basic \
			    -I@KERNEL_ROOT_DIR@/include/salome \
			    -I$(top_builddir)/idl  \
			    @OMNIORB_INCLUDES@ \
			    -I$(top_srcdir)/src/DSC_User/Datastream/Calcium


## Réussir la déclaration forward ds ProcessTimeInterval.hxx afin d'enlever la ligne précédente

noinst_PROGRAMS              = test_DataIdContainer
test_DataIdContainer_SOURCES = test_DataIdContainer.cxx
## pas normal d'être obligé de linké à caus de __init___...thread
test_DataIdContainer_LDFLAGS = -L@KERNEL_ROOT_DIR@/lib/salome 
#-L@PARALLEL_KERNEL_ROOT_DIR@/lib
test_DataIdContainer_LDFLAGS +=  @OMNIORB_LIBS@ -lomniORB4 -lomniDynamic4 -lCOS4 -lCOSDynamic4 -lomnithread 
##\
##			       -lSalomeDSCContainer -lSalomeDSCSuperv -lSalomeDSCSupervDatastream -lSalomeDSCSupervBasic

TESTS                        = test_DataIdContainer
##test_DataIdContainer_CXXFLAGS = $(AM_CXXFLAGS) -I$(top_srcdir)/src/DSC_User/Basic \
##				 -I$(top_srcdir)/src/DSC
##################################################################################################################

include_HEADERS = palm_port_factory.hxx palm_data_short_port_provides.hxx \
	palm_data_seq_short_port_provides.hxx PalmCouplingPolicy.hxx

CLEANFILES = *.hh *SK.cc

