#! /usr/bin/env python3
#  -*- coding: iso-8859-1 -*-
# Copyright (C) 2007-2020  CEA/DEN, EDF R&D, OPEN CASCADE
#
# Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
# CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
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

## \file killSalome.py
#  Stop all %SALOME servers from all sessions by killing them
#

import os, sys, re, signal

from contextlib import  suppress
from killSalomeWithPort import killMyPort, getPiDict, checkUnkilledProcesses, killProcesses
#from salome_utils import getHostName, getShortHostName
from salome_utils import getUserName

def killAllPorts():
    """
    Kill all SALOME sessions belonging to the user.
    """
    user = getUserName()

    def _getPortsFromPiDict(hidden, pattern):
        fpidict = getPiDict('#####', hidden=hidden)
        dirpidict = os.path.dirname(fpidict)
        fpidict = os.path.basename(fpidict)
        fpidict = fpidict.replace('#####', r'(\d*)')
        fnamere = re.compile(pattern.format(fpidict))
        with suppress(IOError):
            for _f in os.listdir(dirpidict):
                _mo = fnamere.match(_f)
                if _mo:
                    killMyPort(_mo.group(1))

    _getPortsFromPiDict(True, "^{}")
    _getPortsFromPiDict(False, "^{}$")
    # kill other processes
    killProcesses(checkUnkilledProcesses())
    if sys.platform != 'win32':
        # delete uri files needed by ompi-server
        cmd = "rm -f " + os.path.expanduser("~") + "/.urifile_*"
        os.system(cmd)

if __name__ == "__main__":
    try:
        from salomeContextUtils import setOmniOrbUserPath
        setOmniOrbUserPath()
    except Exception as e:
        print(e)
        sys.exit(1)
    killAllPorts()
    pass
