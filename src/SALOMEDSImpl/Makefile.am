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
	SALOMEDSImpl_Attributes.hxx \
	SALOMEDSImpl_SObject.hxx \
	SALOMEDSImpl_SComponent.hxx \
	SALOMEDSImpl_Driver.hxx \
	SALOMEDSImpl_Callback.hxx \
	SALOMEDSImpl_ChildIterator.hxx \
	SALOMEDSImpl_GenericAttribute.hxx \
	SALOMEDSImpl_AttributeIOR.hxx \
	SALOMEDSImpl_AttributeExternalFileDef.hxx \
	SALOMEDSImpl_AttributeFileType.hxx \
	SALOMEDSImpl_AttributePersistentRef.hxx \
	SALOMEDSImpl_AttributeSequenceOfReal.hxx \
	SALOMEDSImpl_AttributeSequenceOfInteger.hxx \
	SALOMEDSImpl_AttributeDrawable.hxx \
	SALOMEDSImpl_AttributeSelectable.hxx \
	SALOMEDSImpl_AttributeExpandable.hxx  \
	SALOMEDSImpl_AttributeOpened.hxx \
	SALOMEDSImpl_AttributeFlags.hxx \
	SALOMEDSImpl_AttributeGraphic.hxx \
	SALOMEDSImpl_AttributeTextColor.hxx \
	SALOMEDSImpl_AttributeTextHighlightColor.hxx \
	SALOMEDSImpl_AttributePixMap.hxx \
	SALOMEDSImpl_AttributeLocalID.hxx \
	SALOMEDSImpl_AttributeTarget.hxx \
	SALOMEDSImpl_AttributeTableOfInteger.hxx \
	SALOMEDSImpl_AttributeTableOfReal.hxx \
	SALOMEDSImpl_AttributeTableOfString.hxx \
	SALOMEDSImpl_AttributeStudyProperties.hxx \
	SALOMEDSImpl_AttributePythonObject.hxx \
	SALOMEDSImpl_AttributeReal.hxx \
	SALOMEDSImpl_AttributeInteger.hxx \
	SALOMEDSImpl_AttributeUserID.hxx \
	SALOMEDSImpl_AttributeTreeNode.hxx \
	SALOMEDSImpl_AttributeName.hxx \
	SALOMEDSImpl_AttributeComment.hxx \
	SALOMEDSImpl_AttributeReference.hxx \
	SALOMEDSImpl_UseCaseBuilder.hxx \
	SALOMEDSImpl_UseCaseIterator.hxx \
	SALOMEDSImpl_SComponentIterator.hxx \
	SALOMEDSImpl_StudyBuilder.hxx \
	SALOMEDSImpl_Study.hxx \
	SALOMEDSImpl_StudyManager.hxx \
	SALOMEDSImpl_OCAFApplication.hxx \
	SALOMEDSImpl_ChildNodeIterator.hxx

#
# ===============================================================
# Local definitions
# ===============================================================
#

# This local variable defines the list of CPPFLAGS common to all target in this package.
COMMON_CPPFLAGS=\
	@CAS_CPPFLAGS@ @CAS_CXXFLAGS@ \
	-I$(srcdir)/../HDFPersist \
	@HDF5_INCLUDES@

# This flag is used to resolve the dependencies of OCC libraries.
LDXMUFLAGS = -L/usr/X11R6/lib -lXmu

# This local variable defines the list of dependant libraries common to all target in this package.
COMMON_LIBS =\
	../HDFPersist/libSalomeHDFPersist.la \
	@CAS_LDPATH@ -lTKCAF -lTKBO -lTKLCAF -lTKMath -lTKStdSchema -lTKernel \
	@HDF5_LIBS@ \
	$(LDXMUFLAGS)


#
# ===============================================================
# Libraries targets
# ===============================================================
#
lib_LTLIBRARIES = libSalomeDSImpl.la
libSalomeDSImpl_la_SOURCES =\
		  SALOMEDSImpl_Tool.cxx \
	          SALOMEDSImpl_Callback.cxx \
		  SALOMEDSImpl_StudyHandle.cxx \
	          SALOMEDSImpl_GenericAttribute.cxx \
		  SALOMEDSImpl_SObject.cxx \
		  SALOMEDSImpl_SComponent.cxx \
		  SALOMEDSImpl_ChildIterator.cxx \
		  SALOMEDSImpl_OCAFApplication.cxx \
		  SALOMEDSImpl_AttributeIOR.cxx \
		  SALOMEDSImpl_AttributeExternalFileDef.cxx \
		  SALOMEDSImpl_AttributeFileType.cxx \
	          SALOMEDSImpl_AttributePersistentRef.cxx \
	          SALOMEDSImpl_AttributeSequenceOfReal.cxx \
	          SALOMEDSImpl_AttributeSequenceOfInteger.cxx \
	          SALOMEDSImpl_AttributeDrawable.cxx \
	          SALOMEDSImpl_AttributeSelectable.cxx \
	          SALOMEDSImpl_AttributeExpandable.cxx  \
	          SALOMEDSImpl_AttributeOpened.cxx \
	          SALOMEDSImpl_AttributeFlags.cxx \
	          SALOMEDSImpl_AttributeGraphic.cxx \
	          SALOMEDSImpl_AttributeTextColor.cxx \
	          SALOMEDSImpl_AttributeTextHighlightColor.cxx \
	          SALOMEDSImpl_AttributePixMap.cxx \
	          SALOMEDSImpl_AttributeLocalID.cxx \
	          SALOMEDSImpl_AttributeTarget.cxx \
	          SALOMEDSImpl_AttributeTableOfInteger.cxx \
	          SALOMEDSImpl_AttributeTableOfReal.cxx \
	          SALOMEDSImpl_AttributeTableOfString.cxx \
		  SALOMEDSImpl_AttributeStudyProperties.cxx \
		  SALOMEDSImpl_AttributePythonObject.cxx \
		  SALOMEDSImpl_AttributeReal.cxx \
		  SALOMEDSImpl_AttributeInteger.cxx \
		  SALOMEDSImpl_AttributeUserID.cxx \
		  SALOMEDSImpl_AttributeTreeNode.cxx \
		  SALOMEDSImpl_AttributeName.cxx \
		  SALOMEDSImpl_AttributeComment.cxx \
		  SALOMEDSImpl_AttributeReference.cxx \
		  SALOMEDSImpl_ChildNodeIterator.cxx \
		  SALOMEDSImpl_UseCaseBuilder.cxx \
		  SALOMEDSImpl_UseCaseIterator.cxx \
		  SALOMEDSImpl_SComponentIterator.cxx \
		  SALOMEDSImpl_StudyBuilder.cxx \
		  SALOMEDSImpl_Study.cxx \
		  SALOMEDSImpl_StudyManager.cxx

libSalomeDSImpl_la_CPPFLAGS = $(COMMON_CPPFLAGS)
libSalomeDSImpl_la_LDFLAGS  = -no-undefined -version-info=0:0:0
libSalomeDSImpl_la_LIBADD    = $(COMMON_LIBS)

#
# ===============================================================
# Executables targets
# ===============================================================
#
bin_PROGRAMS = testDS
testDS_SOURCES  = testDS.cxx
testDS_CPPFLAGS = $(COMMON_CPPFLAGS)
testDS_LDADD    = libSalomeDSImpl.la $(COMMON_LIBS)


g++ "-DPACKAGE_NAME=\"Salome2 Project\"" -DPACKAGE_TARNAME=\"salome\" -DPACKAGE_VERSION=\"3.1.0\" "-DPACKAGE_STRING=\"Salome2 Project 3.1.0\"" -DPACKAGE_BUGREPORT=\"gboulant@CS\" -DPACKAGE=\"salome\" -DVERSION=\"3.1.0\" -DSTDC_HEADERS=1 -DHAVE_SYS_TYPES_H=1 -DHAVE_SYS_STAT_H=1 -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_MEMORY_H=1 -DHAVE_STRINGS_H=1 -DHAVE_INTTYPES_H=1 -DHAVE_STDINT_H=1 -DHAVE_UNISTD_H=1 -DHAVE_DLFCN_H=1 -DHAVE_LIBDL=1 -DHAVE_LIBRT=1 -DHAVE_LIBM=1 -DHAVE_NAMESPACES= -D__x86__=1 -D__linux__=1 -D__OSVERSION__=1 -DOMNIORB=1 -DCORBA_HAVE_POA=1 -DCORBA_ORB_INIT_HAVE_3_ARGS=1 -DCORBA_ORB_INIT_THIRD_ARG=\"omniORB\" -DHAVE_GL_GL_H=1 -I. -I../../../KERNEL_SRC/src/SALOMEDSImpl -DOCC_VERSION_MAJOR=5 -DLIN -DLINTEL -DCSFDB -DNO_CXX_EXCEPTION -DNo_exception -DHAVE_CONFIG_H -DHAVE_LIMITS_H -DHAVE_WOK_CONFIG_H -I/local00/gboulant/salome/prerequis/occ524_patch/inc -I/local00/gboulant/salome/BR_auto_V310a2/build/include/salome -funsigned-char -I/local00/bourcier/TEST/SALOME2/prerequis/deb/hdf5-1.6.2/usr/include ../../../KERNEL_SRC/src/SALOMEDSImpl/../HDFPersist -DHAVE_SOCKET -pthread -g -D_DEBUG_ -Wno-deprecated -Wparentheses -Wreturn-type -Wunused -MT libSalomeDSImpl_la-SALOMEDSImpl_Tool.lo -MD -MP -MF .deps/libSalomeDSImpl_la-SALOMEDSImpl_Tool.Tpo -c ../../../KERNEL_SRC/src/SALOMEDSImpl/SALOMEDSImpl_Tool.cxx  -fPIC -DPIC -o .libs/libSalomeDSImpl_la-SALOMEDSImpl_Tool.o
