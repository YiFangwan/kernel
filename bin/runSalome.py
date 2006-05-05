#!/usr/bin/env python

import sys, os, string, glob, time, pickle
from server import *
import orbmodule
import setenv

# -----------------------------------------------------------------------------

from killSalome import killAllPorts

def killLocalPort():
    """
    kill servers from a previous SALOME exection, if needed,
    on the CORBA port given in args of runSalome
    """
    
    from killSalomeWithPort import killMyPort
    my_port=str(args['port'])
    try:
        killMyPort(my_port)
    except:
        print "problem in killLocalPort()"
        pass
    pass
    
def givenPortKill(port):
    """
    kill servers from a previous SALOME exection, if needed,
    on the same CORBA port
    """
    
    from killSalomeWithPort import killMyPort
    my_port=port
    try:
        killMyPort(my_port)
    except:
        print "problem in LocalPortKill(), killMyPort("<<port<<")"
        pass
    pass

def kill_salome(args):
    """
    Kill servers from previous SALOME executions, if needed;
    depending on args 'killall' or 'portkill', kill all executions,
    or only execution on the same CORBA port
    """

    if args['killall']:
        killAllPorts()
    elif args['portkill']:
        givenPortKill(str(args['port']))
	

class InterpServer(Server):
    def __init__(self,args):
        self.args=args
        env_ld_library_path=['env', 'LD_LIBRARY_PATH=' + os.getenv("LD_LIBRARY_PATH")]
        self.CMD=['xterm', '-e']+ env_ld_library_path + ['python']
        #self.CMD=['xterm', '-e', 'python']
       
    def run(self):
        global process_id
        command = self.CMD
        print "command = ", command
	if sys.platform == "win32":
          import win32pm
          pid = win32pm.spawnpid( string.join(command, " "),'-nc' )
	else:
          pid = os.spawnvp(os.P_NOWAIT, command[0], command)
        process_id[pid]=self.CMD
        self.PID = pid

# ---

class CatalogServer(Server):
    def __init__(self,args):
        self.args=args
        self.initArgs()
	#if sys.platform == "win32":
  #        self.SCMD1=[os.environ["KERNEL_ROOT_DIR"] + "/win32/" + os.environ["BIN_ENV"] + "/" + 'SALOME_ModuleCatalog_Server' + ".exe",'-common']
	#else:
        self.SCMD1=['SALOME_ModuleCatalog_Server','-common']
        self.SCMD2=[]
        home_dir=os.getenv('HOME')
        if home_dir is not None:
            self.SCMD2=['-personal',os.path.join(home_dir,'Salome/resources/CatalogModulePersonnel.xml')] 

    def setpath(self,modules_list,modules_root_dir):
        cata_path=[]
        list_modules = modules_list[:]
        list_modules.reverse()
        if self.args["gui"] :
            list_modules = ["KERNEL", "GUI"] + list_modules
        else :
            list_modules = ["KERNEL"] + list_modules
        for module in list_modules:
            if modules_root_dir.has_key(module):
                module_root_dir=modules_root_dir[module]
                module_cata=module+"Catalog.xml"
                #print "   ", module_cata
                cata_path.extend(
                    glob.glob(os.path.join(module_root_dir,
                                           "share",setenv.salome_subdir,
                                           "resources",module_cata)))
                pass
            pass
        self.CMD=self.SCMD1 + [string.join(cata_path,':')] + self.SCMD2

# ---

class SalomeDSServer(Server):
    def __init__(self,args):
        self.args=args
        self.initArgs()
#	if sys.platform == "win32":
#          self.CMD=[os.environ["KERNEL_ROOT_DIR"] + "/win32/" + os.environ["BIN_ENV"] + "/" + 'SALOMEDS_Server' + ".exe"]
#	else:
        self.CMD=['SALOMEDS_Server']

# ---

class RegistryServer(Server):
    def __init__(self,args):
        self.args=args
        self.initArgs()
#	if sys.platform == "win32":
#          self.CMD=[os.environ["KERNEL_ROOT_DIR"] + "/win32/" + os.environ["BIN_ENV"] + "/" + 'SALOME_Registry_Server'+ ".exe", '--salome_session','theSession']
#	else:
        self.CMD=['SALOME_Registry_Server', '--salome_session','theSession']

# ---

class ContainerCPPServer(Server):
    def __init__(self,args):
        self.args=args
        self.initArgs()
#	if sys.platform == "win32":
#          self.CMD=[os.environ["KERNEL_ROOT_DIR"] + "/win32/" + os.environ["BIN_ENV"] + "/" + 'SALOME_Container' + ".exe",'FactoryServer']
#	else:
        self.CMD=['SALOME_Container','FactoryServer']

# ---

class ContainerPYServer(Server):
    def __init__(self,args):
        self.args=args
        self.initArgs()
	if sys.platform == "win32":
          self.CMD=[os.environ["PYTHONBIN"], os.environ["KERNEL_ROOT_DIR"] + '/win32/python/SALOME_ContainerPy.py','FactoryServerPy']
	else:
          self.CMD=['SALOME_ContainerPy.py','FactoryServerPy']

# ---

class ContainerSUPERVServer(Server):
    def __init__(self,args):
        self.args=args
        self.initArgs()
#	if sys.platform == "win32":
#          self.CMD=[os.environ["KERNEL_ROOT_DIR"] + "/win32/" + os.environ["BIN_ENV"] + "/" + 'SALOME_Container' + ".exe",'SuperVisionContainer']
#	else:
        self.CMD=['SALOME_Container','SuperVisionContainer']

# ---

class LoggerServer(Server):
    def __init__(self,args):
        self.args=args
        self.initArgs()
        self.CMD=['SALOME_Logger_Server', 'logger.log']

# ---

class SessionServer(Server):
    def __init__(self,args):
        self.args = args.copy()
        # Bug 11512 (Problems with runSalome --xterm on Mandrake and Debian Sarge)
        self.args['xterm']=0
        #
        self.initArgs()
#	if sys.platform == "win32":
#          self.SCMD1=[os.environ["GUI_ROOT_DIR"] + "/win32/" + os.environ["BIN_ENV"] + "/" + 'SALOME_Session_Server' + ".exe"]
#	else:
        self.SCMD1=['SALOME_Session_Server']
	
        self.SCMD2=[]
        if 'registry' in self.args['embedded']:
            self.SCMD1+=['--with','Registry',
                         '(','--salome_session','theSession',')']
        if 'moduleCatalog' in self.args['embedded']:
            self.SCMD1+=['--with','ModuleCatalog','(','-common']
            home_dir=os.getenv('HOME')
            if home_dir is not None:
                self.SCMD2+=['-personal',os.path.join(home_dir,'Salome/resources/CatalogModulePersonnel.xml')] 
            self.SCMD2+=[')']
        if 'study' in self.args['embedded']:
            self.SCMD2+=['--with','SALOMEDS','(',')']
        if 'cppContainer' in self.args['embedded']:
            self.SCMD2+=['--with','Container','(','FactoryServer',')']
        if 'SalomeAppEngine' in self.args['embedded']:
            self.SCMD2+=['--with','SalomeAppEngine','(',')']
            
        if 'cppContainer' in self.args['standalone'] or 'cppContainer' in self.args['embedded']:
            self.SCMD2+=['CPP']
        if 'pyContainer' in self.args['standalone'] or 'pyContainer' in self.args['embedded']:
            self.SCMD2+=['PY']
        if 'supervContainer' in self.args['containers'] or 'supervContainer' in self.args['standalone']:
            self.SCMD2+=['SUPERV']
        if self.args['gui']:
            self.SCMD2+=['GUI']
        if self.args['splash'] and self.args['gui']:
            self.SCMD2+=['SPLASH']
        if self.args['noexcepthandler']:
            self.SCMD2+=['noexcepthandler']
        if self.args.has_key('modules'):
            self.SCMD2+=['--modules (']
            for mod in self.args['modules']:
                self.SCMD2+=[mod + ':']
            self.SCMD2+=[')']    

    def setpath(self,modules_list,modules_root_dir):
        cata_path=[]
        list_modules = modules_list[:]
        list_modules.reverse()
        if self.args["gui"] :
            list_modules = ["KERNEL", "GUI"] + list_modules
        else :
            list_modules = ["KERNEL"] + list_modules
        for module in list_modules:
            module_root_dir=modules_root_dir[module]
            module_cata=module+"Catalog.xml"
            #print "   ", module_cata
            cata_path.extend(
                glob.glob(os.path.join(module_root_dir,"share",
                                       setenv.salome_subdir,"resources",
                                       module_cata)))
        if (self.args["gui"]) & ('moduleCatalog' in self.args['embedded']):
            self.CMD=self.SCMD1 + [string.join(cata_path,':')] + self.SCMD2
        else:
            self.CMD=self.SCMD1 + self.SCMD2
      
# ---

class ContainerManagerServer(Server):
    def __init__(self,args):
        self.args=args
        self.initArgs()
#	if sys.platform == "win32":
#          self.SCMD1=[os.environ["KERNEL_ROOT_DIR"] + "/win32/" + os.environ["BIN_ENV"] + "/" + 'SALOME_ContainerManagerServer' + ".exe"]
#	else:
        self.SCMD1=['SALOME_ContainerManagerServer']
        self.SCMD2=[]
        if args["gui"] :
            if 'registry' in self.args['embedded']:
                self.SCMD1+=['--with','Registry',
                             '(','--salome_session','theSession',')']
            if 'moduleCatalog' in self.args['embedded']:
                self.SCMD1+=['--with','ModuleCatalog','(','-common']
                self.SCMD2+=['-personal',
                             '${HOME}/Salome/resources/CatalogModulePersonnel.xml',')']
            if 'study' in self.args['embedded']:
                self.SCMD2+=['--with','SALOMEDS','(',')']
            if 'cppContainer' in self.args['embedded']:
                self.SCMD2+=['--with','Container','(','FactoryServer',')']
        
    def setpath(self,modules_list,modules_root_dir):
        cata_path=[]
        list_modules = modules_list[:]
        list_modules.reverse()
        if self.args["gui"] :
            list_modules = ["GUI"] + list_modules
        for module in ["KERNEL"] + list_modules:
            if modules_root_dir.has_key(module):
                module_root_dir=modules_root_dir[module]
                module_cata=module+"Catalog.xml"
                #print "   ", module_cata
                cata_path.extend(
                    glob.glob(os.path.join(module_root_dir,"share",
                                           self.args['appname'],"resources",
                                           module_cata)))
                pass
            pass
        if (self.args["gui"]) & ('moduleCatalog' in self.args['embedded']):
            self.CMD=self.SCMD1 + [string.join(cata_path,':')] + self.SCMD2
        else:
            self.CMD=self.SCMD1 + self.SCMD2

class NotifyServer(Server):
    def __init__(self,args,modules_root_dir):
        self.args=args
        self.initArgs()
        self.modules_root_dir=modules_root_dir
        myLogName = os.environ["LOGNAME"]
        self.CMD=['notifd','-c',
                  self.modules_root_dir["KERNEL"] +'/share/salome/resources/channel.cfg',
                  '-DFactoryIORFileName=/tmp/'+myLogName+'_rdifact.ior',
                  '-DChannelIORFileName=/tmp/'+myLogName+'_rdichan.ior',
                  '-DReportLogFile=/tmp/'+myLogName+'_notifd.report',
                  '-DDebugLogFile=/tmp/'+myLogName+'_notifd.debug',
                  ]

#
# -----------------------------------------------------------------------------

def startGUI():
    """Salome Session Graphic User Interface activation"""
    import Engines
    import SALOME
    import SALOMEDS
    import SALOME_ModuleCatalog
    reload(Engines)
    reload(SALOME)
    reload(SALOMEDS)
    import SALOME_Session_idl
    session=clt.waitNS("/Kernel/Session",SALOME.Session)
    session.GetInterface()
  
# -----------------------------------------------------------------------------

def startSalome(args, modules_list, modules_root_dir):
    """Launch all SALOME servers requested by args"""
    init_time = os.times()

    print "startSalome ", args
    
    #
    # Initialisation ORB et Naming Service
    #
   
    clt=orbmodule.client(args)

    # (non obligatoire) Lancement Logger Server
    # et attente de sa disponibilite dans le naming service
    #

    if args['logger']:
        myServer=LoggerServer(args)
        myServer.run()
        clt.waitLogger("Logger")

    # Notify Server launch
    #

#    print "Notify Server to launch"

#    myServer=NotifyServer(args,modules_root_dir)
#    myServer.run()

    # Lancement Session Server (to show splash ASAP)
    #

    if args["gui"]:
        mySessionServ = SessionServer(args)
        mySessionServ.setpath(modules_list,modules_root_dir)
        mySessionServ.run()

    #
    # Lancement Registry Server,
    # attente de la disponibilite du Registry dans le Naming Service
    #

    if ('registry' not in args['embedded']) | (args["gui"] == 0) :
        myServer=RegistryServer(args)
        myServer.run()
        if sys.platform == "win32":
          clt.waitNS("/Registry")
        else:
          clt.waitNSPID("/Registry",myServer.PID)

    #
    # Lancement Catalog Server,
    # attente de la disponibilite du Catalog Server dans le Naming Service
    #
    

    if ('moduleCatalog' not in args['embedded']) | (args["gui"] == 0):
        cataServer=CatalogServer(args)
        cataServer.setpath(modules_list,modules_root_dir)
        cataServer.run()
        import SALOME_ModuleCatalog
        if sys.platform == "win32":
          clt.waitNS("/Kernel/ModulCatalog",SALOME_ModuleCatalog.ModuleCatalog)
        else:
          clt.waitNSPID("/Kernel/ModulCatalog",cataServer.PID,SALOME_ModuleCatalog.ModuleCatalog)

    #
    # Lancement SalomeDS Server,
    # attente de la disponibilite du SalomeDS dans le Naming Service
    #

    os.environ["CSF_PluginDefaults"] \
    = os.path.join(modules_root_dir["KERNEL"],"share",
                   setenv.salome_subdir,"resources")
    os.environ["CSF_SALOMEDS_ResourcesDefaults"] \
    = os.path.join(modules_root_dir["KERNEL"],"share",
                   setenv.salome_subdir,"resources")

    if "GEOM" in modules_list:
        print "GEOM OCAF Resources"
        os.environ["CSF_GEOMDS_ResourcesDefaults"] \
        = os.path.join(modules_root_dir["GEOM"],"share",
                       setenv.salome_subdir,"resources")
	print "GEOM Shape Healing Resources"
        os.environ["CSF_ShHealingDefaults"] \
        = os.path.join(modules_root_dir["GEOM"],"share",
                       setenv.salome_subdir,"resources")

    print "ARGS = ",args
    if ('study' not in args['embedded']) | (args["gui"] == 0):
        print "RunStudy"
        myServer=SalomeDSServer(args)
        myServer.run()
        if sys.platform == "win32":
          clt.waitNS("/myStudyManager")
        else:
          clt.waitNSPID("/myStudyManager",myServer.PID)

    #
    # Lancement ContainerManagerServer
    #
    
    myCmServer = ContainerManagerServer(args)
    myCmServer.setpath(modules_list,modules_root_dir)
    myCmServer.run()


    from Utils_Identity import getShortHostName
    
    if os.getenv("HOSTNAME") == None:
        if os.getenv("HOST") == None:
            os.environ["HOSTNAME"]=getShortHostName()
        else:
            os.environ["HOSTNAME"]=os.getenv("HOST")

    theComputer = getShortHostName()
    
    #
    # Lancement Container C++ local,
    # attente de la disponibilite du Container C++ local dans le Naming Service
    #

    if 'cppContainer' in args['standalone']:
        myServer=ContainerCPPServer(args)
        myServer.run()
        if sys.platform == "win32":
          clt.waitNS("/Containers/" + theComputer + "/FactoryServer")
        else:
          clt.waitNSPID("/Containers/" + theComputer + "/FactoryServer",myServer.PID)

    #
    # Lancement Container Python local,
    # attente de la disponibilite du Container Python local
    # dans le Naming Service
    #

    if 'pyContainer' in args['standalone']:
        myServer=ContainerPYServer(args)
        myServer.run()
        if sys.platform == "win32":
          clt.waitNS("/Containers/" + theComputer + "/FactoryServerPy")
        else:
          clt.waitNSPID("/Containers/" + theComputer + "/FactoryServerPy",myServer.PID)

    #
    # Lancement Container Supervision local,
    # attente de la disponibilite du Container Supervision local
    # dans le Naming Service
    #

    if 'supervContainer' in args['standalone']:
        myServer=ContainerSUPERVServer(args)
        myServer.run()
        if sys.platform == "win32":
          clt.waitNS("/Containers/" + theComputer + "/SuperVisionContainer")
        else:
          clt.waitNSPID("/Containers/" + theComputer + "/SuperVisionContainer",myServer.PID)
  
    #
    # Attente de la disponibilite du Session Server dans le Naming Service
    #
    
    if args["gui"]:
##----------------        
        import Engines
        import SALOME
        import SALOMEDS
        import SALOME_ModuleCatalog
        reload(Engines)
        reload(SALOME)
        reload(SALOMEDS)
        import SALOME_Session_idl
        if sys.platform == "win32":
          session=clt.waitNS("/Kernel/Session",SALOME.Session)
        else:
          session=clt.waitNSPID("/Kernel/Session",mySessionServ.PID,SALOME.Session)

    end_time = os.times()
    print
    print "Start SALOME, elapsed time : %5.1f seconds"% (end_time[4]
                                                         - init_time[4])

    # ASV start GUI without Loader
    #if args['gui']:
    #    session.GetInterface()

    #
    # additionnal external python interpreters
    #
    nbaddi=0
    
    try:
        if 'interp' in args:
            if args['interp']:
                nbaddi = int(args['interp'][0])
    except:
        import traceback
        traceback.print_exc()
        print "-------------------------------------------------------------"
        print "-- to get an external python interpreter:runSalome --interp=1"
        print "-------------------------------------------------------------"
        
    print "additional external python interpreters: ", nbaddi
    if nbaddi:
        for i in range(nbaddi):
            print "i=",i
            anInterp=InterpServer(args)
            anInterp.run()
    
    return clt

# -----------------------------------------------------------------------------

def useSalome(args, modules_list, modules_root_dir):
    """
    Launch all SALOME servers requested by args,
    save list of process, give info to user,
    show registered objects in Naming Service.
    """
    global process_id
    
    clt=None
    try:
        clt = startSalome(args, modules_list, modules_root_dir)
    except:
        import traceback
        traceback.print_exc()
        print
        print
        print "--- erreur au lancement Salome ---"
        
    #print process_id

    from killSalomeWithPort import getPiDict
    filedict = getPiDict(args['port'])

    process_ids = []
    try:
        fpid=open(filedict, 'r')
        process_ids=pickle.load(fpid)
        fpid.close()
    except:
        pass
    
    fpid=open(filedict, 'w')
    process_ids.append(process_id)
    pickle.dump(process_ids,fpid)
    fpid.close()
    
    print """
    Saving of the dictionary of Salome processes in %s
    To kill SALOME processes from a console (kill all sessions from all ports):
      python killSalome.py 
    To kill SALOME from the present interpreter, if it is not closed :
      killLocalPort()      --> kill this session
                               (use CORBA port from args of runSalome)
      givenPortKill(port)  --> kill a specific session with given CORBA port 
      killAllPorts()       --> kill all sessions
    
    runSalome, with --killall option, starts with killing
    the processes resulting from the previous execution.
    """%filedict
    
    #
    #  Impression arborescence Naming Service
    #
    
    if clt != None:
        print
        print " --- registered objects tree in Naming Service ---"
        clt.showNS()

    return clt

# -----------------------------------------------------------------------------

def registerEnv(args, modules_list, modules_root_dir):
    """
    Register args, modules_list, modules_root_dir in a file
    for further use, when SALOME is launched embedded in an other application.
    """
    fileEnv = '/tmp/' + os.getenv('USER') + "_" + str(args['port']) \
            + '_' + args['appname'].upper() + '_env'
    fenv=open(fileEnv,'w')
    pickle.dump((args, modules_list, modules_root_dir),fenv)
    fenv.close()
    os.environ["SALOME_LAUNCH_CONFIG"] = fileEnv

# -----------------------------------------------------------------------------

def no_main():
    """Salome Launch, when embedded in other application"""
    fileEnv = os.environ["SALOME_LAUNCH_CONFIG"]
    fenv=open(fileEnv,'r')
    args, modules_list, modules_root_dir = pickle.load(fenv)
    fenv.close()
    kill_salome(args)
    clt = useSalome(args, modules_list, modules_root_dir)
    return clt

# -----------------------------------------------------------------------------

def main():
    """Salome launch as a main application"""
    args, modules_list, modules_root_dir = setenv.get_config()
    kill_salome(args)
    #invokation of set_env moved to separate file setenv.py
    #set_env(args, modules_list, modules_root_dir)
    setenv.main()
    clt = useSalome(args, modules_list, modules_root_dir)
    return clt,args

# -----------------------------------------------------------------------------

if __name__ == "__main__":
   import user
   clt,args = main()
