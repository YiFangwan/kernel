#!/usr/bin/env python

"""
   This module is used to send execution traces to a server.
"""

import sys
import os
import json
import urllib2
import urllib

class TracesSender(object):
  """
     Collect and send STUC traces to a server.
  """
  
  def loadLogFile(self, filePath):
    """
       Load STUC traces from a log file.
       
       STUC traces have this format:
       [STUC TRACE]{ "label":"name","begin":"2014-07-24 14:18:45","end":"2014-07-24 14:18:47"}
       
       Lines which don't containt the flag "[STUC TRACE]" are ignored.
       Between [STUC TRACE] and the end of the line, there should be 
       the trace data in json format.
    """
    self.traces = []
    logFile = open(filePath)
    flag = '[STUC TRACE]'
    for line in logFile:
      x, y, p = line.partition(flag)
      if p != '':
        trace = json.loads(p)
        self.traces.append(trace)
      pass
    logFile.close()
    pass
  
  def setInfo(self, appName, appVersion, installPath):
    """
       Informations to join to the traces.
       
       :param appName: name of the current application
       :param appVersion: version of the current application
       :param installPath: installation path of the current application
    """
    self.appName = appName
    self.appVersion = appVersion
    self.installPath = installPath
    pass
  
  def send(self, serverUrl):
    """
       Send STUC traces to a server.
       
       :param serverUrl: complet url where the traces should be sent
    """
    session = {'appName'    : self.appName,
               'appVersion' : self.appVersion,
               'installPath': self.installPath,
               'traces'     : json.dumps(self.traces)}
    dataToSend = urllib.urlencode(session)
#    headers = {"Content-type": "application/x-www-form-urlencoded",
#               "Accept": "text/plain"}
#    req = urllib2.Request(serverUrl, dataToSend,headers )
    req = urllib2.Request(serverUrl, dataToSend)
    response = urllib2.urlopen(req)
    return response
  
  pass

def sendSalome(logfilepath):
  """
     TracesSender usecase for SALOME
  """
  ts = TracesSender()
  salome_path = os.getenv("ROOT_SALOME")
  kernel_path = os.getenv('KERNEL_ROOT_DIR', '' )
  version_path = os.path.join(kernel_path, "bin", "salome", "VERSION")
  version_file = open(version_path)
  version = "unknown"
  for line in version_file:
    x, y, p = line.partition("[SALOME KERNEL] :")
    if p != '':
      version = p
      break
      pass
    pass
  
  ts.setInfo("SALOME", version, salome_path)
  ts.loadLogFile(logfilepath)
  ts.send("http://localhost:8000/cgi-bin/traces.py")

### MAIN ##
if __name__ == "__main__":
  try:
    serverUrl = sys.argv[1]
    appName = sys.argv[2]
    appVersion = sys.argv[3]
    installPath = sys.argv[4]
    logFile = sys.argv[5]
  except:
    print "usage:"
    print "  sendTraces.py <serverUrl> <appName> <appVersion> <installPath> <logFile>"
    exit()
    pass
  
  ts = TracesSender()
  ts.setInfo(appName, appVersion, installPath)
  ts.loadLogFile(logFile)
  ts.send(serverUrl)
