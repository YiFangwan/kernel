
#ContainersManager_Server -ORBInitRef NameService=corbaname::localhost

import Engines

import Containers

MyContainersMgr = clt.waitNS("/Kernel/ContainersManager",Containers.Manager)

MyContainersMgr.ping()

DefaultParams = MyContainersMgr.Parameters()

print "DefaultParameters :"
print "Os            :",DefaultParams.Os
print "Memory        :",DefaultParams.Memory
print "CpuClock      :",DefaultParams.CpuClock
print "NbProc        :",DefaultParams.NbProc
print "NbNode        :",DefaultParams.NbNode
print "HostName      :",DefaultParams.HostName
print "ContainerName :",DefaultParams.ContainerName
print "ContainerType :",DefaultParams.ContainerType
print "NsHostName    :",DefaultParams.NsHostName
print "NsPort        :",DefaultParams.NsPort

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
    params = AllComputers[i].Parameters()
    print 'FullName   ',params.FullName
    print 'Alias      ',params.Alias
    print 'SshAccess  ',params.SshAccess
    print 'Interactive',params.Interactive
    print 'Batch      ',params.Batch
    print 'UserName   ',params.UserName
    print 'Os         ',params.Os
    print 'Memory     ',params.Memory
    print 'Swap       ',params.Swap
    print 'CpuClock   ',params.CpuClock
    print 'NbProc     ',params.NbProc
    print 'NbNode     ',params.NbNode
    env = AllComputers[i].Environment()
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
    i = i + 1

ListOfComputers = MyResourcesMgr.GetComputers( DefaultParams )
print  ''
print 'ListOfComputers',len( ListOfComputers ),'computers found'
i = 0
while i < len( ListOfComputers ) :
    print  ''
    params = ListOfComputers[i].Parameters()
    print 'FullName   ',params.FullName
    print 'Alias      ',params.Alias
    print 'SshAccess  ',params.SshAccess
    print 'Interactive',params.Interactive
    print 'Batch      ',params.Batch
    print 'UserName   ',params.UserName
    print 'Os         ',params.Os
    print 'Memory     ',params.Memory
    print 'Swap       ',params.Swap
    print 'CpuClock   ',params.CpuClock
    print 'NbProc     ',params.NbProc
    print 'NbNode     ',params.NbNode
    env = ListOfComputers[i].Environment()
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
    i = i + 1

aComputer = MyResourcesMgr.SelectComputer( DefaultParams )
aComputer.Parameters().FullName

aComputer = MyResourcesMgr.GetComputer( ListOfComputers )
aComputer.Parameters().FullName

aComputer = MyResourcesMgr.SearchComputer("bordolex")
aComputer.Parameters().FullName

aComputer = MyResourcesMgr.SearchComputer("bojolex")
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
    params = ListOfComputers[i].Parameters()
    print 'FullName   ',params.FullName
    print 'Alias      ',params.Alias
    print 'SshAccess  ',params.SshAccess
    print 'Interactive',params.Interactive
    print 'Batch      ',params.Batch
    print 'UserName   ',params.UserName
    print 'Os         ',params.Os
    print 'Memory     ',params.Memory
    print 'Swap       ',params.Swap
    print 'CpuClock   ',params.CpuClock
    print 'NbProc     ',params.NbProc
    print 'NbNode     ',params.NbNode
    env = ListOfComputers[i].Environment()
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
    params = ListOfComputers[i].Parameters()
    print 'FullName   ',params.FullName
    print 'Alias      ',params.Alias
    print 'SshAccess  ',params.SshAccess
    print 'Interactive',params.Interactive
    print 'Batch      ',params.Batch
    print 'UserName   ',params.UserName
    print 'Os         ',params.Os
    print 'Memory     ',params.Memory
    print 'Swap       ',params.Swap
    print 'CpuClock   ',params.CpuClock
    print 'NbProc     ',params.NbProc
    print 'NbNode     ',params.NbNode
    env = ListOfComputers[i].Environment()
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

component = MyContainersMgr.FindOrLoadComponent( DefaultParams , "AddComponent" )

