#! /usr/bin/env python
#
#  SALOME TestContainer : test of container creation and its life cycle
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
#  File   : TestComponentPy.py
#  Author : Paul RASCLE, EDF
#  Module : SALOME
#  $Header$

import os
import sys
import string
from omniORB import CORBA
import CosNaming
import Engines


#initialise the ORB
orb = CORBA.ORB_init(sys.argv, CORBA.ORB_ID)

#obtain a reference to the root naming context
obj = orb.resolve_initial_references("NameService")
rootContext = obj._narrow(CosNaming.NamingContext)

if rootContext is None:
    print "Name Service Reference is invalid"
    sys.exit(1)

#resolve the name /Containers.dir/FactoryServerPy.object
myMachine=string.split(os.getenv( "HOSTNAME" ),'.')
machineName= myMachine[0]
containerName = "FactoryServerPy"
name = [CosNaming.NameComponent("Containers","dir"),
        CosNaming.NameComponent(machineName,"dir"),
        CosNaming.NameComponent(containerName,"object")]

try:
    obj = rootContext.resolve(name)
except CosNaming.NamingContext.NotFound, ex:
    print  containerName , " not found in Naming Service"
    sys.exit(1)

container = obj._narrow(Engines.Container)
print container._get_machineName()
comp = container.load_impl("SALOME_TestComponentPy","SALOME_TestComponentPy")
print comp._get_instanceName()
comp.ping()
comptest = comp._narrow(Engines.TestComponent)
if comptest is None:
    print "probleme cast"
print comptest.Coucou(1)


