
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
if len( ListOfComputers ) > 0 :
    print "Error",len( ListOfComputers ),"Computers found"
    i = 0
    while i < len( ListOfComputers ) :
        print  ''
        ComputerParameters( ListOfComputers[i].Parameters() )
        ComputerEnvironement( ListOfComputers[i].Environment() )
        i = i + 1
else :
    print 'ListOfComputers',len( ListOfComputers ),'computers found Ok'

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

ContainerParameters( DefaultParams )

DefaultParams.ContainerType = Engines.Undefined
Containers = MyContainersMgr.FindContainers( DefaultParams )
i = 0
while i < len( Containers ) :
    print "Container running on",Containers[ i ]._get_machineName(),"with name",Containers[ i ]._get_name(),"and type",Containers[ i ].type()
    i = i + 1

aContainer = MyContainersMgr.FindContainer( DefaultParams )
print "Container running on",aContainer._get_machineName(),"with name",aContainer._get_name(),"and type",aContainers.type()

aContainer = MyContainersMgr.FindContainer( 'FactoryServer' ) )
print "Container running on",aContainer._get_machineName(),"with name",aContainer._get_name(),"and type",aContainers.type()

aContainer = MyContainersMgr.FindContainer( 'localhost/FactoryServer' ) )
print "Container running on",aContainer._get_machineName(),"with name",aContainer._get_name(),"and type",aContainers.type()

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
print FactoryServerAddComponent.GetContainerRef()._get_name(),FactoryServerAddComponent._get_instanceName(),FactoryServerAddComponent._get_interfaceName()

DefaultParams.ContainerType = Engines.Undefined
DefaultParams.ContainerName = ''

ContainerParameters( DefaultParams )

AddComponent = MyContainersMgr.FindComponent( DefaultParams , 'AddComponent' )
print AddComponent.GetContainerRef()._get_name(),AddComponent._get_instanceName(),AddComponent._get_interfaceName()

ListOfAddComponent = MyContainersMgr.FindComponents( DefaultParams , 'AddComponent' )
i = 0
while i < len( ListOfAddComponent ) :
    print ListOfAddComponent[ i ].GetContainerRef()._get_name(),ListOfAddComponent[ i ]._get_instanceName(),ListOfAddComponent[ i ]._get_interfaceName()
    i = i + 1

aSameAddComponent = MyContainersMgr.FindOrLoad_Component( DefaultParams , "AddComponent" )
print aSameAddComponent.GetContainerRef()._get_name(),aSameAddComponent._get_instanceName(),aSameAddComponent._get_interfaceName()

ListOfAddComponent = MyContainersMgr.FindComponents( DefaultParams , 'AddComponent' )
i = 0
while i < len( ListOfAddComponent ) :
    print  ListOfAddComponent[ i ].GetContainerRef()._get_name(),ListOfAddComponent[ i ]._get_instanceName(),ListOfAddComponent[ i ]._get_interfaceName()
    i = i + 1

ContainerParameters( DefaultParams )

FactoryServeSubComponent = MyContainersMgr.FindOrLoad_Component( DefaultParams , "SubComponent" )
ListOfComponents = MyContainersMgr.FindComponents( DefaultParams , '' )
i = 0
while i < len( ListOfComponents ) :
    print ListOfComponents[ i ].GetContainerRef()._get_name(),ListOfComponents[ i ]._get_instanceName(),ListOfComponents[ i ]._get_interfaceName()
    i = i + 1

DefaultParams.ContainerName = 'SuperVisionContainer'
SuperVisionContainerAddComponent = MyContainersMgr.FindOrLoad_Component( DefaultParams , "AddComponent" )
DefaultParams.ContainerName = ''
ListOfAddComponent = MyContainersMgr.FindComponents( DefaultParams , 'AddComponent' )
i = 0
while i < len( ListOfAddComponent ) :
    print ListOfAddComponent[ i ].GetContainerRef()._get_name(),ListOfAddComponent[ i ]._get_instanceName(),ListOfAddComponent[ i ]._get_interfaceName()
    i = i + 1


ListOfComponents = MyContainersMgr.FindComponents( DefaultParams , '' )
i = 0
while i < len( ListOfComponents ) :
    print ListOfComponents[ i ].GetContainerRef()._get_name(),ListOfComponents[ i ]._get_instanceName(),ListOfComponents[ i ]._get_interfaceName()
    i = i + 1






from LifeCycleCORBA import *
orb = CORBA.ORB_init([''], CORBA.ORB_ID)
lcc = LifeCycleCORBA( orb )


lccMulComponent = lcc.FindOrLoadComponent( 'SuperVisionContainer' , "MulComponent" )

print lccMulComponent.GetContainerRef()._get_name(),lccMulComponent._get_instanceName(),lccMulComponent._get_interfaceName()

lccDivComponent = lcc.FindOrLoadComponent( 'DivComponentContainer' , "DivComponent" )

print lccDivComponent.GetContainerRef()._get_name(),lccDivComponent._get_instanceName(),lccDivComponent._get_interfaceName()


DefaultParams = lcc.Parameters()

DefaultParams.ContainerName = 'SuperVisionContainer'

lccAddComponent = lcc.FindOrLoadComponent( DefaultParams , "AddComponent" )
print lccAddComponent.GetContainerRef()._get_name(),lccAddComponent._get_instanceName(),lccAddComponent._get_interfaceName()

DefaultParams.ContainerName = 'SubComponentContainer'

lccSubComponent = lcc.FindOrLoadComponent( DefaultParams , "SubComponent" )
print lccSubComponent.GetContainerRef()._get_name(),lccSubComponent._get_instanceName(),lccSubComponent._get_interfaceName()

aContainer = lcc.FindContainer( DefaultParams )
print "Container running on",aContainer._get_machineName(),"with name",aContainer._get_name(),"and type",aContainer.type()

aContainer = lcc.FindContainer( 'FactoryServer' )
print "Container running on",aContainer._get_machineName(),"with name",aContainer._get_name(),"and type",aContainer.type()

aContainer = lcc.FindContainer( 'localhost/FactoryServer' )
print "Container running on",aContainer._get_machineName(),"with name",aContainer._get_name(),"and type",aContainer.type()

ContainerParameters( DefaultParams )

DefaultParams.ContainerName = ''

ListOfContainers = lcc.FindContainers( DefaultParams )
i = 0
while i < len( ListOfContainers ) :
    print "Container running on",ListOfContainers[ i ]._get_machineName(),"with name",ListOfContainers[ i ]._get_name(),"and type",ListOfContainers[ i ].type()
    i = i + 1

ListOfComponents = lcc.FindComponents( DefaultParams , '' )
i = 0
while i < len( ListOfComponents ) :
    print ListOfComponents[ i ].GetContainerRef()._get_name(),ListOfComponents[ i ]._get_instanceName(),ListOfComponents[ i ]._get_interfaceName()
    i = i + 1


aComponent = lcc.FindComponent( DefaultParams , 'SubComponent' )
print aComponent.GetContainerRef()._get_name(),aComponent._get_instanceName(),aComponent._get_interfaceName()

lccMulComponent = lcc.FindOrLoadComponent( DefaultParams , "MulComponent" )

print lccMulComponent.GetContainerRef()._get_name(),lccMulComponent._get_instanceName(),lccMulComponent._get_interfaceName()

DefaultParams.HostName = 'bojolex'

lccbojolexMulComponent = lcc.FindOrLoadComponent( DefaultParams , "MulComponent" )

DefaultParams.HostName = 'toto'

lcctotoMulComponent = lcc.FindOrLoadComponent( DefaultParams , "MulComponent" )

