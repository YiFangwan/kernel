#! /usr/bin/env python
#
#  SALOME runContainer : implementation of container and engine for Kernel
#
#  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
#  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS 
# 
#  This library is free software; you can redistribute it and/or 
#  modify it under the terms of the GNU Lesser General Public 
#  License as published by the Free Software Foundation; either 
#  version 2.1 of the License. 
# 
#  This library is distributed in the hope that it will be useful, 
#  but WITHOUT ANY WARRANTY; without even the implied warranty of 
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
#  Lesser General Public License for more details. 
# 
#  You should have received a copy of the GNU Lesser General Public 
#  License along with this library; if not, write to the Free Software 
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA 
# 
#  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 
#
#
#
#  File   : runContainer.py
#  Author : Jean Rahuel, CEA
#  Module : SALOME
#  $Header: 

import os
import sys
import string
from omniORB import CORBA, PortableServer
import Containers
import Engines
from SALOME_NamingServicePy import *

if sys.argv[1] == '--help' :
    print 'USAGE : runContainer.py ContainerName ContainerType NSHostName NSHostPort Path Ld_Library_Path PythonPath CasRoot'
    print '        ContainerType : Engines.Cpp or Engines.Python'
    sys.exit(0)

if ContainerType == 'Engines.Cpp' :
    Container = 'SALOME_Container '
elif ContainerType == 'Engines.Python' :
    Container = 'SALOME_ContainerPy.py '
else :
    print 'ContainerType is bad'
    Container = ''

if len( Container ) > 0 :
    Container = Container + ' -ORBInitRef NameService=corbaname::'
    Container = Container + NSHostName + ':' + NSHostPort
    os.environ["PATH"] = Path
    os.environ["LD_LIBRARY_PATH"] = Ld_Library_Path
    os.environ["PYTHONPATH"] = PythonPath
    os.environ["CASROOT"] = CasRoot
    os.environ["CSF_PluginDefaults"] = os.path.join( CasRoot , "share" , "salome" , "resources" )
    print Container
    exec Container

