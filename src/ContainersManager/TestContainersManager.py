
#python bin/killSalome.py

from ResourcesContainers import *

# ------------------------FactoryServer--------------------------------------------------

DefaultParams = MyContainersMgr.Parameters()

ContainerParameters( DefaultParams )

FactoryServer = MyContainersMgr.FindOrStartContainer( DefaultParams )

FactoryServer.ping()

print "Container FactoryServer is running on",FactoryServer._get_machineName(),"with name",FactoryServer._get_name(),"and type",FactoryServer.type()

FactoryServer.destroy()
clt.showNS()

#python /home/rahuel/SALOME_BR_JR2/KERNEL_install_withoutihm/bin/salome/SALOME_ContainerPy.py FactoryServerPy -ORBInitRef NameService=corbaname::localhost
# ------------------------FactoryServerPy--------------------------------------------------

DefaultParams.ContainerType = Engines.PythonContainer

FactoryServerPy = MyContainersMgr.FindOrStartContainer( DefaultParams )

FactoryServerPy.ping()

print "Container FactoryServerPy is running on",FactoryServerPy._get_machineName(),"with name",FactoryServerPy._get_name(),"and type",FactoryServerPy.type()

FactoryServerPy.destroy()
clt.showNS()

# ------------------------SuperVisionContainer--------------------------------------------------

DefaultParams.ContainerType = Engines.UndefinedContainerType

DefaultParams.ContainerName = "SuperVisionContainer"

SuperVisionContainer = MyContainersMgr.FindOrStartContainer( DefaultParams )

SuperVisionContainer.ping()

print "Container SuperVisionContainer is running on",SuperVisionContainer._get_machineName(),"with name",SuperVisionContainer._get_name(),"and type",SuperVisionContainer.type()


# ------------------------RunningContainers--------------------------------------------------

ListContainers()



# ------------------------AllComputers--------------------------------------------------

ListComputers()

AllComputers = MyResourcesMgr.AllComputers()
print 'AllComputers',len( AllComputers ),'computers found'
i = 0
while i < len( AllComputers ) :
    print  ''
    ComputerParameters( AllComputers[i].Parameters() )
    ComputerEnvironement( AllComputers[i].Environment() )
    i = i + 1

i = 0
while i < len( AllComputers ) :
    print  ''
    print AllComputers[i].FullName(),AllComputers[i].Alias(),'IsAlive',AllComputers[i].IsAlive()
    i = i + 1

# ------------------------ListOfComputers( Params )---------------------------------------------

ListOfComputers = MyResourcesMgr.GetComputers( DefaultParams )
print  ''
print 'ListOfComputers',len( ListOfComputers ),'computers found'
i = 0
while i < len( ListOfComputers ) :
    print  ''
    ComputerParameters( ListOfComputers[i].Parameters() )
    ComputerEnvironement( ListOfComputers[i].Environment() )
    i = i + 1

i = 0
while i < len( ListOfComputers ) :
    print  ''
    print ListOfComputers[i].FullName(),ListOfComputers[i].Alias(),'IsAlive',ListOfComputers[i].IsAlive()
    i = i + 1

# ------------------------SelectComputer( Params )----------------------------------------------

aComputer = MyResourcesMgr.SelectComputer( DefaultParams )
print aComputer.Parameters().FullName,'IsAlive',aComputer.IsAlive()

# ------------------------GetComputer( ListOfComputers )----------------------------------------

aComputer = MyResourcesMgr.GetComputer( ListOfComputers )
print aComputer.Parameters().FullName,'IsAlive',aComputer.IsAlive()

# ------------------------SearchComputer( HostName )--------------------------------------------

aComputer = MyResourcesMgr.SearchComputer("is109973")
print aComputer.Parameters().FullName,'IsAlive',aComputer.IsAlive()

aComputer = MyResourcesMgr.SearchComputer("is109974")
print aComputer.Parameters().FullName,'IsAlive',aComputer.IsAlive()

aComputer = MyResourcesMgr.SearchComputer("dunex")
print aComputer.Parameters().FullName,'IsAlive',aComputer.IsAlive()

aComputer = MyResourcesMgr.SearchComputer("bojolex")
print aComputer.Parameters().FullName,'IsAlive',aComputer.IsAlive()

aComputer = MyResourcesMgr.SearchComputer("xmen.saclay.opencascade.com")
print aComputer.Parameters().FullName,'IsAlive',aComputer.IsAlive()

aComputer = MyResourcesMgr.SearchComputer("unknown")
if aComputer is None :
    print "unknown is None Ok"
else :
    print "Error :",aComputer.Parameters().FullName


# ------------------------GetComputers( Params )--------------------------------------------------

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

i = 0
while i < len( ListOfComputers ) :
    print  ''
    print ListOfComputers[i].FullName(),ListOfComputers[i].Alias(),'Memory',ListOfComputers[i].Memory(),'IsAlive',ListOfComputers[i].IsAlive()
    i = i + 1

# ------------------------SelectComputer( Params )------------------------------------------------

aComputer = MyResourcesMgr.SelectComputer( DefaultParams )
aComputer.Parameters().FullName

# ------------------------GetComputer( ListOfComputers )------------------------------------------

aComputer = MyResourcesMgr.GetComputer( ListOfComputers )
aComputer.Parameters().FullName

# ------------------------GetComputers( Params )--------------------------------------------------

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



from ResourcesContainers import *

# ------------------------FindOrStartContainer( Params )--------------------------------------------------

DefaultParams = MyContainersMgr.Parameters()

DefaultParams.Memory = 500
DefaultParams.ContainerName = ''
DefaultParams.ContainerType = Engines.CppContainer
ContainerParameters( DefaultParams )

is109974Server = MyContainersMgr.FindOrStartContainer( DefaultParams )

is109974Computer = MyResourcesMgr.SearchComputer( is109974Server._get_machineName() )

print "ping",is109974Computer.ping()
print "FullName",is109974Computer.FullName()
print "Alias",is109974Computer.Alias()
print "IsAlive",is109974Computer.IsAlive()
print "SshAccess",is109974Computer.SshAccess()
print "RshAccess",is109974Computer.RshAccess()
print "Interactive",is109974Computer.Interactive()
print "Batch",is109974Computer.Batch()
print "UserName",is109974Computer.UserName()
print "Os",is109974Computer.Os()
print "Memory",is109974Computer.Memory()
print "Swap",is109974Computer.Swap()
print "CpuClock",is109974Computer.CpuClock()
print "NbProc",is109974Computer.NbProc()
print "NbNode",is109974Computer.NbNode()

ComputerEnvironement( is109974Computer.Environment() )

TestComponent = MyContainersMgr.FindOrLoad_Component( DefaultParams , "SalomeTestComponent" )

# ------------------------RunningContainers( DefaultParams )--------------------------------------------------

DefaultParams.ContainerName = ''
DefaultParams.ContainerType = Engines.UndefinedContainerType
ContainerParameters( DefaultParams )
Containers = MyContainersMgr.FindContainers( DefaultParams )
ContainerParameters( DefaultParams )
i = 0
while i < len( Containers ) :
    print "Container running on",Containers[ i ]._get_machineName(),"with name",Containers[ i ]._get_name(),"and type",Containers[ i ].type()
    i = i + 1

# ------------------------RunningContainers--------------------------------------------------

ListContainers()


# ------------------------FindOrLoad_Component( Params , "MED" )---------------------------------

ContainerParameters( DefaultParams )

MED = MyContainersMgr.FindOrLoad_Component( DefaultParams , "MED" )

MED._get_instanceName()
MED._get_interfaceName()
theContainer = MED.GetContainerRef()
theContainer.getHostName()


# ------------------------FindOrLoad_Component( Params , "AddComponent" )---------------------------------

AddComponent = MyContainersMgr.FindOrLoad_Component( DefaultParams , "AddComponent" )

AddComponent._get_instanceName()
AddComponent._get_interfaceName()
theContainer = AddComponent.GetContainerRef()
theContainer.getHostName()


# ------------------------RunningComponents--------------------------------------------------

ListComponents()


# ------------------------FindOrLoad_Component( Params , "AddComponent" )---------------------------------

DefaultParams.ContainerName = 'AddComponentServer'

AddComponent = MyContainersMgr.FindOrLoad_Component( DefaultParams , "AddComponent" )

# ------------------------RunningComponents--------------------------------------------------

ListComponents()


# ------------------------RunningContainers--------------------------------------------------

ListContainers()

DefaultParams.Memory = 0
DefaultParams.ContainerName = ''




# ------------------------FindContainer--------------------------------------------------

aContainer = MyContainersMgr.FindContainer( DefaultParams )
print "Container running on",aContainer._get_machineName(),"with name",aContainer._get_name(),"and type",aContainer.type()

# ------------------------FindOneContainer--------------------------------------------------

aContainer = MyContainersMgr.FindOneContainer( 'localhost' , 'FactoryServer' )
print "Container running on",aContainer._get_machineName(),"with name",aContainer._get_name(),"and type",aContainer.type()

aContainer = MyContainersMgr.FindOneContainer( 'localhost' , 'FactoryServerPy' )
print "Container running on",aContainer._get_machineName(),"with name",aContainer._get_name(),"and type",aContainer.type()

aContainer = MyContainersMgr.FindOneContainer( 'localhost' , 'SuperVisionContainer' )
print "Container running on",aContainer._get_machineName(),"with name",aContainer._get_name(),"and type",aContainer.type()

aContainer = MyContainersMgr.FindOneContainer( '' , 'SuperVisionContainer' )
print "Container running on",aContainer._get_machineName(),"with name",aContainer._get_name(),"and type",aContainer.type()


aContainer = MyContainersMgr.FindOneContainer( '' , 'AddComponentServer' )
print "Container running on",aContainer._get_machineName(),"with name",aContainer._get_name(),"and type",aContainer.type()


# ------------------------C++ Servers--------------------------------------------------

DefaultParams.ContainerType = Engines.CppContainer
Containers = MyContainersMgr.FindContainers( DefaultParams )
i = 0
while i < len( Containers ) :
    print "Container running on",Containers[ i ]._get_machineName(),"with name",Containers[ i ]._get_name(),"and type",Containers[ i ].type()
    i = i + 1

# ------------------------Python Servers--------------------------------------------------

DefaultParams.ContainerType = Engines.PythonContainer
Containers = MyContainersMgr.FindContainers( DefaultParams )
i = 0
while i < len( Containers ) :
    print "Container running on",Containers[ i ]._get_machineName(),"with name",Containers[ i ]._get_name(),"and type",Containers[ i ].type()
    i = i + 1

# ------------------------AddComponentServer--------------------------------------------------

DefaultParams.ContainerType = Engines.UndefinedContainerType
DefaultParams.ContainerName = 'AddComponentServer'
aContainer = MyContainersMgr.FindOrStartContainer( DefaultParams )
print "Container running on",aContainer._get_machineName(),"with name",aContainer._get_name(),"and type",aContainer.type()

ContainerParameters( DefaultParams )

# ------------------------FactoryServers--------------------------------------------------

DefaultParams.ContainerName = 'FactoryServer'
DefaultParams.ContainerType = Engines.UndefinedContainerType
Containers = MyContainersMgr.FindContainers( DefaultParams )
i = 0
while i < len( Containers ) :
    print "Container running on",Containers[ i ]._get_machineName(),"with name",Containers[ i ]._get_name(),"and type",Containers[ i ].type()
    i = i + 1


# ------------------------AddComponent--------------------------------------------------

DefaultParams.ContainerType = Engines.CppContainer
DefaultParams.ContainerName = 'FactoryServer'
FactoryServerAddComponent = MyContainersMgr.FindOrLoad_Component( DefaultParams , "AddComponent" )
print FactoryServerAddComponent.GetContainerRef()._get_name(),FactoryServerAddComponent._get_instanceName(),FactoryServerAddComponent._get_interfaceName()


# ------------------------is109973 Containers--------------------------------------------------

DefaultParams.ContainerType = Engines.UndefinedContainerType
DefaultParams.ContainerName = ''
DefaultParams.HostName = 'is109973'
Containers = MyContainersMgr.FindContainers( DefaultParams )
i = 0
while i < len( Containers ) :
    print "Container running on",Containers[ i ]._get_machineName(),"with name",Containers[ i ]._get_name(),"and type",Containers[ i ].type()
    i = i + 1

# ------------------------is109974 Containers--------------------------------------------------

DefaultParams.HostName = 'is109974'
Containers = MyContainersMgr.FindContainers( DefaultParams )
i = 0
while i < len( Containers ) :
    print "Container running on",Containers[ i ]._get_machineName(),"with name",Containers[ i ]._get_name(),"and type",Containers[ i ].type()
    i = i + 1

# ------------------------is109974FactoryServerPy--------------------------------------------------

DefaultParams.Memory = 500
DefaultParams.HostName = ''
DefaultParams.ContainerType = Engines.PythonContainer
is109974FactoryServerPy = MyContainersMgr.FindOrStartContainer( DefaultParams )

is109974FactoryServerPy.ping()

# ------------------------Python Containers--------------------------------------------------

DefaultParams.Memory = 0
DefaultParams.ContainerType = Engines.PythonContainer
DefaultParams.ContainerName = ''
Containers = MyContainersMgr.FindContainers( DefaultParams )
i = 0
while i < len( Containers ) :
    print "Container running on",Containers[ i ]._get_machineName(),"with name",Containers[ i ]._get_name(),"and type",Containers[ i ].type()
    i = i + 1

# ------------------------Python FactorialComponent--------------------------------------------------

DefaultParams.Memory = 500
aFactorialComponent = MyContainersMgr.FindOrLoad_Component( DefaultParams , "FactorialComponent" )
print aFactorialComponent.GetContainerRef()._get_name(),aFactorialComponent._get_instanceName(),aFactorialComponent._get_interfaceName()


# ------------------------RunningComponents--------------------------------------------------

ListComponents()




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

FactoryServerSubComponent = MyContainersMgr.FindOrLoad_Component( DefaultParams , "SubComponent" )
ListOfComponents = MyContainersMgr.FindComponents( DefaultParams , '' )
i = 0
while i < len( ListOfComponents ) :
    print ListOfComponents[ i ].GetContainerRef()._get_name(),ListOfComponents[ i ]._get_instanceName(),ListOfComponents[ i ]._get_interfaceName()
    i = i + 1

DefaultParams.ContainerName = 'ServerPy'
ServerPy = FindOrStartContainer( DefaultParams )
ServerPy = FindOrStartContainer( DefaultParams )
ServerPy = MyContainersMgr.FindContainer( DefaultParams )
print "Container running on",ServerPy._get_machineName(),"with name",ServerPy._get_name(),"and type",ServerPy.type()

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


ListOfComponents = MyContainersMgr.FindComponents( DefaultParams , 'SubComponent' )
i = 0
while i < len( ListOfComponents ) :
    print ListOfComponents[ i ].GetContainerRef()._get_name(),ListOfComponents[ i ]._get_instanceName(),ListOfComponents[ i ]._get_interfaceName()
    i = i + 1

aAddComponent = MyContainersMgr.FindOneComponent( '' , 'FactoryServer' , "AddComponent" )
print aAddComponent.GetContainerRef()._get_name(),aAddComponent._get_instanceName(),aAddComponent._get_interfaceName()

aSubComponent = MyContainersMgr.FindOneComponent( '' , 'FactoryServer' , "SubComponent" )
print aSubComponent.GetContainerRef()._get_name(),aSubComponent._get_instanceName(),aSubComponent._get_interfaceName()

aSUPERVComponent = MyContainersMgr.FindOneComponent( '' , 'SuperVisionContainer' , "SUPERV" )
print aSUPERVComponent.GetContainerRef()._get_name(),aSUPERVComponent._get_instanceName(),aSUPERVComponent._get_interfaceName()

aAddComponent = MyContainersMgr.FindOneComponent( '' , '' , "AddComponent" )
print aAddComponent.GetContainerRef()._get_name(),aAddComponent._get_instanceName(),aAddComponent._get_interfaceName()

aSubComponent = MyContainersMgr.FindOneComponent( '' , '' , "SubComponent" )
print aSubComponent.GetContainerRef()._get_name(),aSubComponent._get_instanceName(),aSubComponent._get_interfaceName()

aSubComponent = MyContainersMgr.FindOneComponent( 'is109973' , '' , "SubComponent" )
print aSubComponent.GetContainerRef()._get_name(),aSubComponent._get_instanceName(),aSubComponent._get_interfaceName()

aSubComponent = MyContainersMgr.FindOneComponent( 'localhost' , '' , "SubComponent" )
print aSubComponent.GetContainerRef()._get_name(),aSubComponent._get_instanceName(),aSubComponent._get_interfaceName()

aSUPERVComponent = MyContainersMgr.FindOneComponent( '' , '' , "SUPERV" )
print aSUPERVComponent.GetContainerRef()._get_name(),aSUPERVComponent._get_instanceName(),aSUPERVComponent._get_interfaceName()

ServerPy = MyContainersMgr.FindOneContainer( '' , 'ServerPy' )
print "Container running on",ServerPy._get_machineName(),"with name",ServerPy._get_name(),"and type",ServerPy.type()

DefaultParams.ContainerType = Engines.PythonContainer
ContainerParameters( DefaultParams )
TestComponentPy = MyContainersMgr.FindOrLoad_Component( DefaultParams , "SALOME_TestComponentPy" )
print TestComponentPy.GetContainerRef()._get_name(),TestComponentPy._get_instanceName(),TestComponentPy._get_interfaceName()
ListOfComponents = MyContainersMgr.FindComponents( DefaultParams , '' )
i = 0
while i < len( ListOfComponents ) :
    print ListOfComponents[ i ].GetContainerRef()._get_name(),ListOfComponents[ i ]._get_instanceName(),ListOfComponents[ i ]._get_interfaceName()
    i = i + 1

DefaultParams.ContainerType = Engines.CppContainer
ContainerParameters( DefaultParams )
TestComponent = MyContainersMgr.FindOrLoad_Component( DefaultParams , "SalomeTestComponent" )
print TestComponent.GetContainerRef()._get_name(),TestComponent._get_instanceName(),TestComponent._get_interfaceName()
ListOfComponents = MyContainersMgr.FindComponents( DefaultParams , '' )
i = 0
while i < len( ListOfComponents ) :
    print ListOfComponents[ i ].GetContainerRef()._get_name(),ListOfComponents[ i ]._get_instanceName(),ListOfComponents[ i ]._get_interfaceName()
    i = i + 1





from LifeCycleCORBA import *
orb = CORBA.ORB_init([''], CORBA.ORB_ID)
lcc = LifeCycleCORBA( orb )

TestComponent  = lcc.FindOrLoadComponent( 'FactoryServer' , 'SalomeTestComponent' )
print TestComponent.GetContainerRef()._get_name(),TestComponent._get_instanceName(),TestComponent._get_interfaceName()

TestComponent  = lcc.FindOrLoadComponent( '' , 'SalomeTestComponent' )
print TestComponent.GetContainerRef()._get_name(),TestComponent._get_instanceName(),TestComponent._get_interfaceName()

TestComponentPy  = lcc.FindOrLoadComponent( '' , 'SALOME_TestComponentPy' )
if TestComponentPy is None :
    print 'SALOME_TestComponentPy not found : Ok'
else :
    print 'Error',TestComponentPy.GetContainerRef()._get_name(),TestComponentPy._get_instanceName(),TestComponentPy._get_interfaceName(),'was found'

TestComponentPy  = lcc.FindOrLoadComponent( 'ServerPy' , 'SALOME_TestComponentPy' )
print TestComponentPy.GetContainerRef()._get_name(),TestComponentPy._get_instanceName(),TestComponentPy._get_interfaceName()

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


MyContainersMgr.DestroyContainer( '' , 'FactoryServer' )
MyContainersMgr.DestroyContainer( '' , 'FactoryServerPy' )


DefaultParams.HostName = ''
DefaultParams.ContainerName = ''
DefaultParams.ContainerType = Engines.UndefinedContainerType

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


MyContainersMgr.destroy()

