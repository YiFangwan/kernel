
#ContainersManager_Server -ORBInitRef NameService=corbaname::localhost

def ContainerParameters( Params ) :
    print "ContainerParameters :"
    print "Os            :",Params.Os
    print "Memory        :",Params.Memory
    print "CpuClock      :",Params.CpuClock
    print "NbProc        :",Params.NbProc
    print "NbNode        :",Params.NbNode
    print "HostName      : '" + Params.HostName + "'"
    print "ContainerName : '" + Params.ContainerName + "'"
    print "ContainerType :",Params.ContainerType
    print "NsHostName    : '" + Params.NsHostName + "'"
    print "NsPort        :",Params.NsPort

def ComputerParameters( Params ) :
    print "ComputerParameters :"
    print "FullName      : '" + Params.FullName + "'"
    print "Alias         : '" + Params.Alias + "'"
    print "SshAccess     :",Params.SshAccess
    print "Interactive   :",Params.Interactive
    print "Batch         :",Params.Batch
    print "UserName      :",Params.UserName
    print "Os            :",Params.Os
    print "Memory        :",Params.Memory
    print "Swap          :",Params.Swap
    print "CpuClock      :",Params.CpuClock
    print "NbProc        :",Params.NbProc
    print "NbNode        :",Params.NbNode


def ComputerEnvironement( env ) :
    rootNames = env.Module_Root_Dir_Names
    rootValues = env.Module_Root_Dir_Values
    j = 0
    while j < len( rootNames ) :
        print rootNames[ j ],rootValues[ j ]
        j = j + 1
    #print 'Path           ',env.Path
    #print 'Ld_Library_Path',env.Ld_Library_Path
    #print 'PythonPath     ',env.PythonPath
    #print 'CasRoot        ',env.CasRoot

def ListComputers() :
    AllComputers = MyResourcesMgr.AllComputers()
    print  ''
    print 'ListComputers',len( AllComputers ),'computers found'
    i = 0
    while i < len( AllComputers ) :
        print AllComputers[i].FullName(),AllComputers[i].Alias(),'IsAlive',AllComputers[i].IsAlive()
        i = i + 1

def ListContainers() :
    AllContainers = MyContainersMgr.AllContainers()
    print  ''
    print 'ListContainers',len( AllContainers ),'containers found'
    i = 0
    while i < len( AllContainers ) :
        print AllContainers[i]._get_name(),'on',AllContainers[i]._get_machineName(),AllContainers[i].type()
        i = i + 1

def ListComponents() :
    AllComponents = MyContainersMgr.AllComponents()
    print  ''
    print 'ListComponents',len( AllComponents ),'components found'
    i = 0
    while i < len( AllComponents ) :
        print AllComponents[i]._get_instanceName(),'of interface',AllComponents[i]._get_interfaceName(),'on',AllComponents[i].GetContainerRef()._get_name(),'of',AllComponents[i].GetContainerRef()._get_machineName(),AllComponents[i].GetContainerRef().type()
        i = i + 1



import sys,os,time
import string
from omniORB import CORBA

# Import the stubs for the Naming service
import CosNaming

import Engines

import Containers

import Resources

from LifeCycleCORBA import *
orb = CORBA.ORB_init([''], CORBA.ORB_ID)
lcc = LifeCycleCORBA( orb )

# ------------------------NameService--------------------------------------------------

obj = orb.resolve_initial_references("NameService")
rootContext = obj._narrow(CosNaming.NamingContext)

# ------------------------ContainersManager--------------------------------------------------

#MyContainersMgr = clt.waitNS("/Kernel/ContainersManager",Containers.Manager)
name = [CosNaming.NameComponent("Kernel","dir"),
        CosNaming.NameComponent("ContainersManager","object")]
try:
    obj = rootContext.resolve(name)
except CosNaming.NamingContext.NotFound, ex:
    print "/Kernel.dir/ContainersManager.object not found in Naming Service"
    exit(0)

MyContainersMgr = obj._narrow( Containers.Manager )

MyContainersMgr.ping()

#ResourcesManager_Server -common /home/Salome2/KERNEL_install/share/salome/resources/ResourcesCatalog.xml -ORBInitRef NameService=corbaname::localhost

#gdb ResourcesManager_Server
#run -common /home/rahuel/SALOME_BR_JR2/KERNEL_build_withoutihm/share/salome/resources/ResourcesCatalog.xml

# ------------------------ResourcesManager--------------------------------------------------

#MyResourcesMgr = clt.waitNS("/Kernel/ResourcesManager",Resources.Manager)
name = [CosNaming.NameComponent("Kernel","dir"),
        CosNaming.NameComponent("ResourcesManager","object")]
try:
    obj = rootContext.resolve(name)
except CosNaming.NamingContext.NotFound, ex:
    print "/Kernel.dir/ResourcesManager.object not found in Naming Service"
    exit(0)

MyResourcesMgr = obj._narrow( Resources.Manager )

MyResourcesMgr.ping()


