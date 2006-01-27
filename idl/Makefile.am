
#
# This Makefile is responsible of generating the client and server
# implementation of IDL interfaces for both C++ and python usage.
# The building process of the C++ files is in charge of each source
# package and then is not manage here.
#

include $(top_srcdir)/salome_adm/unix/make_common_starter.am

IDL_FILES = \
  SALOME_Exception.idl \
  SALOME_Comm.idl \
  SALOME_ModuleCatalog.idl \
  SALOME_DataTypeCatalog.idl \
  SALOME_RessourcesCatalog.idl \
  SALOMEDS.idl \
  SALOMEDS_Attributes.idl \
  SALOME_Component.idl \
  SALOME_ContainerManager.idl \
  SALOME_TestComponent.idl \
  SALOME_Registry.idl \
  SALOME_MPIObject.idl \
  SALOME_MPIContainer.idl \
  Logger.idl \
  SALOME_GenericObj.idl \
  SALOME_TestModuleCatalog.idl

# This variable defines the files to be installed
salomeidl_DATA = $(IDL_FILES)

# These variables defines the file extentions for clt and srv CORBA
# files generated from the IDL files.
OMNIORB_IDL_CLN_H   = @OMNIORB_IDL_CLN_H@
OMNIORB_IDL_CLN_CXX = @OMNIORB_IDL_CLN_CXX@
OMNIORB_IDL_CLN_OBJ = @OMNIORB_IDL_CLN_OBJ@

OMNIORB_IDL_SRV_H   = @OMNIORB_IDL_SRV_H@
OMNIORB_IDL_SRV_CXX = @OMNIORB_IDL_SRV_CXX@
OMNIORB_IDL_SRV_OBJ = @OMNIORB_IDL_SRV_OBJ@

# These variables defines the building process of CORBA files
OMNIORB_IDL         = @OMNIORB_IDL@
OMNIORB_IDLCXXFLAGS = @OMNIORB_IDLCXXFLAGS@
OMNIORB_IDLPYFLAGS  = @OMNIORB_IDLPYFLAGS@ -I$(top_builddir)/salome/idl
IDLCXXFLAGS = -bcxx @IDLCXXFLAGS@ -I$(top_builddir)/salome/idl
IDLPYFLAGS  = @IDLPYFLAGS@


# Target for generating the stub from the IDL files. The stubs are
# created in the current build directory.
%$(OMNIORB_IDL_CLN_CXX) %$(OMNIORB_IDL_CLN_H): ${top_srcdir}/idl/%.idl
	$(OMNIORB_IDL) $(IDLCXXFLAGS) $(OMNIORB_IDLCXXFLAGS) $<


all: $(IDL_FILES:%.idl=%$(OMNIORB_IDL_CLN_CXX)) $(IDL_FILES:%.idl=%$(OMNIORB_IDL_CLN_H))

# This target overload the standard definition of install. The target
# install-pyidl is added to process python corba stubs for SALOME.
install: install-am install-pyidl

install-pyidl: $(IDL_FILES:%=$(top_srcdir)/idl/%)
	$(INSTALL) -d  $(pkgpythondir)
	ls $^ | while read file; do \
	  $(OMNIORB_IDL) $(IDLPYFLAGS) -C$(pkgpythondir) $$file ; \
	done
