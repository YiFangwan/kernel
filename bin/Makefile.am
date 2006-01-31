# -* Makefile *- 
#
# Author : Guillaume Boulant (CSSI)
# Module : KERNEL
# $Header$
#


include $(top_srcdir)/salome_adm/unix/make_common_starter.am

#
# ===============================================================
# Files to be installed
# ===============================================================
#

# These files are data, module or lib files
salomescript_DATA= \
	VERSION \
	orbmodule.py \
	salome.launch \
	salomeConsole.py

# These files are executable scripts
salomescript_SCRIPTS=\
	envSalome.py \
	runIDLparser \
	runSalome.py \
	killSalome.py \
	killSalomeWithPort.py \
	runSalome \
	runSalome.csh \
	runNS.sh \
	createAppli.sh \
	launchConfigureParser.py \
	showNS.py \
	addToKillList.py \
	NSparam.py


install: install-am install-appliskel

# This is an ugly target to avoid exploring the appliskel subdirectory.
install-appliskel:
	$(INSTALL) -d $(salomescriptdir)
	cp -r $(srcdir)/appliskel $(salomescriptdir)
	find $(salomescriptdir) -name CVS | xargs rm -rf
