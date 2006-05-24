# -* Makefile *- 
#
# Author : Paul RASCLE (EDF)
# Date : 16/02/2006
# $Header$
#

include $(top_srcdir)/salome_adm/unix/make_common_starter.am

EXTRA_DIST = $(srcdir)/KERNEL

dist-hook:
	rm -rf `find $(distdir) -name CVS`

userdoc:
	cp -fr $(srcdir)/KERNEL ./INPUT; \
	cp -f $(top_builddir)/doc/salome/tui/KERNEL/doxyuser ./INPUT; \
	cd ./INPUT; \
        pwd; ls;\
	echo "DOXYGEN SUPPORT PYTHON - $(DOXYGEN_WITH_PYTHON)"; \
	if( test "x$(DOXYGEN_WITH_PYTHON)" = "xyes"); then \
	  sed 's|python_extension_must_be_here|*.py|' ./doxyuser > ./doxyuser1; \
	  $(DOXYGEN) -u ./doxyuser1; \
	else \
	  sed 's|python_extension_must_be_here||' ./doxyuser > ./doxyuser1; \
	fi; \
	echo "DOXYGEN SUPPORT STL - $(DOXYGEN_WITH_STL)"; \
	if( test "x$(DOXYGEN_WITH_STL)" = "xyes"); then \
	  sed -e 's|BUILTIN_STL_SUPPORT    = NO|BUILTIN_STL_SUPPORT    = YES|' ./doxyuser1 > ./doxyuser2; \
	  mv -f doxyuser2 doxyuser1; \
	fi; \
	$(DOXYGEN) ./doxyuser1; \
	cd ..;
	$(INSTALL) -d $(docdir)/tui/KERNEL;
	cp -fr KERNEL $(docdir)/tui
	cp -fr $(srcdir)/KERNEL/sources/static/*.* $(docdir)/tui/KERNEL;
	cp -fr $(srcdir)/KERNEL/sources/ $(docdir)/tui/KERNEL;
	cp -fr $(srcdir)/KERNEL/HTML/ $(docdir)/tui/KERNEL;
	cp -f $(srcdir)/pythfilter.py $(docdir)/tui/KERNEL;

