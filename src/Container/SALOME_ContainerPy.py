#! /usr/bin/env python
#
#  SALOME Container : implementation of container and engine for Kernel
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
#  File   : SALOME_ContainerPy.py
#  Author : Paul RASCLE, EDF
#  Module : SALOME
#  $Header$

import os
import sys
import string
from omniORB import CORBA, PortableServer
# import SALOMEDS before other SALOME modules
# (if not, incomplete import done by SALOME module: no load of SALOMEDS_attributes)
import SALOMEDS 
import Engines, Engines__POA
from SALOME_NamingServicePy import *
from SALOME_ComponentPy import *

from SALOME_utilities import *

#=============================================================================

#define an implementation of the container interface

class SALOME_ContainerPy_i (Engines__POA.Container):
    _orb = None
    _poa = None
    _numInstance = 0
    _containerName = None
    _contId = None
    _machineName = None
    _naming_service = None
    _Name = None
    _Pid = None

    #-------------------------------------------------------------------------

    def __init__(self, orb, poa, containerName):
        MESSAGE( "SALOME_ContainerPy_i::__init__" )
        self._orb = orb
        self._poa = poa
        self._containerName = containerName
        self._contId = self._poa.reference_to_id( self._this() )
        #HostName = os.getenv( "HOSTNAME" )
        HostName = os.getenv( "HOST" )
        MESSAGE( "SALOME_ContainerPy_i::__init__ _machineName : " + HostName )
        myMachine = string.split( HostName , '.' )
        self._machineName = myMachine[0]
        naming_service = SALOME_NamingServicePy_i(self._orb)
        self._naming_service = naming_service
        Container_path = "/Containers/" + myMachine[0] + "/" + self._containerName
        self._Name = Container_path
        MESSAGE( "SALOME_ContainerPy_i::__init__ _Name : " + str(Container_path) )
        naming_service.Register(self._this(), Container_path)
        self._Pid = os.getpid()
            
    #-------------------------------------------------------------------------

    def delete(self):
        MESSAGE( "SALOME_ContainerPy_i::delete" )

    #-------------------------------------------------------------------------

    def destroy(self):
        MESSAGE( "SALOME_ContainerPy_i::destroy : " + self._Name )
        try :
            self._naming_service.Destroy_Name( self._Name )
            #d = dir(self._poa)
            #print "dir(self._poa) ",d,type(self._poa)
            MESSAGE( "SALOME_ContainerPy_i::destroy _poa.deactivate_object self._contId " )
            self._poa.deactivate_object( self._contId )
            MESSAGE( "SALOME_ContainerPy_i::destroy _poa.deactivate_object done" )
            self._poa._release()
            MESSAGE( "SALOME_ContainerPy_i::destroy _poa._released" )
            #d = dir(self)
            #print "dir(self) ",d,type(self)
            #d = dir(self._contId)
            #print "dir(self._contId) ",d,type(self._contId)
            #MESSAGE( "SALOME_ContainerPy_i::destroy " )
            #self.delete( self._contId )
            #self._contId.delete()
            #MESSAGE( "SALOME_ContainerPy_i::destroy self._contId deleted" )
            #self._remove_ref()
            #MESSAGE( "SALOME_ContainerPy_i::destroy self _removed_ref" )
            MESSAGE( "SALOME_ContainerPy_i::destroyed" )
        except :
            MESSAGE( "SALOME_ContainerPy_i::destroy failed" )
        #sys.exit(0)

    #-------------------------------------------------------------------------

    def start_impl( self, ContainerName , ContainerType ):
        MESSAGE(  "SALOME_ContainerPy_i::start_impl " + str(ContainerName) )
        myMachine=string.split(os.getenv( "HOST" ),'.')
        theContainer = "/Containers/" + myMachine[0] + "/" + ContainerName
        try:
            obj = self._naming_service.Resolve(theContainer)
        except :
            obj = None
            MESSAGE(  "SALOME_ContainerPy_i::start_impl " + str(ContainerName) + ".object not found in Naming Service" )
        if obj is None:
            container = None
        else:
            container = obj._narrow(Engines.Container)
            if container is None:
                MESSAGE( "SALOME_ContainerPy_i::start_impl " + str(containerName) + ".object exists but is not a Container" )
            else :
                MESSAGE( "SALOME_ContainerPy_i::start_impl " + str(ContainerName) + ".object found" )
            return container
        if ContainerType == Engines.CppContainer :
            shstr = "SALOME_Container "
        else :
            shstr = "SALOME_ContainerPy.py "
        shstr += ContainerName

        # mpv: fix for SAL4731 - allways create new file to write log of server
        num = 1
        fileName = ""
        while 1:
            fileName = "/tmp/"+ContainerName+"_%i.log"%num
            if not os.path.exists(fileName):
                break
            num += 1
            pass
        
        shstr += " > "
        shstr += fileName
        shstr += " 2>&1 &"
        
        MESSAGE(  "SALOME_ContainerPy_i::start_impl " + "os.system(" + str(shstr) + ")" )
        os.system( shstr )
        count = 21
        while container is None :
            time.sleep(1)
            count = count - 1
            MESSAGE(  str(count) + ". Waiting for " + str(theContainer) )
            try :
                obj = self._naming_service.Resolve(theContainer)
            except :
                obj = None
            if obj is None:
                container = None
            else:
                container = obj._narrow(Engines.Container)
                if container is None:
                    MESSAGE(  str(containerName) + ".object exists but is not a Container" )
                else :
                    MESSAGE(  str(containerName) + ".object registered" )
                return container
            if count == 0 :
                MESSAGE(  "start_impl of " + str(theContainer) + " failed." )
                return container

    #-------------------------------------------------------------------------

    def load_impl(self, nameToRegister, componentName):
        MESSAGE(  "SALOME_ContainerPy_i::load_impl " + str(nameToRegister) + ' ' + str(componentName) )
        self._numInstance = self._numInstance +1
        instanceName = nameToRegister + "_inst_" + `self._numInstance`
        interfaceName = nameToRegister
        the_command = "import " + nameToRegister + "\n"
        the_command = the_command + "comp_i = " + nameToRegister + "." + nameToRegister
        the_command = the_command + "(self._orb, self._poa, self._this(), self._containerName, instanceName, interfaceName)\n"
        valdir = dir()
        print "dir() : ",valdir
        print "self._orb",self._orb
        print "self._poa",self._poa
        print "self._this()",self._this()
        print "self._containerName",self._containerName
        print "instanceName",instanceName
        print "interfaceName",interfaceName
        print "exec the_command = '",the_command,"' :"
        try :
            exec the_command
            comp_o = comp_i._this()
            MESSAGE( "SALOME_ContainerPy_i::load_impl " + componentName + " imported" )
        except :
            comp_o = None
            MESSAGE( "SALOME_ContainerPy_i::load_impl " + componentName + " except" )
        print "return from SALOME_ContainerPy_i::load_impl "
        return comp_o
    
    #-------------------------------------------------------------------------

    def remove_impl(self, component):
        MESSAGE( "SALOME_ContainerPy_i::remove_impl" )

    #-------------------------------------------------------------------------

    def finalize_removal(self):
        MESSAGE( "SALOME_ContainerPy_i::finalize_removal" )

    #-------------------------------------------------------------------------

    def ping(self):
        MESSAGE( "SALOME_ContainerPy_i::ping" )

    #-------------------------------------------------------------------------

    def type(self):
        MESSAGE( "SALOME_ContainerPy_i::type" )
        return Engines.PythonContainer

    #-------------------------------------------------------------------------

    def _get_name(self):
        MESSAGE( "SALOME_ContainerPy_i::_get_name " + self._Name )
        return self._Name

    #-------------------------------------------------------------------------

    def _get_machineName(self):
        MESSAGE( "SALOME_ContainerPy_i::_get_MachineName " + self._machineName )
        return self._machineName


    #-------------------------------------------------------------------------

    def getPID(self):
        MESSAGE( "SALOME_ContainerPy_i::getPID" )
        return self._Pid

#=============================================================================

#initialise the ORB and find the root POA
orb = CORBA.ORB_init(sys.argv, CORBA.ORB_ID)
poa = orb.resolve_initial_references("RootPOA")

#create an instance of SALOME_ContainerPy_i and a Container reference
#containerName = "FactoryServerPy"
MESSAGE( "SALOME_ContainerPy.py " + str(sys.argv) )
if len( sys.argv ) > 1 :
    containerName = sys.argv[1]
else :
    containerName = "FactoryServerPy"
cpy_i = SALOME_ContainerPy_i(orb, poa, containerName)
cpy_o = cpy_i._this()

#activate the POA
poaManager = poa._get_the_POAManager()
poaManager.activate()

#Block for ever
orb.run()

print "SALOME_ContainerPy --> orb.destroy"
orb.destroy()
print "SALOME_ContainerPy orb.destroyed"


        
            


