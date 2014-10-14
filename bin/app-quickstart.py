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
def profileQuickStartParser() :

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
                      action="store",
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

#
def profileReplaceStrings( src, dst, options ) :
    with open( dst, "wt" ) as fout:
        with open( src, "rt" ) as fin:
            for line in fin:
                l = line.replace( '[LIST_OF_MODULES]', options.modules )
                l = l.replace( '[NAME_OF_APPLICATION]', options.name.upper() )
                l = l.replace( '<Name_of_Application>', options.name )
                l = l.replace( '(name_of_application)', options.name.lower() )
                fout.write( l )

#Generation of a template profile sources
def profileGenerateSources( options, args ) :

    #Set name of several directories
    app_dir = os.path.join( options.prefix, options.name )
    kernel_root_dir = os.environ["KERNEL_ROOT_DIR"]
    bin_salome_dir = os.path.join( kernel_root_dir, "bin", "salome" )
    template_dir = os.path.join( bin_salome_dir, "app-template"  )

    #Check if the directory of the sources already exists and delete it
    if os.path.exists( app_dir ) :
        if not options.force :
            print "Directory %s already exists." %app_dir
            print "Use option --force to overwrite it."
            return
        else :
            shutil.rmtree( app_dir )

    #Copy template directory
    os.mkdir( app_dir )
    for root, dirs, files in os.walk( template_dir ) :
        dst_dir = root.replace( template_dir, app_dir ) 
        for d in dirs :
            os.mkdir( os.path.join( dst_dir, d ) )
        for f in files :
            profileReplaceStrings( os.path.join( root, f ), os.path.join( dst_dir, f ), options )

    #Complete source directory
    contextFiles = [ "salomeContext.py", "salomeContextUtils.py", "parseConfigFile.py" ]
    for f in contextFiles :
        shutil.copy( os.path.join( bin_salome_dir, f ), os.path.join( app_dir, "src" ) )


# -----------------------------------------------------------------------------

if __name__ == '__main__':

    (options, args) = profileQuickStartParser().parse_args()

    profileGenerateSources( options, args )
