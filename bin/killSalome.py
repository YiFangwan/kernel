#!/usr/bin/env python

import sys,os,pickle,signal

process_id={}

# -----------------------------------------------------------------------------
#
# Fonction d'arrêt de salome
#

def killSalome():
   print "arret du serveur ContainersManager ==> arret des containers"
   import os
   import sys
   import string
   from omniORB import CORBA
   import CosNaming
   import Engines
   import Containers
   try :
      orb = CORBA.ORB_init(sys.argv, CORBA.ORB_ID)
      obj = orb.resolve_initial_references("NameService")
      rootContext = obj._narrow(CosNaming.NamingContext)
      if rootContext is None:
         print "Name Service Reference is invalid"
      else :
         name = [CosNaming.NameComponent("Kernel","dir"),
                 CosNaming.NameComponent("ContainersManager","object")]
         obj = rootContext.resolve( name )
         MyContainersMgr = obj._narrow(Containers.Manager)
         print "MyContainersMgr.ping :",MyContainersMgr.ping()
         MyContainersMgr.destroy()
         MyContainersMgrdestroyed = 1
   except :
      print "Cannot destroy the ContainersManager"
      MyContainersMgrdestroyed = 0

   print "arret des serveurs SALOME"
   for pid, cmd in process_id.items():
      if ((cmd[0] != 'ContainersManager_Server') | (MyContainersMgrdestroyed == 0)) & (cmd[0] != 'startContainer.py') :
         print "arret du process %s : %s"% (pid, cmd[0])
         try:
            os.kill(pid,signal.SIGKILL)
         except:
            print "  ------------------ process %s : %s inexistant"% (pid, cmd[0])
   print "arret du naming service"
   os.system("killall -9 omniNames")
   
# -----------------------------------------------------------------------------

filedict='/tmp/'+os.getenv('USER')+'_SALOME_pidict'
#filedict='/tmp/'+os.getlogin()+'_SALOME_pidict'
try:
   fpid=open(filedict, 'r')
except:
   print "le fichier %s des process SALOME n'est pas accessible"% filedict
   sys.exit(1)
   
process_id=pickle.load(fpid)
fpid.close()

killSalome()

os.remove(filedict)


