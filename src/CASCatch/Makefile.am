#  File   : Makefile.in
#  Author : Sergey RUIN (OCN)
#  Module : SALOME

include $(top_srcdir)/salome_adm/unix/make_common_starter.am


salomeinclude_HEADERS      = CASCatch_Failure.hxx \
                             CASCatch_CatchSignals.hxx \
                             CASCatch_ErrorHandler.hxx

# Libraries targets
lib_LTLIBRARIES         = libCASCatch.la 
libCASCatch_la_SOURCES  = CASCatch_Failure.cxx \
                          CASCatch_ErrorHandler.cxx \
                          CASCatch_CatchSignals.cxx

libCASCatch_la_LDFLAGS  = -no-undefined -version-info=0:0:0
libCASCatch_la_CPPFLAGS = @CAS_CPPFLAGS@
libCASCatch_la_CXXFLAGS = @CAS_CXXFLAGS@
libCASCatch_la_LIBADD   = @CAS_KERNEL@
