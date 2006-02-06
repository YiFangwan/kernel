# -* Makefile *- 
#
# Author : Paul RASCLE (EDF)
# Date : 06/02/2006
# $Header$
#

#SUBDIRS = salome

docdir   = $(datadir)/doc
doc_DATA = \
	index.html \
	UnitTests.html \
	SALOME_Application.html \
	INSTALL.html

EXTRA_DIST=$(doc_DATA)


# Building html from restructured text files
STYLESHEET=rst.css
RST2HTML=@RST2HTML@
#RSTOPTS=--output-encoding=latin1 --stylesheet=$(STYLESHEET)
RSTOPTS=--output-encoding=latin1

%.html: $(srcdir)/%.txt
	$(RST2HTML) $(RSTOPTS) $< $@
