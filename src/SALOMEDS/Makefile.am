#
#  File   : Makefile.am
#  Author : Guillaume Boulant (CSSI)
#  Module : KERNEL

include $(top_srcdir)/salome_adm/unix/make_common_starter.am

#
# ===============================================================
# Files to be installed
# ===============================================================
#
# header files  
salomeinclude_HEADERS= \
	SALOMEDS_StudyManager_i.hxx \
	SALOMEDS_Driver_i.hxx \
	SALOMEDS_StudyManager.hxx \
	SALOMEDS_Study_i.hxx \
	SALOMEDS_Study.hxx \
	SALOMEDS_SObject_i.hxx \
	SALOMEDS_SObject.hxx \
	SALOMEDS_SComponent_i.hxx \
	SALOMEDS_SComponent.hxx \
	SALOMEDS_GenericAttribute_i.hxx \
	SALOMEDS_GenericAttribute.hxx

# Scripts to be installed
salomescript_DATA = SALOME_DriverPy.py

#
# ===============================================================
# Local definitions
# ===============================================================
#

# Sources built from idl files
BUILT_SOURCES =\
	$(top_builddir)/idl/SALOMEDS.hh $(top_builddir)/idl/SALOMEDSSK.cc \
	$(top_builddir)/idl/SALOMEDS_Attributes.hh $(top_builddir)/idl/SALOMEDS_AttributesSK.cc \
	$(top_builddir)/idl/SALOME_GenericObj.hh $(top_builddir)/idl/SALOME_GenericObjSK.cc \
	$(top_builddir)/idl/SALOME_Component.hh $(top_builddir)/idl/SALOME_ComponentSK.cc
	$(top_builddir)/idl/SALOME_Exception.hh $(top_builddir)/idl/SALOME_ExceptionSK.cc

# This local variable defines the list of CPPFLAGS common to all target in this package.
COMMON_CPPFLAGS=\
	@CAS_CPPFLAGS@ @CAS_CXXFLAGS@ \
	@BOOST_CPPFLAGS@ \
	-I$(srcdir)/../HDFPersist \
	@HDF5_INCLUDES@ \
	-I$(srcdir)/../Basics \
	-I$(srcdir)/../SALOMELocalTrace \
	-I$(srcdir)/../Utils \
	-I$(srcdir)/../SALOMEDSImpl \
	-I$(srcdir)/../NamingService \
	-I$(srcdir)/../GenericObj \
	-I$(srcdir)/../SALOMEDSClient \
	-I$(srcdir)/../LifeCycleCORBA \
	-I$(top_builddir)/salome_adm/unix \
	-I$(top_builddir)/idl \
	@CORBA_CXXFLAGS@ @CORBA_INCLUDES@

# This flag is used to resolve the dependencies of OCC libraries.
LDXMUFLAGS = -L/usr/X11R6/lib -lXmu

# This local variable defines the list of dependant libraries common to all target in this package.
COMMON_LIBS =\
	../TOOLSDS/libTOOLSDS.la \
	../NamingService/libSalomeNS.la \
	../Utils/libOpUtil.la \
	../SALOMELocalTrace/libSALOMELocalTrace.la \
	../Basics/libSALOMEBasics.la \
	../HDFPersist/libSalomeHDFPersist.la \
	../SALOMEDSImpl/libSalomeDSImpl.la \
	../GenericObj/libSalomeGenericObj.la \
	../LifeCycleCORBA/libSalomeLifeCycleCORBA.la \
	@CAS_LDPATH@ -lTKernel -lTKCAF -lTKBO -lTKStdSchema \
	@HDF5_LIBS@ \
	$(LDXMUFLAGS)


#LDFLAGS+=  -lSalomeGenericObj -lSalomeLifeCycleCORBA

#
# ===============================================================
# Libraries targets
# ===============================================================
#
lib_LTLIBRARIES = libSalomeDS.la
libSalomeDS_la_SOURCES = 	\
		  SALOMEDS.cxx \
		  SALOMEDS_Driver_i.cxx \
		  SALOMEDS_StudyManager_i.cxx \
		  SALOMEDS_UseCaseBuilder_i.cxx \
		  SALOMEDS_UseCaseIterator_i.cxx \
		  SALOMEDS_ChildIterator_i.cxx \
		  SALOMEDS_SComponentIterator_i.cxx \
		  SALOMEDS_Study_i.cxx \
		  SALOMEDS_StudyBuilder_i.cxx \
		  SALOMEDS_SObject_i.cxx \
		  SALOMEDS_SComponent_i.cxx \
		  SALOMEDS_GenericAttribute_i.cxx \
	          SALOMEDS_AttributeComment_i.cxx \
	          SALOMEDS_AttributeExternalFileDef_i.cxx \
	          SALOMEDS_AttributeFileType_i.cxx \
	          SALOMEDS_AttributeIOR_i.cxx \
	          SALOMEDS_AttributeInteger_i.cxx \
	          SALOMEDS_AttributeName_i.cxx \
	          SALOMEDS_AttributePersistentRef_i.cxx \
	          SALOMEDS_AttributeReal_i.cxx \
	          SALOMEDS_AttributeSequenceOfReal_i.cxx \
	          SALOMEDS_AttributeSequenceOfInteger_i.cxx \
	          SALOMEDS_AttributeDrawable_i.cxx \
	          SALOMEDS_AttributeSelectable_i.cxx \
	          SALOMEDS_AttributeOpened_i.cxx \
	          SALOMEDS_AttributeFlags_i.cxx \
	          SALOMEDS_AttributeGraphic_i.cxx \
	          SALOMEDS_AttributeExpandable_i.cxx \
	          SALOMEDS_AttributeTextColor_i.cxx  \
	          SALOMEDS_AttributeTextHighlightColor_i.cxx  \
	          SALOMEDS_AttributePixMap_i.cxx  \
		  SALOMEDS_AttributeTreeNode_i.cxx \
		  SALOMEDS_AttributeLocalID_i.cxx \
	          SALOMEDS_AttributeUserID_i.cxx \
	          SALOMEDS_AttributeTarget_i.cxx \
	          SALOMEDS_AttributeTableOfInteger_i.cxx \
	          SALOMEDS_AttributeTableOfReal_i.cxx \
		  SALOMEDS_AttributeTableOfString_i.cxx \
		  SALOMEDS_AttributeStudyProperties_i.cxx \
		  SALOMEDS_AttributePythonObject_i.cxx \
		  SALOMEDS_SObject.cxx \
		  SALOMEDS_SComponent.cxx \
		  SALOMEDS_GenericAttribute.cxx \
		  SALOMEDS_ChildIterator.cxx \
		  SALOMEDS_SComponentIterator.cxx \
		  SALOMEDS_UseCaseIterator.cxx \
		  SALOMEDS_UseCaseBuilder.cxx \
		  SALOMEDS_StudyBuilder.cxx \
		  SALOMEDS_Study.cxx \
		  SALOMEDS_StudyManager.cxx \
		  SALOMEDS_AttributeStudyProperties.cxx \
		  SALOMEDS_AttributeComment.cxx \
		  SALOMEDS_AttributeDrawable.cxx \
		  SALOMEDS_AttributeExpandable.cxx \
		  SALOMEDS_AttributeExternalFileDef.cxx \
		  SALOMEDS_AttributeFileType.cxx \
		  SALOMEDS_AttributeFlags.cxx \
		  SALOMEDS_AttributeGraphic.cxx \
		  SALOMEDS_AttributeIOR.cxx \
		  SALOMEDS_AttributeInteger.cxx \
		  SALOMEDS_AttributeLocalID.cxx \
		  SALOMEDS_AttributeName.cxx \
		  SALOMEDS_AttributeOpened.cxx \
		  SALOMEDS_AttributePythonObject.cxx \
		  SALOMEDS_AttributeReal.cxx \
		  SALOMEDS_AttributeSelectable.cxx \
		  SALOMEDS_AttributeSequenceOfInteger.cxx \
		  SALOMEDS_AttributePersistentRef.cxx \
		  SALOMEDS_AttributePixMap.cxx \
		  SALOMEDS_AttributeSequenceOfReal.cxx \
		  SALOMEDS_AttributeTableOfInteger.cxx \
		  SALOMEDS_AttributeTableOfReal.cxx \
		  SALOMEDS_AttributeTableOfString.cxx \
		  SALOMEDS_AttributeTarget.cxx \
		  SALOMEDS_AttributeTextColor.cxx \
		  SALOMEDS_AttributeTextHighlightColor.cxx \
		  SALOMEDS_AttributeTreeNode.cxx \
		  SALOMEDS_AttributeUserID.cxx \
		  $(BUILT_SOURCES)

libSalomeDS_la_CPPFLAGS = $(COMMON_CPPFLAGS)
libSalomeDS_la_LDFLAGS  = -no-undefined -version-info=0:0:0
libSalomeDS_la_LIBADD    = $(COMMON_LIBS)

#
# ===============================================================
# Executables targets
# ===============================================================
#
bin_PROGRAMS = SALOMEDS_Server SALOMEDS_Client

SALOMEDS_Server_SOURCES  = SALOMEDS_Server.cxx
SALOMEDS_Server_CPPFLAGS = $(COMMON_CPPFLAGS)
SALOMEDS_Server_LDADD    = \
	libSalomeDS.la $(COMMON_LIBS) \
	../ResourcesManager/libSalomeResourcesManager.la \
	-lTKLCAF -lTKMath \
	@CORBA_LIBS@

SALOMEDS_Client_SOURCES  = SALOMEDS_Client.cxx
SALOMEDS_Client_CPPFLAGS = $(COMMON_CPPFLAGS)
SALOMEDS_Client_LDADD    = \
	libSalomeDS.la $(COMMON_LIBS) \
	../ResourcesManager/libSalomeResourcesManager.la \
	-lTKLCAF -lTKMath \
	@CORBA_LIBS@


################
#LDFLAGSFORBIN= $(LDFLAGS)  -lRegistry -lSalomeNotification -lSalomeContainer -lSalomeResourcesManager -lSALOMEBasics

