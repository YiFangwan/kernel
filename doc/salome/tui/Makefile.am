# -* Makefile *- 
#
# Author : Paul RASCLE (EDF)
# Date : 16/02/2006
# $Header$
#

include $(top_srcdir)/salome_adm/unix/make_common_starter.am

userdoc:
	cp -fr $(srcdir)/KERNEL ./INPUT; \
	cd INPUT; \
	sed 's|../../../share/salome|@top_srcdir@|' ./doxyfile > ./doxyfile1; \
	mv -f doxyfile1 doxyfile; \
	$(DOXYGEN) ./doxyfile; \
	cd ..;
	$(INSTALL) -d $(docdir)/tui/KERNEL; 
	cp -fr $(srcdir)/KERNEL/sources/static/*.* $(docdir)/tui/KERNEL;
	cp -fr $(srcdir)/KERNEL/sources/ $(docdir)/tui/KERNEL;
	cp -fr $(srcdir)/KERNEL/HTML/ $(docdir)/tui/KERNEL;
	cp -f $(srcdir)/pythfilter.py $(docdir)/tui/KERNEL;

