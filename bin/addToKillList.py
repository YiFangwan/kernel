#! /usr/bin/env python
#  -*- coding: iso-8859-1 -*-
# Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
#
# Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
# CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
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

import os, sys, pickle, signal, re, time
from launchConfigureParser import verbose
import subprocess as sp

_ompi_server = "ompi-server"

########## adds to the kill list of SALOME one more process ##########

def findFileDict():
    """
    Detect current SALOME session's port number.
    Returns port number.
    """
    from salome_utils import getPortNumber
    port = getPortNumber()
    if verbose(): print "myport = ", port
    return port

def addToKillList(command_pid, command, port=None):
    """
    Add the process to the SALOME processes dictionary file.
    Parameters:
    - command_pid : command PID
    - command     : command (string or list of strings)
    - [port]      : SALOME port number; if this parameter is None (default),
    it is detected automatically
    """
    # retrieve current processes dictionary
    from killSalomeWithPort import getPiDict
    if port is None: port=findFileDict()
    filedict = getPiDict(port)

    try:
        with open(filedict, 'r') as fpid:
            process_ids=pickle.load(fpid)
    except:
        process_ids=[]
        pass
    # check if PID is already in dictionary
    already_in=False
    for process_id in process_ids:
        for pid in process_id.keys():
            if int(pid) == int(command_pid):
                already_in=True
                break
            pass
        if already_in: break
        pass

    # add process to the dictionary
    if not already_in:
        import types
        if type(command) == types.ListType: command=" ".join([str(c) for c in command])
        command=command.split()[0]
        try:
            if verbose(): print "addToKillList: %s : %s" % ( str(command_pid), command )
            process_ids.append({int(command_pid): [command]})
            dir = os.path.dirname(filedict)
            if not os.path.exists(dir): os.makedirs(dir, 0777)
            with open(filedict,'w') as fpid:
                pickle.dump(process_ids, fpid)
        except:
            if verbose(): print "addToKillList: can not add command %s : %s to the kill list" % ( str(command_pid), command )
            pass
        pass
    pass

def killList(port=None):
    """
    Kill all the processes listed in the SALOME processes dictionary file.
    - [port]      : SALOME port number; if this parameter is None (default),
    it is detected automatically
    """
    # retrieve processes dictionary
    from killSalomeWithPort import getPiDict
    if port is None: port=findFileDict()

    # new-style dot-prefixed pidict file
    filedict=getPiDict(port)
    # provide compatibility with old-style pidict file (not dot-prefixed)
    if not os.path.exists(filedict): filedict = getPiDict(port, hidden=False)

    try:
        with open(filedict, 'r') as fpid:
            process_ids=pickle.load(fpid)
    except:
        process_ids=[]
        pass
    # kill processes
    for process_id in process_ids:
        #print process_id
        for pid, cmd in process_id.items():
            try:
                os.kill(int(pid),signal.SIGKILL)
            except:
                print "  ------------------ process %s : %s inexistant"% (pid, cmd[0])
                pass
            pass
        pass
    # remove processes dictionary file
    os.remove(filedict)
    pass

def isMatchingKey(pid,keyToFind):
    import os
    pat=re.compile("[\s]+")
    p=sp.Popen(["ps","ocmd",str(pid)],stdout=sp.PIPE)
    ret,_=p.communicate()
    if p.returncode != 0:
        return False
    ret=ret.decode("utf-8")
    lines=ret.split("\n")
    if len(lines) != 3:
        return False
    res2=pat.split(lines[1])
    if len(res2) != 3:
        return False
    res2=os.path.basename(res2[-1])
    return res2==keyToFind

def findZePID(key_to_find):
    p=sp.Popen(["pidof",_ompi_server],stdout=sp.PIPE)
    ret,_=p.communicate()
    assert(p.returncode==0)
    ret=ret.decode("utf-8")
    pat=re.compile("[\s]+")
    pat.split(ret)
    pids=[int(elt) for elt in pat.split(ret) if elt!=""]
    zePids=[p for p in pids if isMatchingKey(p,key_to_find)]
    assert(len(zePids)==1)
    return zePids[0]

def findZePIDWait(key_to_find):
    for i in range(10):
        try:
            time.sleep(0.2)
            return findZePID(key_to_find)
        except:
            pass
    assert(False)

if __name__ == "__main__":
    if verbose(): print sys.argv
    if sys.argv[2]!=_ompi_server:
        addToKillList(sys.argv[1], sys.argv[2])
    else:
        true_pid = findZePIDWait(os.path.basename(sys.argv[3]))
        addToKillList(true_pid, sys.argv[2])
    pass
