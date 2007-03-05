BUILT_SOURCES = SALOME_Component.hh SALOME_Exception.hh

%.hh : @KERNEL_ROOT_DIR@/idl/salome/%.idl
	@IDL@ @IDLGENFLAGS@ -I@KERNEL_ROOT_DIR@/idl/salome $<

AM_CXXFLAGS       = -I$(top_srcdir)/src/DSC_User \
		    -I$(top_srcdir)/src/DSC_User/Datastream \
		    -I$(top_srcdir)/src/DSC_Basic \
		    -I@KERNEL_ROOT_DIR@/include/salome \
		    -I$(top_builddir)/idl  \
		     @OMNIORB_INCLUDES@

noinst_LTLIBRARIES = libSalomeCalcium.la 
nodist_libSalomeCalcium_la_SOURCES = $(top_builddir)/idl/Calcium_PortsSK.cc
libSalomeCalcium_la_SOURCES  = calcium_port_factory.cxx \
				calcium_uses_port.cxx	\
				calcium_repository_types.cxx \
				CalciumCouplingPolicy.cxx \
				calcium_destructors_port_provides.cxx \
				calcium_destructors_port_uses.cxx
libSalomeCalcium_la_LDFLAGS = -L@KERNEL_ROOT_DIR@/lib/salome/ -lOpUtil

lib_LTLIBRARIES = libCalciumC.la
libCalciumC_la_SOURCES = Calcium.c 

## TESTS ##

noinst_PROGRAMS              = test_DataIdContainer
test_DataIdContainer_SOURCES = test_DataIdContainer.cxx 
test_DataIdContainer_LDADD   = libSalomeCalcium.la

## pas normal d'être obligé de linké à caus de __init___...thread
test_DataIdContainer_LDFLAGS = -L@KERNEL_ROOT_DIR@/lib/salome 
test_DataIdContainer_LDFLAGS += @OMNIORB_LIBS@ -lomniORB4 -lomniDynamic4 -lCOS4 -lCOSDynamic4 -lomnithread 

#noinst_PROGRAMS               += test_CalciumInterface
#test_CalciumInterface_SOURCES = test_CalciumInterface.cxx
#DSC_User = $(top_srcdir)/src/DSC_User
#test_CalciumInterface_CXXFLAGS = -I$(DSC_User)/Basic \
#				 -I$(DSC_User) \
#				 -I$(top_srcdir)/src/DSC_Basic \
#				 -I$(DSC_User)/Datastream  \
#				 -I$(DSC_User)/Datastream/Palm \
#				 -I$(DSC_User)/Datastream/Calcium \
#				 -I@KERNEL_ROOT_DIR@/include/salome \
#				 -I$(top_builddir)/idl  \
#				  @OMNIORB_INCLUDES@

## pas normal d'être obligé de linké à caus de __init___...thread
#test_CalciumInterface_LDFLAGS = -L@KERNEL_ROOT_DIR@/lib/salome 
#test_CalciumInterface_LDFLAGS +=  -lomniORB4 -lomniDynamic4 -lCOS4 -lCOSDynamic4 -lomnithread 
#test_CalciumInterface_LDADD   = -lOpUtil libSalomeCalcium.la $(DSC_User)/libSalomeDSCSuperv.la


TESTS                        = test_DataIdContainer 
#test_CalciumInterface


include_HEADERS = calcium_port_factory.hxx \
	CalciumGenericProvidesPort.hxx \
	calcium_uses_port.hxx CalciumGenericUsesPort.hxx \
	calcium_port_provides.hxx \
	calcium_integer_port_uses.hxx \
	calcium_real_port_uses.hxx \
	calcium_double_port_uses.hxx \
	calcium_string_port_uses.hxx \
	calcium_logical_port_uses.hxx \
	calcium_complex_port_uses.hxx \
	CalciumCouplingPolicy.hxx CalciumTypes.hxx CalciumInterface.hxx \
	Copy2UserSpace.hxx Copy2CorbaSpace.hxx CalciumPortTraits.hxx
include_HEADERS += calcium.h calciumP.h version.h calcium.hf
CLEANFILES = *.hh *SK.cc

