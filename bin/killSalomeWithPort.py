#!/usr/bin/env python

# Copyright (C) 2005  OPEN CASCADE, CEA, EDF R&D, LEG
#           PRINCIPIA R&D, EADS CCR, Lip6, BV, CEDRAT
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either 
# version 2.1 of the License.
# 
# This library is distributed in the hope that it will be useful 
# but WITHOUT ANY WARRANTY; without even the implied warranty of 
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
# Lesser General Public License for more details.
# 
# You should have received a copy of the GNU Lesser General Public  
# License along with this library; if not, write to the Free Software 
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
# 
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
# 

import os, sys, pickle, signal, commands

def getPiDict(port,appname='salome',full=True):
    from Utils_Identity import getShortHostName

    # get hostname by special function in all cases to
    # have always same result in lower case at win32
    host = getShortHostName()
    if not host:
        host = os.getenv("HOSTNAME")
    if not host:
        host = os.getenv("HOST")

    filedict = []
    filedict.append( os.getenv('USER') )          # user name
    filedict.append( host )                       # host name
    filedict.append( str(port) )                  # port number
    filedict.append( appname.upper() )            # application name
    filedict.append( 'pidict' )                   # constant part

    filedict = '_'.join(filedict)
    if full:
        filedict = os.getenv("HOME") + '/' + filedict
    return filedict

def appliCleanOmniOrbConfig(port):
    """
    remove omniorb config files related to the port in SALOME application:
    - ${HOME}/${APPLI}/.omniORB_${HOSTNAME}_${NSPORT}.cfg
    - ${HOME}/${APPLI}/.omniORB_${HOSTNAME}_last.cfg
    the last is removed only if the link points to the first file.
    """
    from Utils_Identity import getShortHostName
    appli=os.environ.get("APPLI")
    if appli is None:
        #Run outside application context
        pass
    else:
        home = os.environ['HOME']
        home='%s/%s'%(home,appli)
        hostname=getShortHostName()
        omniorb_config = '%s/.omniORB_%s_%s.cfg'%(home,hostname, str(port))
        last_running_config = '%s/.omniORB_%s_last.cfg'%(home, hostname)
        if os.access(last_running_config,os.F_OK):
            pointedPath = os.readlink(last_running_config)
            if pointedPath[0] != '/':
                pointedPath=os.path.join(os.path.dirname(last_running_config), pointedPath)
            if pointedPath == omniorb_config:
                os.unlink(last_running_config)
                pass
            pass
        if os.access(omniorb_config,os.F_OK):
            os.remove(omniorb_config)
            pass
        pass

########## kills all salome processes with the given port ##########

def killMyPort(port):
    filedict=getPiDict(port)
    found = 0
    try:
        fpid=open(filedict, 'r')
        found = 1
    except:
        print "file %s giving SALOME process id is not readable"% filedict
        pass
        
    if found:
        #cmd = 'pid=`ps -eo pid,command | egrep "[0-9] omniNames -start '+str(port)+'"` ; echo $pid > /tmp/logs/'+os.getenv('USER')+"/_"+port+'_Pid_omniNames.log'
        #a = os.system(cmd)
        try:
            fpidomniNames=open('/tmp/logs/'+os.getenv('USER')+"/_"+port+'_Pid_omniNames.log')
            prc = fpidomniNames.read()
            fpidomniNames.close()
            if prc != None :
                for field in prc.split(" ") :
                    if field == "omniNames" :
                        if pidfield != "egrep" :
                            print 'stop process '+pidfield+' : omniNames'
                            if sys.platform == "win32":
                                import win32pm
                                win32pm.killpid(int(pidfield),0)
                            else:
                                os.system('kill -9 '+pidfield)
                    pidfield = field
        except:
            pass
        
        try:
            process_ids=pickle.load(fpid)
            fpid.close()
            for process_id in process_ids:

                for pid, cmd in process_id.items():
                    print "stop process %s : %s"% (pid, cmd[0])
                    try:
                        if sys.platform == "win32":
                          import win32pm
                          win32pm.killpid(int(pid),0)
                        else:
                           os.kill(int(pid),signal.SIGKILL)
                    except:
                        print "  ------------------ process %s : %s not found"% (pid, cmd[0])
                        pass
                pass
        except:
            pass
        
        os.remove(filedict)
        cmd='ps -eo pid,command | egrep "[0-9] omniNames -start '+str(port)+'" | sed -e "s%[^0-9]*\([0-9]*\) .*%\\1%g"'
        pid = commands.getoutput(cmd)
        a = ""
        while pid != "" and len(a.split(" ")) < 2:
            a = commands.getoutput("pid=`ps -eo pid,command | egrep \"[0-9] omniNames -start "+str(port)+"\" | sed -e \"s%[^0-9]*\([0-9]*\) .*%\\1%g\"`; kill -9 $pid")
            pid = commands.getoutput("ps -eo pid,command | egrep \"[0-9] omniNames -start "+str(port)+"\" | sed -e \"s%[^0-9]*\([0-9]*\) .*%\\1%g\"")
	    print pid
            
        pass

    appliCleanOmniOrbConfig(port)
    pass
            

if __name__ == "__main__":
    for port in sys.argv[1:]:
        killMyPort(port)
