#! /usr/bin/env python
#
#  SALOME startContainer : implementation of container and engine for Kernel
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
#  File   : startContainer.py
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
    print 'USAGE : startContainer.py with_xterm HostName ContainerName ContainerType NSHostName NSHostPort'
    print '        ContainerType : Engines.CppContainer or Engines.PythonContainer'
    sys.exit(0)

orb = CORBA.ORB_init(sys.argv, CORBA.ORB_ID)

naming_service = SALOME_NamingServicePy_i(orb)

MyContainersMgr = naming_service.Resolve("/Kernel/ContainersManager")
DefaultParams = MyContainersMgr.Parameters()
DefaultParams.with_xterm = int( sys.argv[1] )
DefaultParams.HostName = sys.argv[2]
DefaultParams.ContainerName = sys.argv[3]
if sys.argv[4] == 'Engines.CppContainer' :
    DefaultParams.ContainerType = Engines.CppContainer
elif sys.argv[4] == 'Engines.PythonContainer' :
    DefaultParams.ContainerType = Engines.PythonContainer
else :
    DefaultParams.ContainerType = Engines.UndefinedContainerType
DefaultParams.NsHostName = sys.argv[5]
DefaultParams.NsPort = int( sys.argv[6] )

#print "startContainer Parameters :"
#print "with_xterm    :",DefaultParams.with_xterm
#print "Os            :",DefaultParams.Os
#print "Memory        :",DefaultParams.Memory
#print "CpuClock      :",DefaultParams.CpuClock
#print "NbProc        :",DefaultParams.NbProc
#print "NbNode        :",DefaultParams.NbNode
#print "HostName      :",DefaultParams.HostName
#print "ContainerName :",DefaultParams.ContainerName
#print "ContainerType :",DefaultParams.ContainerType
#print "NsHostName    :",DefaultParams.NsHostName
#print "NsPort        :",DefaultParams.NsPort

aContainer = MyContainersMgr.FindOrStartContainer( DefaultParams )

if aContainer != None :
    aContainer.ping()
else :
    print "startContainer failed"



