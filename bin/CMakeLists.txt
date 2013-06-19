# Copyright (C) 2012-2013  CEA/DEN, EDF R&D, OPEN CASCADE
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

ADD_SUBDIRECTORY(appliskel)

CONFIGURE_FILE(VERSION.in VERSION @ONLY)

INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/VERSION DESTINATION ${SALOME_INSTALL_BINS})

# ===============================================================
# Files to be installed
# ===============================================================

# These files are data, module or lib files
#INSTALL(FILES config_appli.xml salome.launch DESTINATION ${SALOME_INSTALL_SCRIPT_DATA})

# These files are executable scripts
SET(SCRIPTS
  appli_clean.sh
  runIDLparser
  addToKillList.py
  appli_gen.py
  envSalome.py
  killSalome.py
  killSalomeWithPort.py
  launchConfigureParser.py
  nameserver.py
  NSparam.py
  orbmodule.py
  ORBConfigFile.py
  parseConfigFile.py
  runSalome
  runSalome.py
  runSession.py
  runConsole.py
  salomeConsole.py
  salomeRunner.py
  salome_session.py
  salome_utils.py
  searchFreePort.py
  server.py
  setenv.py
  showNS.py
  shutdownSalome.py
  virtual_salome.py
  waitContainers.py
  waitNS.py
  )
SALOME_INSTALL_SCRIPTS("${SCRIPTS}" ${SALOME_INSTALL_SCRIPT_SCRIPTS})