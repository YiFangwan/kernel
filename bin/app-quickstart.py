#! /usr/bin/env python
#  -*- coding: iso-8859-1 -*-
# Copyright (C) 2007-2014  CEA/DEN, EDF R&D, OPEN CASCADE
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
import shutil
import optparse

# Options of this script
def ProfileQuickStartParser() :

    parser = optparse.OptionParser( usage = "usage: python app-quickstart.py [options]" )

    parser.add_option('-p',
                      "--prefix",
                      metavar="</Path/to/the/sources/of/application>",
                      type="string",
                      action="store",
                      dest="prefix",
                      default='.',
                      help="Where the application's sources will be generated")

    parser.add_option('-m',
                      "--modules",
                      metavar="<module1,module2,...>",
                      type="string",
                      action="append",
                      dest="modules",
                      default='KERNEL,GUI',
                      help="List of the application's modules")

    parser.add_option('-n',
                      "--name",
                      type="string",
                      action="store",
                      dest="name",
                      default='PROFILE',
                      help="Name of the profile")

    parser.add_option('-f',
                      "--force",
                      action="store_true",
                      dest="force",
                      help="Overwrites existing sources")

    return parser


#Generation of a template profile sources
def generate_sources( options, args ) :

    #Set name of several directories
    app_dir = os.path.join( options.prefix, options.name )
    src_dir = os.path.join( app_dir, "src" )
    kernek_root_dir = os.environ["KERNEL_ROOT_DIR"]
    bin_dir = os.path.join( kernek_root_dir, "bin" )
    template_dir = os.path.join( bin_dir, "app-template"  )

    #Check if the directory of the sources already exists
    if os.path.exists( app_dir ) and not options.force :
        print "Directory %s already exists." %app_dir
        print "Use option --force to overwrite it."
        return

    #Copy template directory
    if os.path.exists( app_dir ) :
         shutil.rmtree( app_dir )
    shutil.copytree( template_dir, app_dir )

    #Complete source directory
    shutil.copy( os.path.join( kernek_root_dir, "LICENCE" ), app_dir )
    shutil.copy( os.path.join( bin_dir, "salomeContext.py" ), src_dir )
    shutil.copy( os.path.join( bin_dir, "parseConfigFile.py" ), src_dir )
    shutil.copy( os.path.join( bin_dir, "salomeContextUtils.py.in" ), os.path.join( src_dir, "salomeContextUtils.py" ) )

# -----------------------------------------------------------------------------

if __name__ == '__main__':

    (options, args) = ProfileQuickStartParser().parse_args()

    generate_sources( options, args )
