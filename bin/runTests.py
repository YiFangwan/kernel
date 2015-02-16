# Copyright (C) 2015 CEA/DEN, EDF R&D, OPEN CASCADE
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

import os
from optparse import OptionParser
from salomeContextUtils import getScriptsAndArgs, formatScriptsAndArgs

# Use to display newlines (\n) in epilog
class MyParser(OptionParser):
    def format_epilog(self, formatter):
        return self.epilog
#

class TestParameters:
  def __init__(self, resources):
    self.resources = resources
#

def configureTests(args=None, exe=None):
  if args is None:
    args = []
  if exe:
      usage = "Usage: %s [options] [components]"%exe
  else:
      usage = "Usage: %prog [options] [components]"
  epilog  = """\n
Run tests of SALOME components (KERNEL, YACS...). If components are not given, run tests of each avaible component.\n
* Tests can be restricted to a set of components.
* For each component, a subset of test files may be given using "with:" syntax.\n
Examples:
* Running all tests of all available components:
       salome test
* Running all tests of components KERNEL and YACS:
       salome test KERNEL YACS
* Running tests named test04 and test07 of component KERNEL:
       salome test KERNEL with:test04,test07
* Running tests named test04 and test07 of component KERNEL, and tests named test11 and test12 of component YACS:
       salome test KERNEL with:test04,test07 YACS with:test11,test12

Components must be separated by blank characters.
Tests files, if specified, must be comma-separated (without blank characters) and prefixed by "with:" keyword (without quotes).\n
"""
  parser = MyParser(usage=usage, epilog=epilog)
  parser.add_option("-r", "--resources", metavar="<resources>", default=0,
                    action="store", type="string", dest="resources",
                    help="A catalog of available resources (if a local test need to connect a remote machine)."
                    )
  try:
    (options, args) = parser.parse_args(args)
  except Exception, e:
    print e
    return

  params = TestParameters(options.resources)
  return params, args
#

class ComponentAndTests:
  # component: the component to be tested
  # tests: subset of component tests to be run
  def __init__(self, component=None, tests=None):
    self.component = component
    self.tests = tests
  #
  def __repr__(self):
    msg = "\n# Component: %s\n"%self.component
    msg += "     * Tests: %s\n"%self.tests
    return msg
  #
#

# Return an array of ComponentAndTests objects
def getComponentAndTestFiles(args=[]):
  # Syntax of args: component [with:a1,...,an] ... component [with:a1,...,an]
  compoTests = []
  currentKey = None
  withPrefix = "with:"

  for i in range(len(args)):
    elt = os.path.expanduser(args[i])

    if elt.startswith(withPrefix):
      if not currentKey:
        raise SalomeContextException("test files list must follow corresponding component in command line.")
      elt = elt.replace(withPrefix, '')
      compoTests[len(compoTests)-1].tests = elt.split(",")
      currentKey = None
    else:
      currentKey = elt
      compoTests.append(ComponentAndTests(component=currentKey))
  # end for loop
  return compoTests
#

def runTests(params, args):
  compoTests = getComponentAndTestFiles(args)
  print compoTests


  # :TODO: run tests



#
