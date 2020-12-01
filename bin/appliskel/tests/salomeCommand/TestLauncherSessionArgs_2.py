#!/usr/bin/env python3
# Copyright (C) 2013-2020  CEA/DEN, EDF R&D, OPEN CASCADE
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

import unittest

import os
import sys
import imp
from io import StringIO
import logging

logger = logging.getLogger("TestLauncherLogger")
logger.level = logging.DEBUG
logger.addHandler(logging.StreamHandler())

class TestSessionArgs(unittest.TestCase):
  #
  @classmethod
  def setUpClass(cls):
    # Set some predefined command args and corresponding output messages
    cls.hello0 = ["hello.py", "args:outfile=LOGFILE"]
    cls.hello0Msg = "Hello!"
    cls.hello1 = ["hello.py", "args:you,outfile=LOGFILE"]
    cls.hello1Msg = "Hello to: you"
    cls.helloToAdd = ["hello.py", "args:add.py,1,2,3,outfile=LOGFILE"]
    cls.helloToAddMsg = "Hello to: add.py, 1, 2, 3"
    cls.helloToList = ["hello.py", "args:['file1','file2'],1,2,3,[True,False],'ok',outfile=LOGFILE"]
    cls.helloToListMsg = "Hello to: ['file1','file2'], 1, 2, 3, [True,False], 'ok'"
    cls.add0 = ["add.py", "args:outfile=LOGFILE"]
    cls.add0Msg = "No args!"
    cls.add3 = ["add.py", "args:1,2,3,outfile=LOGFILE"]
    cls.add3Msg = "1+2+3 = 6"
    cls.lines0 = ["lines.py", "args:outfile=LOGFILE"]
    cls.lines0Msg = "No files given"
    cls.lines2 = ["lines.py", "args:hello.py,add.py,outfile=LOGFILE"]
    cls.lines2Msg = "hello.py is 35 lines longadd.py is 37 lines long"
    cls.linesUnreadable = ["lines.py", "args:hello.py,add.py,1,2,outfile=LOGFILE"]
    cls.linesUnreadableMsg = "hello.py is 35 lines longadd.py is 37 lines longFile '1' cannot be readFile '2' cannot be read"
    sys.stdout = StringIO()
  #
  def setUp(self):
    import tempfile
    from salomeContextUtils import setOmniOrbUserPath
    setOmniOrbUserPath()
    self.logFile = tempfile.NamedTemporaryFile()
    if sys.platform == "win32": # Close file because of permission denined on Windows
        self.logFile.close()
  #
  def tearDown(self):
    self.logFile.close()
  #
  def session(self, args=None):
    if args is None:
      args = []
    args = [x.replace("LOGFILE",self.logFile.name) for x in args]
    try:
      import setenv
      setenv.main(True)
      import runSession
      params, args = runSession.configureSession(args, exe="salome shell")
      return runSession.runSession(params, args)
    except SystemExit as e:
      if str(e) != '0':
        logger.error(e)
      import traceback
      traceback.print_exc()
      pass
  #
  def assertLogFileContentsEqual(self, message):
    with open(self.logFile.name, "r") as f:
      contents = f.read().replace('\n', '')

    self.assertTrue(contents==message, "Contents differ!\n\tGenerated contents: %s\n\tExpected contents: %s"%(contents, message))
  #
  def testHelloToList(self):
    self.session(self.helloToList)
    self.assertLogFileContentsEqual(self.helloToListMsg)
  #
  def testLines0(self):
    self.session(self.lines0)
    self.assertLogFileContentsEqual(self.lines0Msg)
  #
  def testLines2(self):
    self.session(self.lines2)
    self.assertLogFileContentsEqual(self.lines2Msg)
  #
  def testLines2Add3(self):
    self.session(self.lines2+self.add3)
    self.assertLogFileContentsEqual(self.lines2Msg+self.add3Msg)
  #
#

if __name__ == "__main__":
  unittest.main()
#
