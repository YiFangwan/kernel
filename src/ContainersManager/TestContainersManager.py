
#ContainersManager_Server -ORBInitRef NameService=corbaname::localhost

import Engines

import Containers

def ContainerParameters( Params ) :
    print "ContainerParameters :"
    print "Os            :",Params.Os
    print "Memory        :",Params.Memory
    print "CpuClock      :",Params.CpuClock
    print "NbProc        :",Params.NbProc
    print "NbNode        :",Params.NbNode
    print "HostName      :",Params.HostName
    print "ContainerName :",Params.ContainerName
    print "ContainerType :",Params.ContainerType
    print "NsHostName    :",Params.NsHostName
    print "NsPort        :",Params.NsPort

def ComputerParameters( Params ) :
    print "ComputerParameters :"
    print "FullName      :",Params.FullName
    print "Alias         :",Params.Alias
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
    print 'Path           ',env.Path
    print 'Ld_Library_Path',env.Ld_Library_Path
    print 'PythonPath     ',env.PythonPath
    print 'CasRoot        ',env.CasRoot



MyContainersMgr = clt.waitNS("/Kernel/ContainersManager",Containers.Manager)

MyContainersMgr.ping()

DefaultParams = MyContainersMgr.Parameters()

ContainerParameters( DefaultParams )

FactoryServer = MyContainersMgr.FindOrStartContainer( DefaultParams )

FactoryServer.ping()

print "Container FactoryServer is running on",FactoryServer._get_machineName(),"with name",FactoryServer._get_name(),"and type",FactoryServer.type()

DefaultParams.ContainerType = Engines.Python

FactoryServerPy = MyContainersMgr.FindOrStartContainer( DefaultParams )

FactoryServerPy.ping()

print "Container FactoryServerPy is running on",FactoryServerPy._get_machineName(),"with name",FactoryServerPy._get_name(),"and type",FactoryServerPy.type()

DefaultParams.ContainerType = Engines.Undefined

DefaultParams.ContainerName = "SuperVisionContainer"

SuperVisionContainer = MyContainersMgr.FindOrStartContainer( DefaultParams )

SuperVisionContainer.ping()

print "Container SuperVisionContainer is running on",SuperVisionContainer._get_machineName(),"with name",SuperVisionContainer._get_name(),"and type",SuperVisionContainer.type()


#ResourcesManager_Server -common /home/Salome2/KERNEL_install/share/salome/resources/ResourcesCatalog.xml -ORBInitRef NameService=corbaname::localhost

import Resources

MyResourcesMgr = clt.waitNS("/Kernel/ResourcesManager",Resources.Manager)

MyResourcesMgr.ping()

AllComputers = MyResourcesMgr.AllComputers()
print 'AllComputers',len( AllComputers ),'computers found'
i = 0
while i < len( AllComputers ) :
    print  ''
    ComputerParameters( AllComputers[i].Parameters() )
    ComputerEnvironement( AllComputers[i].Environment() )
    i = i + 1

ListOfComputers = MyResourcesMgr.GetComputers( DefaultParams )
print  ''
print 'ListOfComputers',len( ListOfComputers ),'computers found'
i = 0
while i < len( ListOfComputers ) :
    print  ''
    ComputerParameters( ListOfComputers[i].Parameters() )
    ComputerEnvironement( ListOfComputers[i].Environment() )
    i = i + 1

aComputer = MyResourcesMgr.SelectComputer( DefaultParams )
aComputer.Parameters().FullName

aComputer = MyResourcesMgr.GetComputer( ListOfComputers )
aComputer.Parameters().FullName

aComputer = MyResourcesMgr.SearchComputer("bordolex")
aComputer.Parameters().FullName

aComputer = MyResourcesMgr.SearchComputer("bojolex")
aComputer.Parameters().FullName

aComputer = MyResourcesMgr.SearchComputer("xmen.saclay.opencascade.com")
aComputer.Parameters().FullName

aComputer = MyResourcesMgr.SearchComputer("unknown")
if aComputer is None :
    print "unknown is None Ok"
else :
    print "Error :",aComputer.Parameters().FullName


DefaultParams.Memory = 500

ListOfComputers = MyResourcesMgr.GetComputers( DefaultParams )
print  ''
print 'ListOfComputers',len( ListOfComputers ),'computers found'
i = 0
while i < len( ListOfComputers ) :
    print  ''
    ComputerParameters( ListOfComputers[i].Parameters() )
    ComputerEnvironement( ListOfComputers[i].Environment() )
    i = i + 1

aComputer = MyResourcesMgr.SelectComputer( DefaultParams )
aComputer.Parameters().FullName

aComputer = MyResourcesMgr.GetComputer( ListOfComputers )
aComputer.Parameters().FullName

DefaultParams.Memory = 1000

ListOfComputers = MyResourcesMgr.GetComputers( DefaultParams )
print  ''
print 'ListOfComputers',len( ListOfComputers ),'computers found'
if len( ListOfComputers ) > 0 :
    print "Error",len( ListOfComputers ),"Computers found"
    i = 0
    while i < len( ListOfComputers ) :
        print  ''
        ComputerParameters( ListOfComputers[i].Parameters() )
        ComputerEnvironement( ListOfComputers[i].Environment() )
        i = i + 1

aComputer = MyResourcesMgr.SelectComputer( DefaultParams )
if aComputer is None :
    print "aComputer is None Ok"
else :
    print "Error :",aComputer.Parameters().FullName

aComputer = MyResourcesMgr.GetComputer( ListOfComputers )
if aComputer is None :
    print "aComputer is None Ok"
else :
    print "Error :",aComputer.Parameters().FullName



DefaultParams.Memory = 500
DefaultParams.ContainerName = 'xmenServer'
xmenServer = MyContainersMgr.FindOrStartContainer( DefaultParams )

xmenServer.ping()

component = MyContainersMgr.FindOrLoad_Component( DefaultParams , "AddComponent" )

Containers = MyContainersMgr.FindContainers( DefaultParams )
if len( Containers ) == 0 :
    print len( Containers ),"found Error"

DefaultParams.Memory = 0
DefaultParams.ContainerName = ''

DefaultParams.ContainerType = Engines.Undefined
Containers = MyContainersMgr.FindContainers( DefaultParams )
i = 0
while i < len( Containers ) :
    print "Container running on",Containers[ i ]._get_machineName(),"with name",Containers[ i ]._get_name(),"and type",Containers[ i ].type()
    i = i + 1

DefaultParams.ContainerType = Engines.Cpp
Containers = MyContainersMgr.FindContainers( DefaultParams )
i = 0
while i < len( Containers ) :
    print "Container running on",Containers[ i ]._get_machineName(),"with name",Containers[ i ]._get_name(),"and type",Containers[ i ].type()
    i = i + 1

DefaultParams.ContainerType = Engines.Python
Containers = MyContainersMgr.FindContainers( DefaultParams )
i = 0
while i < len( Containers ) :
    print "Container running on",Containers[ i ]._get_machineName(),"with name",Containers[ i ]._get_name(),"and type",Containers[ i ].type()
    i = i + 1

ContainerParameters( DefaultParams )

DefaultParams.ContainerType = Engines.Cpp
DefaultParams.ContainerName = 'FactoryServer'
FactoryServerAddComponent = MyContainersMgr.FindOrLoad_Component( DefaultParams , "AddComponent" )
FactoryServerAddComponent._get_instanceName()
FactoryServerAddComponent._get_interfaceName()

AddComponent = MyContainersMgr.FindComponent( DefaultParams , 'AddComponent' )
AddComponent._get_instanceName()
AddComponent._get_interfaceName()

ListOfAddComponent = MyContainersMgr.FindComponents( DefaultParams , 'AddComponent' )

FactoryServeSubComponent = MyContainersMgr.FindOrLoad_Component( DefaultParams , "SubComponent" )

from LifeCycleCORBA import *
orb = CORBA.ORB_init([''], CORBA.ORB_ID)
lcc = LifeCycleCORBA( orb )

DefaultParams.ContainerName = 'SuperVisionContainer'

lccMulComponent = lcc.FindOrLoadComponent( 'SuperVisionContainer' , "MulComponent" )

DefaultParams.ContainerName = 'DivComponentContainer'

lccDivComponent = lcc.FindOrLoadComponent( 'DivComponentContainer' , "DivComponent" )

